#pragma once

//-----------------------------------------------------------------------------

/*! \file
    \brief Поиск подключаемых файлов
*/

//-----------------------------------------------------------------------------

#include "umba/umba.h"
#include "umba/stl.h"
#include "umba/filename.h"
#include "umba/filesys.h"
#include "umba/env.h"

#include <string>
#include <map>
#include <vector>
#include <set>

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
namespace umba
{



//-----------------------------------------------------------------------------
//! Класс для поиска подключаемых файлов
/*!
    Использует понятие "уровень подключения", для каждого уровня задаётся отдельный набор путей поиска.
    Для каждого уровня подключения задаётся список дополнительных уровней подключения, которые будут
    использоваться, если поиск на текущем уровне подключения окончился неудачей.

    Обращение к файловой системе производится через абстракцию FileCache.

    Для упрощения поиска можно для каждого уровня подключения задать шорткат - символ,
    который может открывать или закрывать имя искомого файла. Так, для C/C++ символы '<' и '>' можно
    сделать шорткатами для SystemLevel, а символ '\\"' - шорткатом для UserLevel.

    Некоторые термины. 
    \b Базовый \b файл (base file) - это файл, из которого производится инклюд.
    \b Базовый \b каталог - это каталог, в котором ф котором расположен файл, из которого производится инклюд.
    \b Base \b name - это имя базового файла или каталога. Если имя заканчивается символом-разделителем пути - это каталог, иначе - файл.

    \tparam FileCache          Тип файлового кэша
    \tparam FilenameStringType Тип строки имён файлов
    \tparam IncludeLevelsType  Тип "уровень подключения" - для C/C++ достаточно bool для разделения системных и пользовательских инклудов.
 */
template<typename FileCache, typename FilenameStringType, typename IncludeLevelsType >
class IncludeFinder
{

public:

    //typedef typename FileCache::ValueVectorType  FileDataVectorType;
    typedef typename FileCache::FileIdType         FileIdType;           //!< Тип идентификатора файла
    typedef typename FileCache::FileCacheInfo      FileInfoType;         //!< Тип инфы о файле
    typedef FilenameStringType                     FilenameStringType;   //!< Тип строки с именем файла

    static const FileIdType invalidFileId = FileCache::invalidFileId;    //!< Инвалидный идентификатор файла

protected:

    //! Тип инклуда
    struct IncludeTypeInfo
    {
        std::vector<IncludeLevelsType>       additionalLookupOrder;  //!< Дополнительные уровни поиска
        std::vector<FilenameStringType>      lookupDirs;             //!< Список каталогов для поиска на текущем уровне
    };

    typedef typename FilenameStringType::value_type  CharType;       //!< Тип символов (в именах файлов)

    FileCache                                       *m_pCache;       //!< Указатель на объект кэша файлов
    std::map< IncludeLevelsType, IncludeTypeInfo >   m_lookupMap;    //!< Мапа поиска по уровням инклудов
    std::map< CharType, IncludeLevelsType >          m_easyMarkers;  //!< Мапа маркеров-шорткатов


public:

    //! Конструктор, принимает указатель на файловый кэш
    IncludeFinder( FileCache *pCache ) 
    : m_pCache(pCache)
    , m_lookupMap()
    , m_easyMarkers()
    {}

    //! Конструктор копирования
    IncludeFinder( const IncludeFinder &finder ) 
    : m_pCache(finder.m_pCache)
    , m_lookupMap(finder.m_lookupMap)
    , m_easyMarkers(finder.m_easyMarkers)
    {}

    //! Конструктор копирования с заменой файлового кэша
    IncludeFinder( const IncludeFinder &finder, FileCache *pCache ) 
    : m_pCache(pCache)
    , m_lookupMap(finder.m_lookupMap)
    , m_easyMarkers(finder.m_easyMarkers)
    {}


    //------------------------------
    // Интерфейс настройки
    //------------------------------

    //! Очистка/сброс
    void clear()
    {
        m_lookupMap  .clear();
        m_easyMarkers.clear();
    }

    //! Добавляет путь поиска для заданного инклуд уровня
    void addLookupPath( IncludeLevelsType lvl, const FilenameStringType &p )
    {
        m_lookupMap[lvl].lookupDirs.push_back(p);
    }

    //! Добавляет пути поиска для заданного инклуд уровня
    void addLookupPaths( IncludeLevelsType lvl, const std::vector<FilenameStringType> &pl )
    {
        std::vector<FilenameStringType> &dirs = m_lookupMap[lvl].lookupDirs;
        dirs.insert( dirs.end(), pl.begin(), pl.end() );
    }

    //! Добавляет пути поиска для заданного инклуд уровня, пути задаются в одной строке
    /*! Разделителем является спец символ:
        - Win32 - ';' (точка с запятой, semicolon)
        - Linux - ':' (двоеточие, colon)
     */
    void addLookupPaths( IncludeLevelsType lvl, const FilenameStringType &pl )
    {
        addLookupPaths( umba::filename::splitPathList( pl ) );
    }

