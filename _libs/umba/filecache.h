#pragma once

//-----------------------------------------------------------------------------

/*! \file
    \brief Файловый кеш
*/

//-----------------------------------------------------------------------------

#include "umba/filename.h"
#include "umba/filesys.h"

#include <map>

//NOTE: !!! Not safe for threading


namespace umba
{


//! Файловый кэш
/*!
    Изначально предполагался для ускорения чтения C++ инклудов, а также фиксации состояния
    на момент первого использования, чтобы повторные подключения файла не получали другую версию
    (где необходимо, требуется указать флаг checkModified).

    Предполагается, что файлы текстовые и имеют разумный размер, и их разумное количество, для помещения их всех в оперативную
    память (если это не так, просто переходите на 64 бита).

    Энкодер требуется, если есть желание как-то обработать файл после чтения и до последующего парсинга - перекодировать, или 
    преобразовать в Unicode.

    Простой энкодер может просто перекидывать данные со входа на выход.

    Для экономии памяти энкодер может получать входные данные по ссылке и очищать входной вектор (и шринк-ту-фитить его).

    Единожды выданные FileId больше не меняются.

    Изначально файловый кэш предназначен только для чтения файлов, запись обратно не предусмотрена

    \tparam FilenameStringType   Тип строки имён файлов
    \tparam ByteType             Тип байта содержимого файла, обычно char/uint8_t
    \tparam DataType             Тип данных раскодированного файла, обычно uint8_t, возможно, будет wchar_t при перекодировке в Юникод
    \tparam EncoderType          Тип энкодера. Обычно достаточно простого проброса, но он может, например, исправлять кодировку текста или перекодировать в Юникод
    \tparam FileIdType           Тип идентификатора файла
    \tparam UserDataType         Пользовательский тип, ассоциированный с файлом

    \todo
    //TODO: Хочется запилить версию для MCU, чтобы использовать единый API файлового кэша

    \todo
    //TODO: Нужно отвязать от вызовов umba::filesys и umba::filename, параметризовав данный класс соответствующими API объектами 

    \todo
    //TODO: От MCU требуется наличие большого куска оперативы.
            Если потребуется обрабатывать файлы, которые не лезут в оперативу, то надо подумать.


 */
template< typename FilenameStringType
        , typename ByteType
        , typename DataType
        , typename EncoderType
        , typename FileIdType
        , typename UserDataType = unsigned /* dummy */
        >
class FileCache
{

public:

    typedef FilenameStringType         FilenameStringType;  //!< Тип строки имён файлов                                                                               
    typedef ByteType                   ByteType;            //!< Тип байта содержимого файла, обычно char/uint8_t                                                     
    typedef DataType                   DataType;            //!< Тип данных раскодированного файла, обычно uint8_t, возможно, будет wchar_t при перекодировке в Юникод
    typedef std::vector<ByteType>      ByteVectorType;      //!< Вектор исходных данных файла
    typedef std::vector<DataType>      DataVectorType;      //!< Вуктор перекодированных данных файла
    typedef FileIdType                 FileIdType;          //!< Тип идентификатора файла

    static const FileIdType            invalidFileId = (FileIdType)-1; //!< Неверный/недопустимый идентификатор

    //! Информация по кешированному файлу 
    struct FileCacheInfo
    {
        FilenameStringType                 orgFilename;      //!< Исходное имя файла
        FilenameStringType                 ntvFilename;      //!< Нативное имя файла 
        FilenameStringType                 cmpFilename;      //!< Имя файла для сравнения
        umba::filesys::FileStat            fileStat;         //!< Статистика по файлу - размер, дата рождения и тп
        
        mutable ByteVectorType             originalFileData; //!< Исходные данные файла
        mutable DataVectorType             encodedFileData ; //!< Перекодированные данные файла

        FileIdType                         fileId;           //!< Идентификатор файла

        mutable UserDataType               userData;         //!< Пользовательские данные

