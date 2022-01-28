#pragma once

//-----------------------------------------------------------------------------

/*! \file
    \brief Утилиты для работы с именами файлов
*/

//-----------------------------------------------------------------------------

#include "umba/stl.h"
#include "umba/string_plus.h"
#include "umba/env.h"
#include "umba/critical_section.h"
#include "umba/filesys.h"


#include <algorithm>

//#include "splits.h"
//#include "isa.h"

#include "zz_inc_windows.h"


// umba::filename::
namespace umba
{
namespace filename
{


//-----------------------------------------------------------------------------
template<typename StringType> inline  StringType getNativeParentDirAlias( )  { return umba::string_plus::make_string<StringType>(".."); } //!< Возвращает строку с алиасом родительского каталога, как это принято в целевой системе. Обычно это ".."
template<typename StringType> inline  StringType getNativeCurrentDirAlias( ) { return umba::string_plus::make_string<StringType>(".") ; } //!< Возвращает строку с алиасом текущего каталога, как это принято в целевой системе. Обычно это "."
template<typename StringType> inline  StringType getNativeHomeDirAlias( )    { return umba::string_plus::make_string<StringType>("~") ; } //!< Возвращает строку с алиасом домашнего каталога текущего пользователя

template<typename CharType>   inline CharType getNativeExtSep( )    { return (CharType)'.'; } //!< Возвращает символ - разделитель расширения файла
//template<typename StringType> inline StringType getNativeExtSep( )  { return StringType(1, getNativeExtSep<typename StringType::value_type>() ); }

//! Возвращает символ - разделитель имён файлов в пути
template<typename CharType>
inline CharType getNativePathSep( )
{
    #if defined(WIN32) || defined(_WIN32)
    return (CharType)'\\';
    #else
    return (CharType)'/';
    #endif
}

//! Возвращает символ - разделитель путей в списке
template<typename CharType>
inline CharType getNativePathListSep( )
{
    #if defined(WIN32) || defined(_WIN32)
    return (CharType)';';
    #else
    return (CharType)':';
    #endif
}

//template<typename StringType> inline StringType getNativePathSep( )      { return StringType(1, getNativePathSep<typename StringType::value_type>() ); }
//template<typename StringType> inline StringType getNativePathListSep( )  { return StringType(1, getNativePathListSep<typename StringType::value_type>() ); }


//-----------------------------------------------------------------------------
//! Возвращает UNC префикс
template<typename StringType> inline
StringType getNativeUncPrefix()
{
    #if defined(WIN32) || defined(_WIN32)
    return umba::string_plus::make_string<StringType>("\\\\?\\");
    #else
    return StringType();
    #endif
}

//! Возвращает сетевой UNC префикс
template<typename StringType> inline
StringType getNativeNetworkUncPrefix()
{
    #if defined(WIN32) || defined(_WIN32)
    return umba::string_plus::make_string<StringType>("\\\\?\\UNC\\");
    #else
    return StringType();
    #endif
}

//-----------------------------------------------------------------------------
//! Возвращает сетевой префикс
template<typename StringType> inline
StringType getNativeNetworkPathPrefix()
{
    return umba::string_plus::make_string<StringType>( getNativePathSep<typename StringType::value_type>(), 2 );
}

//! Возвращает true, если путь начинается с сетевого префикса
template<typename StringType> inline
bool hasNativeNetworkPathPrefix( StringType &p )
{
    namespace ustrp = umba::string_plus;
    if (ustrp::starts_with(p, getNativeNetworkPathPrefix<StringType>()))
        return true;
    return false;
}

//! Возвращает true, если путь начинается с UNC префикса, обрезая его
template<typename StringType> inline
StringType stripNativeUncPrefix( StringType &p )
{
    namespace ustrp = umba::string_plus;
    ustrp::starts_with_and_strip(p, getNativeNetworkUncPrefix<StringType>());
    ustrp::starts_with_and_strip(p, getNativeUncPrefix<StringType>());
    return p;
}

//-----------------------------------------------------------------------------
template<typename CharType> inline bool isExtSep ( CharType ch ) { return ch==(CharType)'.'; }                        //!< Возвращает true, если символ - разделитель расширения
template<typename CharType> inline bool isPathSep( CharType ch ) { return ch==(CharType)'/' || ch==(CharType)'\\'; }  //!< Возвращает true, если символ - разделитель пути

//-----------------------------------------------------------------------------
template<typename StringType> inline bool hasLastPathSep( StringType &p )   { return (p.empty() || !isPathSep(p[p.size()-1])) ? false : true; }                                              //!< Возвращает true, если последний символ - разделитель пути
template<typename StringType> inline bool stripLastPathSep( StringType &p ) { if (hasLastPathSep(p)) { p.erase( p.size()-1, 1 ); return true; } return false; }                              //!< Возвращает true, если последний символ - разделитель пути, обрезая его
template<typename StringType> inline StringType stripLastPathSepCopy( const StringType &p ) { if (!hasLastPathSep(p)) return p; StringType res = p; stripLastPathSep(res); return res; }     //!< Возвращает копию аргумента, обрезая разделитель пути в конце, если он есть
template<typename StringType> inline bool hasFirstPathSep( StringType &p )  { return (p.empty() || !isPathSep(p[0])) ? false : true; }                                                       //!< Возвращает true, если первый символ - разделитель пути
template<typename StringType> inline bool stripFirstPathSep(StringType &p)  { if (hasFirstPathSep(p)) p.erase( 0, 1 ); return true; }                                                        //!< Возвращает true, если первый символ - разделитель пути, обрезая его
template<typename StringType> inline StringType stripFirstPathSepCopy( const StringType &p ) { if (!hasFirstPathSep(p)) return p; StringType res = p; stripFirstPathSep(res); return res; }  //!< Возвращает копию аргумента, обрезая разделитель пути в начале, если он есть

template<typename StringType> inline bool hasLastExtSep( StringType &p )    { return (p.empty() || !isExtSep(p[p.size()-1])) ? false : true; }                                               //!< Возвращает true, если последний символ - разделитель расширения                  
template<typename StringType> inline bool stripLastExtSep( StringType &p )  { if (hasLastExtSep(p)) { p.erase( p.size()-1, 1 ); return true; } return false; }                               //!< Возвращает true, если последний символ - разделитель расширения, обрезая его     
template<typename StringType> inline StringType stripLastExtSepCopy( const StringType &p ) { if (!hasLastExtSep(p)) return p; StringType res = p; stripLastExtSep(res); return res; }        //!< Возвращает копию аргумента, обрезая разделитель расширения в конце, если он есть 
template<typename StringType> inline bool hasFirstExtSep( StringType &p )   { return (p.empty() || !isExtSep(p[0])) ? false : true; }                                                        //!< Возвращает true, если первый символ - разделитель расширения                     
template<typename StringType> inline bool stripFirstExtSep(StringType &p)   { if (hasFirstExtSep(p)) { p.erase( 0, 1 ); return true; } return false; }                                       //!< Возвращает true, если первый символ - разделитель расширения, обрезая его        
template<typename StringType> inline StringType stripFirstExtSepCopy( const StringType &p ) { if (!hasFirstExtSep(p)) return p; StringType res = p; stripFirstExtSep(res); return res; }     //!< Возвращает копию аргумента, обрезая разделитель расширения в начале, если он есть



//-----------------------------------------------------------------------------
//! Разделяет список путей
template<typename StringType> inline
std::vector< StringType > splitPathList( const StringType &pl, typename StringType::value_type pathListSep = getNativePathListSep<typename StringType::value_type>() )
{
    return umba::string_plus::split( pl, pathListSep, true /* skipEmpty */ );
}

/*
template<typename StringType> inline
std::vector< StringType > getKeepCasePaths()
{
    static bool envReaded = false;
    static std::vector< StringType >  pathCache;

    if (!envReaded)
    {
        UMBA_CRITICAL_SECTION_EX( umba::CriticalSection, globalCriticalSection);
        if (!envReaded)
        {
        }
    }
}
*/

//-----------------------------------------------------------------------------
//! Добавляет разделитель пути, если его нет в конце
template<typename StringType> inline
void appendPathSepInline( StringType &p, typename StringType::value_type pathSep = getNativePathSep<typename StringType::value_type>() )
{
    if (!hasLastPathSep<StringType>(p)) p.append(1,pathSep);
}

//-----------------------------------------------------------------------------
//! Добавляет разделитель пути в возвращаемую копию аргумента, если его нет в конце
template<typename StringType> inline
StringType appendPathSepCopy( const StringType &p, typename StringType::value_type pathSep = getNativePathSep<typename StringType::value_type>() )
{
    StringType pCopy = p; appendPathSepInline<StringType>(pCopy); return pCopy;
}

//-----------------------------------------------------------------------------
//! Нормализует разделители путей
template<typename StringType> inline
StringType normalizePathSeparators( StringType fileName, typename StringType::value_type pathSep = getNativePathSep<typename StringType::value_type>() )
{
    for(auto &ch : fileName)
    {
        if (isPathSep(ch))
            ch = pathSep;
    }

    return fileName;
}

//-----------------------------------------------------------------------------
//! Возвращает true, если в пути содержаться разделители пути
template<typename StringType> inline
StringType hasPathSeparators( const StringType &fileName )
{
    for(auto ch : fileName)
    {
        if (isPathSep(ch))
            return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
//! Делает "каноническое" имя, схлопывая все лишние алиасы (".." и ".")
template<typename StringType> inline
StringType makeCanonical( StringType fileName, typename StringType::value_type pathSep = getNativePathSep<typename StringType::value_type>() )
{
    std::replace_if( fileName.begin(), fileName.end(), isPathSep<typename StringType::value_type>, pathSep );

    namespace ustrp = umba::string_plus;

    bool hasNetworkUncPrefix = false;
    bool hasUncPrefix = false;

    #if defined(WIN32) || defined(_WIN32)
    if (ustrp::starts_with_and_strip(fileName, getNativeNetworkUncPrefix<StringType>()))
        hasNetworkUncPrefix = true;
    else if (ustrp::starts_with_and_strip(fileName, getNativeUncPrefix<StringType>()))
        hasUncPrefix = true;
    #endif

    bool hasNetPrefix = false;
    if (ustrp::starts_with_and_strip(fileName, getNativeNetworkPathPrefix<StringType>()))
        hasNetPrefix = true;

    bool lastPathSep = stripLastPathSep(fileName);

    std::vector< StringType > parts = ustrp::split(fileName, pathSep, true /* skipEmpty */ );

    std::vector< StringType > resParts;

    StringType nativeCurrentDirAlias = getNativeCurrentDirAlias<StringType>();
    StringType nativeParentDirAlias  = getNativeParentDirAlias<StringType>();

    std::vector< StringType >::iterator pit = parts.begin();
    for(; pit != parts.end(); ++pit)
    {
        if (*pit==nativeCurrentDirAlias)
            continue;

        if (*pit==nativeParentDirAlias)
        {
            if (!resParts.empty())
                resParts.erase( --resParts.end() );
            continue;
        }

        resParts.push_back(*pit);
    }

    pit = resParts.begin();
    StringType nativeHomeDirAlias = getNativeHomeDirAlias<StringType>();
    if (!hasNetPrefix && !hasNetworkUncPrefix && !hasUncPrefix && pit!=resParts.end() && !nativeHomeDirAlias.empty() && *pit==nativeHomeDirAlias)
    {
        StringType homePath = filesys::getCurrentUserHomeDirectory<StringType>();
        if (!homePath.empty())
        {
            std::replace_if( homePath.begin(), homePath.end(), isPathSep<typename StringType::value_type>, pathSep );
            std::vector< StringType > homeParts = ustrp::split(homePath, pathSep, true /* skipEmpty */ );

            resParts.erase(pit);
            resParts.insert( resParts.begin(), homeParts.begin(), homeParts.end() );
        }
    }


    StringType res = ustrp::merge(resParts,pathSep);

    if (lastPathSep)
        appendPathSepInline(res,pathSep);

    if (hasNetPrefix)
        res = getNativeNetworkPathPrefix<StringType>() + res;

    if (hasNetworkUncPrefix)
        res = getNativeNetworkUncPrefix<StringType>() + res;
    else if (hasUncPrefix)
        res = getNativeUncPrefix<StringType>() + res;

    return res;
}

//-----------------------------------------------------------------------------
//! Делает "каноническое" имя для сравнения
template<typename StringType> inline
StringType makeCanonicalForCompare( StringType fileName, typename StringType::value_type pathSep = getNativePathSep<typename StringType::value_type>() )
{
    #if defined(WIN32) || defined(_WIN32)
    namespace ustrp = umba::string_plus;
    StringType canoname = makeCanonical(fileName, pathSep);

    if (ustrp::starts_with_and_strip(canoname, getNativeNetworkUncPrefix<StringType>()))
        canoname = getNativeNetworkPathPrefix<StringType>() + canoname;
    else
        ustrp::starts_with_and_strip(canoname, getNativeUncPrefix<StringType>());

    //return ustrp::toupper_copy(canoname);
    //return ustrp::tolower_copy(canoname);
    return canoname;
    #else
    return makeCanonical(fileName);
    #endif
}

//-----------------------------------------------------------------------------
//! Подготавливает имя для "нативного" использования - для передачи имени в системные API
template<typename StringType> inline
StringType prepareForNativeUsage( const StringType &fileName )
{
    #if defined(WIN32) || defined(_WIN32)
        StringType canoname = makeCanonical(fileName);

        if (sizeof(typename StringType::value_type)==sizeof(char))
            return canoname;

        namespace ustrp = umba::string_plus;

        //if (hasNativeNetworkPathPrefix(canoname))
        //{
        //    return canoname;
        //}
        if (ustrp::starts_with(canoname, getNativeNetworkUncPrefix<StringType>()))
            return canoname; // already UNC name

        StringType nativeUncPrefix = getNativeUncPrefix<StringType>();
        if (ustrp::starts_with(canoname, nativeUncPrefix))
            return canoname; // already UNC name

        if (ustrp::starts_with_and_strip(canoname, getNativeNetworkPathPrefix<StringType>()))
        {
            return getNativeNetworkUncPrefix<StringType>() + canoname;
        }

        return getNativeUncPrefix<StringType>() + canoname;

    #else

        return makeCanonical(fileName);

    #endif
}

//-----------------------------------------------------------------------------
//! Возвращает true,  если путь абсолютный
template<typename StringType> inline 
bool isAbsPath( StringType p, typename StringType::value_type pathSep = getNativePathSep<typename StringType::value_type>() )
{
    namespace ustrp = umba::string_plus;

    p = makeCanonical(p);

    #if defined(WIN32) || defined(_WIN32)
    if (ustrp::starts_with(p, getNativeUncPrefix<StringType>()))
        return true;
    #endif

    if (ustrp::starts_with(p, ustrp::make_string<StringType>(pathSep)))
        return true;

    #if defined(WIN32) || defined(_WIN32)
    if (p.size()<2)
        return false;
    if ( ustrp::is_alpha(p[0]) && (p[1]==(typename StringType::value_type)':') || (p.size()>2 && ustrp::is_alpha(p[1]) && p[2]==(typename StringType::value_type)':'))
        return true;
    #endif

    return false;
}

//-----------------------------------------------------------------------------
//! Делает абсолютный путь под CWD, если задан относительный path 
template<typename StringType> inline 
StringType makeAbsPath( const StringType &path
                      , const StringType &cwd  = umba::filesys::getCurrentDirectory<StringType>()
                      , typename StringType::value_type pathSep = getNativePathSep<typename StringType::value_type>()
                      )
{
    if (isAbsPath(path))
        return path;

    return appendPath(cwd, path);
}

//-----------------------------------------------------------------------------
//! Удаляет префикс пути - делает имя относительным
template<typename StringType> inline 
bool isSubPathName( StringType commonPath
                  , StringType fullName
                  , StringType *pResName = 0
                  , typename StringType::value_type pathSep = getNativePathSep<typename StringType::value_type>()
                  )
{
    commonPath = makeCanonical(commonPath, pathSep);
    fullName   = makeCanonical(fullName  , pathSep);

    appendPathSepInline(commonPath, pathSep);
    if (umba::string_plus::starts_with_and_strip(fullName, commonPath))
    {
        if (pResName)
           *pResName = fullName;

        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
//! Удаляет префикс пути - делает имя относительным
template<typename StringType> inline 
StringType makeRelPath( StringType commonPath
                      , StringType fullName
                      , typename StringType::value_type pathSep = getNativePathSep<typename StringType::value_type>()
                      )
{
    StringType res;
    if (isSubPathName(commonPath, fullName, &res, pathSep))
        return res;

    return fullName;
}

//-----------------------------------------------------------------------------
//! Добавляет путь (или имя файла) к другому пути
template<typename StringType> inline
StringType appendPath( const StringType &p, const StringType &f, typename StringType::value_type pathSep = getNativePathSep<typename StringType::value_type>() )
{
    if (p.empty())
        return f;

    if (f.empty())
        return p;

    if (hasLastPathSep(p))
        return p + stripFirstPathSepCopy(f);

    if (hasFirstPathSep(f))
        return p + f;

    return p + StringType(1, pathSep) + f;
}

//-----------------------------------------------------------------------------
//! Добавляет расширение
template<typename StringType> inline
StringType appendExt( const StringType &n, const StringType &e, typename StringType::value_type extSep = getNativeExtSep<typename StringType::value_type>() )
{

    // Если расширение не задано вообще, то не приклеиваем его, и не приклеиваем даже точку/разделитель расширения
    if (e.empty())
        return n;

    // Если в качестве расширения задана только точка, то она будет приклеена к имени файла

/*
    if (n.empty())
        return e;
    
    if (e.empty())
        return n;

    if (hasLastExtSep(n))
        return n + stripFirstExtSepCopy(e);

    if (hasFirstExtSep(e))
        return n + e;

    return n + StringType(1, extSep) + e;
*/
    return stripLastExtSepCopy(n) + StringType(1, extSep) + stripFirstExtSepCopy(e);
}

//-----------------------------------------------------------------------------
//! Добавляет расширение
template<typename StringType> inline
StringType appendExtention( const StringType &n, const StringType &e, typename StringType::value_type extSep = getNativeExtSep<typename StringType::value_type>() )
{
    return appendExt(n, e, extSep);
}
//inline CharType getNativeExtSep( )

//-----------------------------------------------------------------------------
//! Извлекает путь из имени без последнего разделителя пути
template<typename StringType> inline
StringType getPath( const StringType &s )
{
    //find_if( s.rbegin(), s.rend(), isPathSep<typename StringType::value_type>);

    return stripLastPathSepCopy( StringType( s.begin()
                                           , find_if( s.rbegin(), s.rend(), isPathSep<typename StringType::value_type>).base()
                                           )
                               );
}

inline std::string  getPath( const char    *p ) { return getPath<std::string> ( p ); } //!< Извлекает путь из имени
inline std::wstring getPath( const wchar_t *p ) { return getPath<std::wstring>( p ); } //!< Извлекает путь из имени


//-----------------------------------------------------------------------------
//! Извлекает из полного пути имя файла + расширение
template<typename StringType> inline
StringType getFileName( const StringType &path )
{
    auto lastPathSepIt = find_if( path.rbegin(), path.rend(), isPathSep<typename StringType::value_type>).base();

    if (lastPathSepIt==path.end()) // Разделитель пути не найден вообще
        return path;

    return StringType( lastPathSepIt, path.end() );
}

inline std::string  getFileName( const char    *p ) { return getFileName<std::string> ( p ); } //!< Извлекает из полного пути имя файла + расширение
inline std::wstring getFileName( const wchar_t *p ) { return getFileName<std::wstring>( p ); } //!< Извлекает из полного пути имя файла + расширение

//-----------------------------------------------------------------------------
//! Извлекает из имени расширение
template<typename StringType> inline
StringType getFileExtention( const StringType &path )
{
    auto lastPathSepRevIt = find_if( path.rbegin(), path.rend(), isPathSep<typename StringType::value_type>); // .base();

    // if (lastPathSepRevIt!=path.rbegin()) // Разделитель пути найден - смещаемся за него
    //    lastPathSepRevIt--;

    //typename StringType::const_reverse_iterator rLastPathSepIt = lastPathSepIt;

    auto lastExtSepIt = find_if( path.rbegin(), lastPathSepRevIt, isExtSep<typename StringType::value_type>).base();

    // if (lastExtSepIt!=path.end()) // Разделитель расширения найден - смещаемся за него
    //     lastExtSepIt++;

    // if (lastPathSepIt==path.end())
    //    lastPathSepIt = path.begin();
    
    // if (lastExtSepIt==path.end())
    //     lastExtSepIt = path.end();

    if (lastExtSepIt==lastPathSepRevIt.base())
        lastExtSepIt = path.end();

    return StringType( lastExtSepIt, path.end() );
}

//! Извлекает из имени расширение
template<typename StringType> inline
StringType getFileExt( const StringType &path )
{
    return getFileExtention<StringType>(path);
}

inline std::string  getFileExtention( const char    *p ) { return getFileExtention<std::string> ( p ); } //!< Извлекает из имени расширение
inline std::wstring getFileExtention( const wchar_t *p ) { return getFileExtention<std::wstring>( p ); } //!< Извлекает из имени расширение

inline std::string  getFileExt      ( const char    *p ) { return getFileExtention<std::string> ( p ); } //!< Извлекает из имени расширение
inline std::wstring getFileExt      ( const wchar_t *p ) { return getFileExtention<std::wstring>( p ); } //!< Извлекает из имени расширение

//! Извлекает из имени расширение - для совместимости
template<typename StringType> inline
StringType getExt( const StringType &path )
{
    return getFileExtention<StringType>(path);
}

inline std::string  getExt( const char    *p ) { return getFileExtention<std::string> ( p ); } //!< Извлекает из имени расширение
inline std::wstring getExt( const wchar_t *p ) { return getFileExtention<std::wstring>( p ); } //!< Извлекает из имени расширение


//-----------------------------------------------------------------------------
//! Извлекает из имени имя файла без пути и расширения
template<typename StringType> inline
StringType getName( const StringType &path )
{
    auto lastPathSepRevIt = find_if( path.rbegin(), path.rend(), isPathSep<typename StringType::value_type>); // .base();

    // if (lastPathSepRevIt!=path.rbegin()) // Разделитель пути найден - смещаемся за него
    //    lastPathSepRevIt--;

    auto lastExtSepIt = find_if( path.rbegin(), lastPathSepRevIt, isExtSep<typename StringType::value_type>).base();

    // if (lastExtSepIt!=path.end()) // Разделитель расширения найден - смещаемся перед ним
    //     lastExtSepIt--;

    auto lastPathSepIt = lastPathSepRevIt.base();
    if (lastPathSepIt==path.end())
       lastPathSepIt = path.begin();

    // if (lastExtSepIt==path.end())
    //     lastExtSepIt = path.end();

    if (lastExtSepIt==lastPathSepIt)
        lastExtSepIt = path.end();

    return stripLastExtSepCopy(StringType( lastPathSepIt, lastExtSepIt ));
}

inline std::string  getName( const char    *p ) { return getName<std::string> ( p ); } //!< Извлекает из имени расширение
inline std::wstring getName( const wchar_t *p ) { return getName<std::wstring>( p ); } //!< Извлекает из имени расширение


//-----------------------------------------------------------------------------
//! Извлекает имя диска из пути
template<typename StringType> inline
StringType getDrive( StringType n )
{
    #if defined(WIN32) || defined(_WIN32)
    namespace ustrp = umba::string_plus;
    n = stripNativeUncPrefix( n );
    n = makeCanonical( n );
    std::vector< StringType > parts = ustrp::split(n, getNativePathSep<typename StringType::value_type>(), true /* skipEmpty */ );
    if (parts.empty())
        return StringType();

    const StringType &p1 = parts[0];
    if (p1.empty())
        return StringType();

    if (p1[p1.size()-1]==(typename StringType::value_type)':')
        return p1;

    #endif

    return StringType();

}

//-----------------------------------------------------------------------------
//! Проверяет путь на присутствие имени диска, и если отсутствует, то обновляет его, извлекая имя диска из другого пути 
template<typename StringType> inline
StringType checkPathPrependDrive( const StringType &n, const StringType &curDir = filesys::getCurrentDirectory<StringType>())
{
    #if defined(WIN32) || defined(_WIN32)
    namespace ustrp = umba::string_plus;

    StringType tmpCaninical = makeCanonical<StringType>(n);
    if ( ustrp::starts_with(tmpCaninical, getNativeUncPrefix<StringType>()) 
      || ustrp::starts_with_and_strip(tmpCaninical, getNativeNetworkPathPrefix<StringType>())
       )
        return n;

    if (hasFirstPathSep(n))
        return getDrive<StringType>(curDir) + n;
    #endif

    return n;
}



#if 0
inline
std::string getPathName( const std::string &s )
{
    if (s.empty())
        return s;

    std::string::size_type pos = s.size();
    for( ; pos; --pos)
    {
        char ch = s[pos-1];
        if (ch=='.')
        {
            return std::string( s, 0, pos-1 ); // cut ext
        }
        if (isPathSep(ch)) // (ch=='\\' || ch=='/') // found path sep before ext sep
        {
            return s;
        }
    }

    return s;
}

//-----------------------------------------------------------------------------
inline
std::string getNameFromFull( const std::string &s )
{
    if (s.empty())
        return s;

    std::string::size_type pos = s.size();
    for( ; pos; --pos)
    {
        char ch = s[pos-1];
        if (isPathSep(ch)) // (ch=='\\' || ch=='/')
        {
            return std::string( s, pos );
        }
    }

    return s;
}

//-----------------------------------------------------------------------------
inline
std::string getFileName( const std::string &name )
{
    std::string f, s;
    splitToPair( getNameFromFull(name), f, s, '.' );
    return f;
}

//-----------------------------------------------------------------------------
inline
std::string getFileExtention( const std::string &name )
{
    std::string f, s;
    splitToPair( getNameFromFull(name), f, s, '.' );
    return s;
}

inline
std::string appendExtention( std::string name, std::string ext )
{
    if (!ext.empty() && ext.front()=='.')
        ext.erase(0,1);
    if (name.empty() || name.back()!='.')
        name.append(".");
    return name+ext;
}

//-----------------------------------------------------------------------------
inline
std::string generateOutputFilename( const std::string &inputFilename, const std::string &outputFilename, const std::string &defName, const std::string &defExt)
{
    std::string outputFinalName = outputFilename;
    std::string ext  = getFileExtention( outputFinalName );
    std::string name = getFileName( outputFinalName );
    std::string path = getPath( outputFinalName );

    std::string inputPath = getPath(inputFilename);

    
    if (outputFinalName.empty())
        path = getPath(inputFilename);
    
    if (name.empty())
    {
        outputFinalName = defName;
        if (!path.empty())
            outputFinalName = path + std::string(1, getPathSep()) /* std::string("\\") */  + outputFinalName;
    }
    
    //if (ext.empty())
    //    ext = defExt;
    
    if (ext.empty())
    {
        if (!defExt.empty())
            outputFinalName.append(".");
        outputFinalName.append(defExt);
    }

    return outputFinalName;

}

//-----------------------------------------------------------------------------
inline
bool setFileReadOnlyAttr( const std::string &file, bool bSet = true )
{
    #if defined(WIN32) || defined(_WIN32)
    DWORD attrs = GetFileAttributesA( file.c_str() );
    if (attrs==INVALID_FILE_ATTRIBUTES)
       {
        return false; // GetLastError();
       }

    if (bSet)
       attrs |= FILE_ATTRIBUTE_READONLY;
    else
       attrs &= ~FILE_ATTRIBUTE_READONLY;

    BOOL bRes = SetFileAttributesA( file.c_str(), attrs );
    if (!bRes)
       {
        return false; //GetLastError();
       }
    return true;
    #else
    return false; //UNDONE: not implemented
    #endif
}
#endif

//-----------------------------------------------------------------------------
/*
inline
RCODE setFileReadOnlyAttr( const std::wstring &file, bool bSet = true )
{
    DWORD attrs = GetFileAttributesW( file.c_str() );
    if (attrs==INVALID_FILE_ATTRIBUTES)
       {
        return WIN2RC(GetLastError());
       }

    if (bSet)
       attrs |= FILE_ATTRIBUTE_READONLY;
    else
       attrs &= ~FILE_ATTRIBUTE_READONLY;

    BOOL bRes = SetFileAttributesW( file.c_str(), attrs );
    if (!bRes)
       {
        return WIN2RC(GetLastError());
       }
    return EC_OK;
}
*/





} // namespace filename
} // namespace umba