    //! Копирует лукап пути из одного инклюд уровня в другой
    void addLookupPaths( IncludeLevelsType lvl, IncludeLevelsType lvlCopyFrom )
    {
        std::map< IncludeLevelsType, IncludeTypeInfo >::iterator lvlIt = m_lookupMap.find(lvlCopyFrom);

        if (lvlIt == m_lookupMap.end())
            return; // lookup level not found

        addLookupPaths( lvl, lvlIt->second.lookupDirs );
    }

    //! Добавляет лукап путь, равный алиасу текущего каталога - обычно это строка "."
    /*! При поиске такой алис будет раскрыт в имя каталога, в котором расположен текущий базовый файл.
     */
    void addBaseLookupPath(IncludeLevelsType lvl)
    {
        addLookupPath( lvl, umba::filename::getNativeCurrentDirAlias<FilenameStringType>() );
    }

    //! Добавляет порядок поиска к текущему инклюд уровню
    void addLookupOrder( IncludeLevelsType lvl, IncludeLevelsType additionalOrder )
    {
        if (lvl==additionalOrder) return;
        m_lookupMap[lvl].additionalLookupOrder.push_back(additionalOrder);
    }

    void addLookupOrder( IncludeLevelsType lvl, IncludeLevelsType a1, IncludeLevelsType a2 )  { addLookupOrder(lvl, a1); addLookupOrder(lvl, a2); }                                                                                                                                                //!< Добавляет порядок поиска к текущему инклюд уровню
    void addLookupOrder( IncludeLevelsType lvl, IncludeLevelsType a1, IncludeLevelsType a2, IncludeLevelsType a3 )  { addLookupOrder(lvl, a1); addLookupOrder(lvl, a2); addLookupOrder(lvl, a3); }                                                                                                 //!< Добавляет порядок поиска к текущему инклюд уровню
    void addLookupOrder( IncludeLevelsType lvl, IncludeLevelsType a1, IncludeLevelsType a2, IncludeLevelsType a3, IncludeLevelsType a4 )  { addLookupOrder(lvl, a1); addLookupOrder(lvl, a2); addLookupOrder(lvl, a3); addLookupOrder(lvl, a4); }                                                  //!< Добавляет порядок поиска к текущему инклюд уровню
    void addLookupOrder( IncludeLevelsType lvl, IncludeLevelsType a1, IncludeLevelsType a2, IncludeLevelsType a3, IncludeLevelsType a4, IncludeLevelsType a5 )  { addLookupOrder(lvl, a1); addLookupOrder(lvl, a2); addLookupOrder(lvl, a3); addLookupOrder(lvl, a4); addLookupOrder(lvl, a5); }   //!< Добавляет порядок поиска к текущему инклюд уровню

    //! Добавляет порядок поиска к текущему инклюд уровню
    void addLookupOrder( IncludeLevelsType lvl, const std::vector<IncludeLevelsType> &additionalOrder )
    {
        std::vector<IncludeLevelsType>::const_iterator it = additionalOrder.begin();
        for(; it != additionalOrder.end(); ++it)
            addLookupOrder(lvl, *it);
    }

    //! Добавляет лукап шорткат для уровня поиска 
    void addLookupShortcut( IncludeLevelsType lvl, CharType braceCharShortcut )
    {
        m_easyMarkers[braceCharShortcut] = lvl;
    }

    //! Устанавливает уровень инклуда по умолчанию. Используется, если уровень инклуда не задан явно, и в переданном имени не обнаружен шорткат
    void setLevelForDefaultLookup( IncludeLevelsType lvl )
    {
        m_easyMarkers[(CharType)0] = lvl;
    }


    //------------------------------
    // Интерфейс поиска
    //------------------------------