        //! Копирование только имён
        void copyNames( const FileCacheInfo &fciFrom )
        {
            orgFilename = fciFrom.orgFilename;
            ntvFilename = fciFrom.ntvFilename;
            cmpFilename = fciFrom.cmpFilename;
        }

    };


    typedef FileCacheInfo FileInfo; //!< Алиас для обращения как к FileCache::FileInfo


protected:

    std::map<FilenameStringType, FileCacheInfo> m_cache;            //!< Собственно, кэш \\todo //TODO: Переделать, сделать ключем FileId!!!
    EncoderType                                 m_encoder;          //!< Энкодер

    FileIdType                                  m_fileIdCounter;    //!< Счетчик для генерации FileId
    std::map<FilenameStringType, FileIdType>    m_fileIdMap;        //!< Мапа для поиска FileId по имени
    std::map<FileIdType, FilenameStringType>    m_fileIdMapToName;  //!< Мапа для поиска имени по FileId 

    //------------------------------
    //! Хелпер для формирования абсолютного имени
    FilenameStringType makeAbsName(const FilenameStringType &fileName, FilenameStringType curDir = FilenameStringType()) const
    {
        using namespace umba::filename;
        using namespace umba::filesys;

        if (curDir.empty())
            curDir = umba::filesys::getCurrentDirectory<FilenameStringType>();

        // FilenameStringType resName;
        FilenameStringType resName = fileName;
        resName = umba::filename::makeCanonical( resName );
        //

        if (!isAbsPath(fileName))
        {
            // resName = appendPath(curDir,fileName);
            resName = appendPath(umba::filename::makeCanonical(curDir),resName);
        }

        resName = umba::filename::makeCanonical( resName );

        resName = checkPathPrependDrive<FilenameStringType>(resName, curDir);

        return umba::filename::makeCanonical<FilenameStringType>(resName);
        
        //return resName;
    }

    //------------------------------
    FilenameStringType makeCanonicalForCompareFromFullName( const FilenameStringType &name ) const { return umba::filename::makeCanonicalForCompare<FilenameStringType>(name); } //!< Делает "каноническое" имя для сравнения
    FilenameStringType makeNativeFileSysFromFullName      ( const FilenameStringType &name ) const { return umba::filename::prepareForNativeUsage(name); }                       //!< Подготавливает имя для "нативного" использования - для передачи имени в системные API

    //------------------------------
    //! Заполняет поля имён FileCacheInfo - готовит его для чтения данных
    FileCacheInfo makeFileInfoForReading( const FilenameStringType &fileName, const FilenameStringType &curDir = FilenameStringType() ) const
    {
        using namespace umba::filename;
        using namespace umba::filesys;

        FileCacheInfo newFileInfo;

        newFileInfo.orgFilename = makeAbsName( fileName, curDir );
        newFileInfo.cmpFilename = makeCanonicalForCompareFromFullName(newFileInfo.orgFilename);
        newFileInfo.ntvFilename = makeNativeFileSysFromFullName( newFileInfo.orgFilename );

        newFileInfo.fileId = invalidFileId;

        return newFileInfo;
    }

    //------------------------------
    //! Читает статистику файла
    umba::filesys::FileStat readFileStat( const FileCacheInfo &fileInfo )
    {
        return umba::filesys::getFileStat( fileInfo.ntvFilename );
    }

    //------------------------------
    //! Реализация получения идентификатора файла
    FileIdType getFileIdImpl( const FilenameStringType &nameCanonicalForCompare //!< Каноническое сравнибельное имя файла
                            , bool  allowCreateNewId                            //!< Можно ли создавать новый ID, если файлу ранее ID не выдавался
                            )
    {
        std::map<FilenameStringType, FileIdType>::const_iterator it = m_fileIdMap.find(nameCanonicalForCompare);
        if (it!=m_fileIdMap.end())
        {
            return it->second;
        }

        if (!allowCreateNewId)
            return invalidFileId;

        FileIdType newFileId = ++m_fileIdCounter; // we keep 0 as marker value

        m_fileIdMap[nameCanonicalForCompare] = newFileId;
        m_fileIdMapToName[newFileId]         = nameCanonicalForCompare;

        return newFileId;
    }



public:

    //! Конструктор по умолчанию, получает только энкодер
    FileCache( const EncoderType &encoder ) 
        : m_cache(), m_encoder(encoder), m_fileIdCounter(0), m_fileIdMap(), m_fileIdMapToName() {}

    //! Конструктор копирования
    FileCache( const FileCache &fileCache ) 
        : m_cache(fileCache.m_cache), m_encoder(fileCache.m_encoder), m_fileIdCounter(fileCache.m_fileIdCounter), m_fileIdMap(fileCache.m_fileIdMap), m_fileIdMapToName(fileCache.m_fileIdMapToName) {}

    //! Конструктор копрования с заменой энкодера
    FileCache( const FileCache &fileCache
             , const EncoderType &encoder ) 
        : m_cache(fileCache.m_cache), m_encoder(encoder), m_fileIdCounter(fileCache.m_fileIdCounter), m_fileIdMap(fileCache.m_fileIdMap), m_fileIdMapToName(fileCache.m_fileIdMapToName) {}


protected:

    //! Реализация получения FileCacheInfo* по имени файла
    FileCacheInfo* getFileCacheInfoImpl( const FilenameStringType &fileName                       //!< Имя файла
                                       , bool  checkModified                                      //!< Проверять, был ли модифицирован (и перечитать, если был)
                                       , const FilenameStringType  &curDir = FilenameStringType() //!< Текущий каталог
                                       )
    {
        using namespace umba::filename;
        using namespace umba::filesys;

        FileCacheInfo newFileInfo = makeFileInfoForReading( fileName, curDir );


        std::map<FilenameStringType, FileCacheInfo>::iterator it = m_cache.find(newFileInfo.cmpFilename);

        if (it==m_cache.end()) // !!! Not found
        {
            if (!readFile<FilenameStringType, ByteType>(newFileInfo.ntvFilename, newFileInfo.originalFileData , &newFileInfo.fileStat, false  /* !ignoreSizeErrors */ ))
                return 0;

            newFileInfo.encodedFileData = m_encoder(newFileInfo.originalFileData);
            newFileInfo.fileId          = getFileIdImpl( newFileInfo.cmpFilename, true  /* allowCreateNewId */ );

            m_cache[newFileInfo.cmpFilename] = newFileInfo;

            it = m_cache.find(newFileInfo.cmpFilename);

            checkModified = false;
        }

        if (checkModified)
        {
            FileStat fstat = readFileStat( newFileInfo );
            if (fstat.fileType==it->second.fileStat.fileType && fstat.timeLastModified!=it->second.fileStat.timeLastModified)
            {
                FileCacheInfo &fileInfo = it->second;
                if (!readFile<FilenameStringType>(newFileInfo.ntvFilename, fileInfo.originalFileData, &fileInfo.fileStat, false  /* !ignoreSizeErrors */ ))
                {
                    m_cache.erase(it);
                    return 0;
                }

                fileInfo.encodedFileData = m_encoder(fileInfo.originalFileData);
                fileInfo.fileStat = fstat;
            }
        }

        FileCacheInfo &fileInfo = it->second;

        //if (pFinalFilename)
        //   *pFinalFilename = fileInfo.orgFilename;

        return &fileInfo;
    }


public:

    //! Возвращает FileId по имени файла, или invalidFileId, если такой файл не кэширован
    FileIdType getFileId( const FilenameStringType &fileName, const FilenameStringType  &curDir = FilenameStringType() )
    {
        FileCacheInfo newFileInfo = makeFileInfoForReading( fileName, curDir );
        return getFileIdImpl( newFileInfo.cmpFilename, false /* allowCreateNewId */ );
    }