    //------------------------------
    //! Поиск инклуда с явным указанием уровня инклуда. При этом имя искомого файла должно быть очищено от всевозможных скобок и прочего
    FileIdType findFile( const FilenameStringType &lookupFor
                       , const FilenameStringType &baseName    // File Name included from, or path with slash at end
                       , IncludeLevelsType         lookupLvlType
                       , bool                      checkModified = false
                       )
    {
        FilenameStringType basePath = umba::filename::hasLastPathSep(baseName)
                                    ? baseName
                                    : baseName.empty() ? baseName : umba::filename::appendPathSepCopy<FilenameStringType>(umba::filename::getPath<FilenameStringType>(baseName))
                                    ;

        // Тут, для начала, надо проверить, не задан ли абсолютный путь
        // и попробовать открыть его
        if (umba::filename::isAbsPath(lookupFor))
        {
            return m_pCache->findFileId( lookupFor, checkModified, basePath /* curDir */ );
        }
        

        std::map< IncludeLevelsType, IncludeTypeInfo >::iterator lvlIt = m_lookupMap.find(lookupLvlType);

        if (lvlIt == m_lookupMap.end())
            return invalidFileId; // lookup level not found

        IncludeTypeInfo &includeTypeInfo = lvlIt->second;

                                    
        // basePath has path sep as last symbol here

        std::vector<IncludeLevelsType> lvlOrder;
        lvlOrder.push_back(lookupLvlType);
        lvlOrder.insert( lvlOrder.end(), includeTypeInfo.additionalLookupOrder.begin(), includeTypeInfo.additionalLookupOrder.end() );

        // Сделали список лвл

        std::vector<FilenameStringType> lookupDirs;
        {
            //std::set<FilenameStringType>    alreadyUsedLookupDirs;
            std::set<IncludeLevelsType>     alreadyUsedLvls;
           
            // Делаем список каталогов, в которых ищем файл, по использованным levels
            std::vector<IncludeLevelsType>::const_iterator lvlOrdIt = lvlOrder.begin();
            for(; lvlOrdIt!=lvlOrder.end(); ++lvlOrdIt)
            {
                if (alreadyUsedLvls.find(*lvlOrdIt)!=alreadyUsedLvls.end())
                    continue;
           
                alreadyUsedLvls.insert(*lvlOrdIt);
           
                std::map< IncludeLevelsType, IncludeTypeInfo >::const_iterator lvlIt = m_lookupMap.find(*lvlOrdIt);
                if (lvlIt == m_lookupMap.end())
                    continue;
           
                std::vector<FilenameStringType>::const_iterator ldIt = lvlIt->second.lookupDirs.begin();
                for(; ldIt!=lvlIt->second.lookupDirs.end(); ++ldIt)
                {
                    FilenameStringType lkpDir = *ldIt;
                    if (lkpDir==umba::filename::getNativeCurrentDirAlias<FilenameStringType>())
                    {
                       if (!basePath.empty())
                           lookupDirs.push_back(basePath);
                    }
                    else if ( umba::string_plus::unquote_if_quoted(lkpDir, umba::string_plus::make_string<FilenameStringType>("%"))     // Windows style env var - %VARNAME%
                           || umba::string_plus::unquote_if_quoted(lkpDir, umba::string_plus::make_string<FilenameStringType>("$"))     // Combo - Unix like Windows style env var - $VARNAME$
                           || umba::string_plus::starts_with_and_strip(lkpDir, umba::string_plus::make_string<FilenameStringType>("$")) // Unix style env var - $VARNAME
                            ) 
                    {
                        FilenameStringType pathListStr;
                        if ( umba::env::getVar(lkpDir,pathListStr) )
                        {
                            std::vector<FilenameStringType> paths = umba::filename::splitPathList( pathListStr );
                            lookupDirs.insert( lookupDirs.end(), paths.begin(), paths.end() );
                        }
                    }
                    else
                    {
                       lookupDirs.push_back(*ldIt);
                    }
                }
            }
        }


        std::vector<FilenameStringType>::const_iterator ldIt = lookupDirs.begin();
        for(; ldIt!=lookupDirs.end(); ++ldIt)
        {
            FilenameStringType testName = umba::filename::appendPath( *ldIt, lookupFor );
            //const FileDataVectorType *pData = m_pCache->getFileData( testName, checkModified, &foundName );
            FileIdType fileId = m_pCache->findFileId( testName, checkModified, basePath /* curDir */ );
            
            if (fileId!=invalidFileId)
               return fileId;
        }

        return invalidFileId;
    }

    //------------------------------
    //! Поиск инклуда с детектом уровня инклуда.
    FileIdType findFile( FilenameStringType        lookupFor
                       , const FilenameStringType &baseName    // File Name included from, or path with slash at end
                       , bool  checkModified = false
                       )
    {
        IncludeLevelsType foundLvl = (IncludeLevelsType)-1;

        namespace ustrp = umba::string_plus;

        std::map< CharType, IncludeLevelsType >::const_iterator mit = m_easyMarkers.begin();

        for(; mit!=m_easyMarkers.end(); ++mit)
        {
            CharType quotChar = mit->first;

            if (quotChar==(CharType)0)
            {
                foundLvl = mit->second;
                continue;
            }

            if (ustrp::unquote_if_quoted(lookupFor, quotChar))
            {
                foundLvl = mit->second;
                break;
            }
        }

        return findFile( lookupFor, baseName, foundLvl, checkModified );
    }

    //------------------------------
    //! Поиск инклуда с детектом уровня инклуда, без указания базового файла/каталога.
    FileIdType findFile( FilenameStringType lookupFor
                       , bool               checkModified = false
                       )
    {
        return findFile( foundName, lookupFor, checkModified, FilenameStringType() /* baseName */ );
    }

    //------------------------------
    //! Возвращает FileInfo по идентификатору файла
    const FileInfoType* getFileInfo( FileIdType fileId )
    {
        return m_pCache->getFileInfo( fileId );
    }

};






} // namespace umba