    //! Производит поиск FileId по имени файла, если файл не кеширован, производит его поиск на диске, выдаёт ID и кеширует
    FileIdType findFileId( const FilenameStringType &fileName, bool checkModified, const FilenameStringType  &curDir = FilenameStringType() )
    {
        FileCacheInfo* pCacheInfo = getFileCacheInfoImpl( fileName
                                                        , checkModified
                                                        , curDir
                                                        );

        if (!pCacheInfo)
            return invalidFileId;

        return pCacheInfo->fileId;
    }

    //! Возвращает запись с информацией о файле по заданному ID
    const FileCacheInfo* getFileInfo( FileIdType fileId ) const
    {
        std::map<FileIdType, FilenameStringType>::const_iterator idIt = m_fileIdMapToName.find(fileId);
        if (idIt==m_fileIdMapToName.end()) // no such ID
            return 0;

        const FilenameStringType &name = idIt->second;

        std::map<FilenameStringType, FileCacheInfo>::const_iterator cacheIt = m_cache.find(name);
        if (cacheIt==m_cache.end())
            return 0;

        return &cacheIt->second;
    }

    //! Возвращает запись с информацией о файле
    const FileCacheInfo* getFileInfo( const FilenameStringType &fileName
                                    , bool checkModified
                                    , const FilenameStringType  &curDir = FilenameStringType()
                                    )
    {
        return getFileCacheInfoImpl( fileName, checkModified, curDir );
    }

    /*
    const std::vector<ByteType>* getFileData( const FilenameStringType &fileName, bool checkModified, FilenameStringType *pFinalFilename = 0 )
    {
        const FileCacheInfo *pFileCacheInfo = getFileCacheInfo( fileName, checkModified, pFinalFilename );
        if (!pFileCacheInfo)
            return 0;
        return &(pFileCacheInfo->fileData);
    }
    */

    //! Возвращает статистику по файлу - данные о датах модификации, и тп
    umba::filesys::FileStat getFileStat( const FilenameStringType &fileName ) const
    {
        return readFileStat( makeFileInfoForReading( fileName ) );
    }
    
    //! Возвращает кешированную статистику по файлу
    const umba::filesys::FileStat* getCachedFileStat( const FilenameStringType &fileName ) const
    {
        using umba::filename;

        FileCacheInfo newFileInfo = makeFileInfoForReading( fileName );

        std::map<FilenameStringType, FileCacheInfo>::const_iterator it = m_cache.find(newFileInfo.cmpFilename);

        if (it==m_cache.end())
            return 0;
        
        const FileCacheInfo &fileInfo = it->second;

        return &fileInfo.fileStat;
    }

/*
    const std::vector<ByteType>* forceReload( const FilenameStringType &fileName )
    {
        return getFileData( fileName, true );
    }
*/

    //! Возвращает true если файл не наден в кеше или модифицирован
    bool isModified( const FilenameStringType &fileName ) const
    {
        using umba::filename;

        FileCacheInfo newFileInfo = makeFileInfoForReading( fileName );

        std::map<FilenameStringType, FileCacheInfo>::const_iterator it = m_cache.find(newFileInfo.cmpFilename);

        if (it==m_cache.end())
            return true;

        umba::filesys::FileStat actualFileStat = getFileStat( newFileInfo );

        if (actualFileStat.timeLastModified!=it->second.timeLastModified)
            return true;

        return false; // exist in cache and not modified since last read
    }

}; // class FileCache

/*
struct FileStat
{
    FileType     fileType;
    filesize_t   fileSize;
    filetime_t   timeCreation    ;
    filetime_t   timeLastModified;
    filetime_t   timeLastAccess  ;

    bool isValid() const { return fileType!=FileTypeInvalid; }
    bool isDir()   const { return fileType==FileTypeDir    ; }
    bool isFile()  const { return fileType==FileTypeFile   ; }
*/




} // namespace umba

