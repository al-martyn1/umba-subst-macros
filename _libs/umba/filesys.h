#pragma once

//-----------------------------------------------------------------------------

/*! 
    \file
    \brief Прослойка абстракций для работы с файловой системой
 */

//-----------------------------------------------------------------------------

#include "umba/stl.h"
#include "umba/alloca.h"
#include "umba/env.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <exception>
#include <stdexcept>
#include <iostream>
#include <ctime>

#if defined(WIN32) || defined(_WIN32)
    #include <Shlobj.h>
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <pwd.h>
#endif



// umba::filesys::
namespace umba{
//! Прослойка абстракций для работы с файловой системой
namespace filesys{}
}
//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
namespace umba
{
namespace filesys
{

/*! \ingroup UMBA_CORE
 *  \addtogroup UMBA_FILESYSTEM
 *  @{
 */

//----------------------------------------------------------------------------
//! Тип файла
enum FileType
{
    FileTypeInvalid, //!< Файл не найден, или ещё какая-то проблема
    FileTypeUnknown, //!< Тип файла не известен
    FileTypeDir    , //!< Каталог, не файл
    FileTypeFile   , //!< Файл
    FileTypeRam      //!< Что-то просто лежит в памяти
};

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
#if defined(WIN32) || defined(_WIN32)
    typedef   __time64_t  filetime_t; //!< Файловое время - время создания, изменения, доступа, и тп
    typedef   uint64_t    filesize_t; //!< Тип для хранения размера файла
#else
    typedef   time_t      filetime_t; //!< Файловое время - время создания, изменения, доступа, и тп. Для *nix/MCU используется дефолтный тип, который ограничен 2038 годом. Я уже буду одной ногой на пенсии
    typedef   uint64_t    filesize_t; //!< Тип для хранения размера файла
#endif

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
//! Шаблонная функция форматирования файлового даты/времени (C++ аналог strftime). Описание форматной строки тут - https://man7.org/linux/man-pages/man3/strftime.3.html
template<typename StringType> inline
StringType formatFiletime( filetime_t t, const StringType &fmt );

//------------------------------

#if defined(WIN32) || defined(_WIN32)

//------------------------------
//! Версия std::string strftime для C++. Описание форматной строки тут - https://man7.org/linux/man-pages/man3/strftime.3.html
template<> inline
std::string formatFiletime<std::string>( filetime_t t, const std::string &fmt )
{
    if (fmt.empty())
        return fmt;

    std::tm tmBuf;
    if (_localtime64_s(&tmBuf, &t)!=0)
        return std::string("INVALID_TIME");

    char buf[64];
    size_t zpos = strftime( &buf[0], 63, fmt.c_str(), &tmBuf );
    if (!zpos)
        return std::string("TOO_LONG_TIME_FORMAT");

    buf[zpos] = 0;

    return std::string(buf);
}

//------------------------------
//! Версия std::wstring strftime для C++. Описание форматной строки тут - https://man7.org/linux/man-pages/man3/strftime.3.html
template<> inline
std::wstring formatFiletime<std::wstring>( filetime_t t, const std::wstring &fmt )
{
    if (fmt.empty())
        return fmt;

    std::tm tmBuf;
    if (_localtime64_s(&tmBuf, &t)!=0)
        return std::wstring(L"INVALID_TIME");

    wchar_t buf[64];
    size_t zpos = wcsftime( &buf[0], 63, fmt.c_str(), &tmBuf );
    if (!zpos)
        return std::wstring(L"TOO_LONG_TIME_FORMAT");

    buf[zpos] = 0;

    return std::wstring(buf);
}

//------------------------------

#else

//------------------------------
//! Версия std::string strftime для C++. Описание форматной строки тут - https://man7.org/linux/man-pages/man3/strftime.3.html
template<> inline
std::string formatFiletime<std::string>( filetime_t t, const std::string &fmt )
{
    if (fmt.empty())
        return fmt;

    std::tm *ptmBuf = std::localtime(&t);
    char buf[64];
    size_t zpos = strftime( &buf[0], 63, fmt.c_str(), &tmBuf );
    if (!zpos)
        return std::string("TOO_LONG_TIME_FORMAT");

    buf[zpos] = 0;

    return std::string(buf);
}

//------------------------------
//! Версия std::wstring strftime для C++. Описание форматной строки тут - https://man7.org/linux/man-pages/man3/strftime.3.html
template<> inline
std::wstring formatFiletime<std::wstring>( filetime_t t, const std::wstring &fmt )
{
    return umba::string_plus::make_string<std::wstring>( formatFiletime<std::string>( t, umba::string_plus::make_string<std::string>(fmt) ) );
}

//------------------------------

#endif

//------------------------------


//------------------------------
//! Версия strftime для C++, принимающая указатель на сырую форматную строку. Описание форматной строки тут - https://man7.org/linux/man-pages/man3/strftime.3.html
template<typename StringType> inline
StringType formatFiletime( filetime_t t, const typename StringType::value_type *fmt )
{
    return formatFiletime<StringType>( t, StringType(fmt) );
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
//! Данные статистики по файлу
struct FileStat
{
    FileType     fileType;         //!< Тип файла - файл, каталог, хз, ошибка
    filesize_t   fileSize;         //!< Размер файла
    filetime_t   timeCreation    ; //!< Время создания
    filetime_t   timeLastModified; //!< Время последней модификации
    filetime_t   timeLastAccess  ; //!< Время последнего доступа

    //! Возвращает true, если тип файла валидный
    bool isValid() const { return fileType!=FileTypeInvalid; }

    //! Возвращает true, если статистика - по каталогу
    bool isDir()   const { return fileType==FileTypeDir    ; }

    //! Возвращает true, если статистика - по файлу
    bool isFile()  const { return fileType==FileTypeFile   ; }
};

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
#if defined(WIN32) || defined(_WIN32)
    
    //! Одно универсальное имя для struct stat
    #define struct_file_stat               struct __stat64

    //! Хелпер получения статистики файла в ОС-зависимом формате по имени в std::string
    inline int struct_file_stat_get_stat( const std::string &fileName, struct_file_stat *statBuf )
    {
        return _stat64( fileName.c_str(), statBuf );
    }

    //! Хелпер получения статистики файла в ОС-зависимом формате по имени в std::wstring
    inline int struct_file_stat_get_stat( const std::wstring &fileName, struct_file_stat *statBuf )
    {
        return _wstat64( fileName.c_str(), statBuf );
    }

#else

    //! Одно универсальное имя для struct stat
    #define struct_file_stat    struct stat

    //! Хелпер получения статистики файла в ОС-зависимом формате по имени в std::string
    inline int struct_file_stat_get_stat( const std::string &fileName, struct_file_stat *statBuf )
    {
        return stat( fileName.c_str(), statBuf );
    }

    //! Хелпер получения статистики файла в ОС-зависимом формате по имени в std::wstring
    inline int struct_file_stat_get_stat( const std::wstring &fileName, struct_file_stat *statBuf )
    {
        throw std::runtime_error("Not implemented: wide version of the struct_file_stat_get_stat (getFileStat) not implemented for non-WIN32");
    }

#endif

//----------------------------------------------------------------------------





//----------------------------------------------------------------------------
//! Хелпер конвертации системной статистики в нашу
inline 
void parseStatToFileStat( const struct_file_stat &statBuf, FileStat &fileStat )
{
    fileStat.fileSize         = statBuf.st_size;
    fileStat.timeCreation     = statBuf.st_ctime;
    fileStat.timeLastModified = statBuf.st_mtime;
    fileStat.timeLastAccess   = statBuf.st_atime;

    fileStat.fileType = FileTypeUnknown;

    #if defined(WIN32) || defined(_WIN32)
    if (statBuf.st_mode&_S_IFDIR)
        fileStat.fileType = FileTypeDir;
    else if (statBuf.st_mode&_S_IFREG)
        fileStat.fileType = FileTypeFile;
    #else
    if (statBuf.st_mode&S_IFDIR)
        fileStat.fileType = FileTypeDir;
    else if (statBuf.st_mode&S_IFREG)
        fileStat.fileType = FileTypeFile;
    #endif
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
#if defined(WIN32) || defined(_WIN32)

//! Хелпер (std::string) для генерик открытия файла для чтения
inline
HANDLE openFileForReadingWin32(const std::string &filename)
{
    if (filename.empty()) return INVALID_HANDLE_VALUE;

    return CreateFileA( filename.c_str(), GENERIC_READ
                      , FILE_SHARE_READ | FILE_SHARE_WRITE
                      , 0 // lpSecurityAttributes
                      , OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL
                      , 0 // hTemplateFile
                      );
}

//! Хелпер (std::wstring) для генерик открытия файла для чтения
inline
HANDLE openFileForReadingWin32(const std::wstring &filename)
{
    if (filename.empty()) return INVALID_HANDLE_VALUE;

    return CreateFileW( filename.c_str(), GENERIC_READ
                      , FILE_SHARE_READ | FILE_SHARE_WRITE
                      , 0 // lpSecurityAttributes
                      , OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL
                      , 0 // hTemplateFile
                      );
}

// https://docs.microsoft.com/en-us/windows/win32/sysinfo/converting-a-time-t-value-to-a-file-time

//! Хелпер конвертации юниксового filetime_t в виндовый
inline FILETIME convertUnixTimeT( filetime_t t )
{
    filetime_t t1 = t, t2 = 10000000ull, t3 = 116444736000000000ull;
    LONGLONG ll = t1*t2 + t3;
    FILETIME ft;
    ft.dwLowDateTime = (DWORD) ll;
    ft.dwHighDateTime = (DWORD)(ll >>32);
    return ft;
}

//! Хелпер конвертации виндового FILETIME в юниксовый
inline filetime_t convertWindowsFiletime( FILETIME ft )
{
    filetime_t ftH = ft.dwHighDateTime;
    filetime_t ftL = ft.dwLowDateTime;
    //filetime_t ft

    filetime_t wndFt = (ftH<<32)+ftL;

    return (wndFt - 116444736000000000ull) / 10000000ull;
}

//! Хелпер получения статистики файла
template<typename StringType> inline
FileStat getFileStat( const StringType &fileName )
{

    // https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfiletime

    // https://docs.microsoft.com/en-us/windows/win32/sysinfo/converting-a-time-t-value-to-a-file-time

    FileStat fileStat;
    fileStat.fileType = FileTypeInvalid;


    HANDLE hFile = openFileForReadingWin32( fileName.c_str() );
    if (hFile==INVALID_HANDLE_VALUE) 
    {
        DWORD err = GetLastError();
        return fileStat;
    }


    FILETIME creationTime;
    FILETIME lastAccessTime;
    FILETIME lastWriteTime;

    if (!GetFileTime(hFile, &creationTime, &lastAccessTime, &lastWriteTime))
    {
        CloseHandle(hFile);
        return fileStat;
    }

    LARGE_INTEGER liFileSize;
    if (!GetFileSizeEx(hFile,&liFileSize))
    {
        CloseHandle(hFile);
        return fileStat;
    }

    fileStat.fileSize = (filesize_t)liFileSize.QuadPart;


    fileStat.fileType = FileTypeFile; // Need to check is dir
    // enum FileType { FileTypeInvalid, FileTypeUnknown, FileTypeDir, FileTypeFile, FileTypeRam };

    fileStat.timeCreation     = convertWindowsFiletime(creationTime);
    fileStat.timeLastModified = convertWindowsFiletime(lastAccessTime);
    fileStat.timeLastAccess   = convertWindowsFiletime(lastWriteTime);

    CloseHandle(hFile);
    return fileStat;
}

#else // !WIN32

//! Хелпер получения статистики файла
template<typename StringType> inline
FileStat getFileStat( const StringType &fileName )
{
    FileStat fileStat;
    fileStat.fileType = FileTypeInvalid;

    if (fileName.empty())
        return fileStat;

    struct_file_stat buffer;
    int res = struct_file_stat_get_stat( fileName, &buffer );

    if (res<0)
        return fileStat;

    parseStatToFileStat( buffer, fileStat );
    return fileStat;
}


#endif

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
//! Получение filetime_t на текущий момент
inline filetime_t getFileTimeNow()
{
    #if defined(WIN32) || defined(_WIN32)
    __time64_t t;
    _time64(&t);
    #else
    time_t t;
    time(&t);
    #endif

    return (filetime_t)t;
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
/*
    Нужно а) уметь проверять файл по имени на доступность для чтения
          б) сразу его читать без переоткрытия

          Или фик с ним, и не париться с преждевременной оптимизацией?
          По идее, два последовательных открытия не должны быть сильно узким местом

    https://docs.microsoft.com/en-us/windows/win32/fileio/creating-and-opening-files
    https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea

*/
//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
//! Получение текущего рабочего каталога - генерик версия
template<typename StringType> inline
StringType getCurrentDirectory()
{
    throw std::runtime_error("Not implemented: getCurrentDirectory not specialized for this StringType");
}

//----------------------------------------------------------------------------
#if defined(WIN32) || defined(_WIN32)

//----------------------------------------------------------------------------
//! Проверка доступности файла на чтение
template<typename StringType> inline
bool isFileReadable( const StringType &filename )
{
    HANDLE hFile = openFileForReadingWin32( filename.c_str() );
    if (hFile==INVALID_HANDLE_VALUE) return false;
    CloseHandle(hFile);
    return true;
}

//------------------------------
// "vector" class used for storing data cause it's allow access to raw data for writting in C++ 0x03
// In C++ 0x11 string class also has the same functionality
//----------------------------------------------------------------------------
//! Чтение файла в вектор
/*! 
    \tparam StringType Тип имени файла - std::string/std::wstring
    \tparam DataType   Тип читаемых данных

    \return Возвращает true, если файл был прочитан
 */
template<typename StringType, typename DataType> inline
bool readFile( const StringType &filename       //!< Имя файла
             , std::vector<DataType> &filedata  //!< Вектор для данных
             , FileStat *pFileStat = 0          //!< [out] Статистика файла
             , bool ignoreSizeErrors = true     //!< Игнорировать разночтения из статистики файла и реально прочитанного размера
             )
{
    filedata.clear();

    FileStat fileStat = getFileStat( filename );
    if (fileStat.fileType!=FileTypeFile)
        return false;

    if (pFileStat)
        *pFileStat = fileStat;

    if (fileStat.fileSize==0)
        return true; // no data for reading at all

    if (!isFileReadable( filename ))
        return false;

    const size_t itemSize = sizeof(DataType);
    const size_t numItems = (size_t)(fileStat.fileSize / itemSize);
    filedata.resize( numItems ); // We can read files which are always can fit to memory
    const size_t numRawBytesToRead = filedata.size()*itemSize;

    // Here starts "no exceptions" (exception safe) zone

    HANDLE hFile = openFileForReadingWin32( filename.c_str() );
    if (hFile==INVALID_HANDLE_VALUE)
    {
        DWORD err = GetLastError();
        return false;
    }

    DWORD readedBytes = 0;

    BOOL bReadRes = ReadFile( hFile, &filedata[0], (DWORD)numRawBytesToRead, &readedBytes, 0 );

    CloseHandle(hFile);

    if (!bReadRes)
    {
        filedata.clear(); // Ошибка. Если вектор не временный, то неплохо бы его обнулить, чтобы место в памяти не хавал.
        return false;
    }

    if (readedBytes!=(DWORD)fileStat.fileSize)
    {
        if (ignoreSizeErrors)
        {
            fileStat.fileSize = (filesize_t)filedata.size(); // сохраняем консистенцию размера вектора и fileStat'а. Или не надо?
            return true;
        }

        // Чистим вектор, и возвращаем ошибку
        filedata.clear();
        return false;
    }

    // Вроде всё хорошоу
    return true;
}

//------------------------------
//! Получение текущего рабочего каталога - специализация для std::string
template<> inline
std::string getCurrentDirectory<std::string>()
{
    char  ch = 0;
    char *buf = &ch;
    DWORD size = ::GetCurrentDirectoryA(1, buf);
    if (!size) return std::string();
    buf = (char*)_alloca(size*sizeof(char)+1);
    ::GetCurrentDirectoryA(size, buf);
    return std::string(buf);
}

//------------------------------
//! Получение текущего рабочего каталога - специализация для std::wstring
template<> inline
std::wstring getCurrentDirectory<std::wstring>()
{
    wchar_t  ch = 0;
    wchar_t *buf = &ch;
    DWORD size = ::GetCurrentDirectoryW(1, buf);
    if (!size) return std::wstring();
    buf = (wchar_t*)_alloca(size*sizeof(wchar_t)+1);
    ::GetCurrentDirectoryW(size, buf);
    return std::wstring(buf);
}

//----------------------------------------------------------------------------

#else // !WIN32

//----------------------------------------------------------------------------

//! Проверка доступности файла на чтение
template<typename StringType> inline
bool isFileReadable( const StringType &filename )
{
    throw std::runtime_error("Not implemented: isFileReadable not specialized for this StringType");
}

//----------------------------------------------------------------------------
//! Чтение файла в вектор
/*! 
    \tparam StringType Тип имени файла - std::string / std::wstring
    \tparam DataType   Тип читаемых данных

    \return Возвращает true, если файл был прочитан
 */
template<typename StringType, typename DataType> inline
bool readFile( const StringType &filename, std::vector<DataType> &filedata
             , FileStat *pFileStat = 0          //!< [out] Статистика файла
             , bool ignoreSizeErrors = true     //!< Игнорировать разночтения из статистики файла и реально прочитанного размера
             )
{
    filedata.clear();

    FileStat fileStat = getFileStat( filename );
    if (fileStat.fileType!=FileTypeFile)
        return false;

    if (pFileStat)
        *pFileStat = fileStat;

    if (fileStat.fileSize==0)
        return true; // no data for reading at all

    if (!isFileReadable( filename ))
        return false;

    const size_t itemSize = sizeof(DataType);
    const size_t numItems = (size_t)(fileStat.fileSize / itemSize);
    filedata.resize( numItems ); // We can read files which are always can fit to memory
    const size_t numRawBytesToRead = filedata.size()*itemSize;


    // Here starts "no exceptions" (exception safe) zone
    std::ifstream ifs;

    ifs.clear();
    ifs.open( filename.c_str() );
    if (!ifs)
    {
        filedata.clear(); // Ошибка. Если вектор не временный, то неплохо бы его обнулить, чтобы место в памяти не хавал.
        return false;
    }

    ifs.read( (char*)&filedata[0], (size_t)numRawBytesToRead );

    if (ifs.fail())
    {
        filedata.clear(); // Ошибка. Если вектор не временный, то неплохо бы его обнулить, чтобы место в памяти не хавал.
        return false;
    }

    size_t readedBytes = (size_t)ifs.gcount();

    if (readedBytes!=(size_t)fileStat.fileSize)
    {
        if (ignoreSizeErrors)
        {
            fileStat.fileSize = (filesize_t)filedata.size(); // сохраняем консистенцию размера вектора и fileStat'а. Или не надо?
            return true;
        }

        // Чистим вектор, и возвращаем ошибку
        filedata.clear();
        return false;
    }

    // Вроде всё хорошоу
    return true;

}

//----------------------------------------------------------------------------
//! Проверка доступности файла на чтение - специализация для std::string
template<> inline
bool isFileReadable<std::string>( const std::string &filename )
{
    if (filename.empty()) return false;

    std::ifstream ifs;

    ifs.clear();
    ifs.open( filename.c_str() );
    if (!!ifs)
    {
        foundName = fileName;
        ifs.close();
        return true;
    }

    return false;
}

//----------------------------------------------------------------------------
//! Чтение файла в вектор - специализация
template<> inline
bool readFile<std::wstring, uint8_t>( const std::wstring &filename, std::vector<uint8_t> &filedata, FileStat *pFileStat = 0, bool ignoreSizeErrors = true /* read something */ )
{
    throw std::runtime_error("Not implemented: readFile not specialized for this StringType");
}

//----------------------------------------------------------------------------
//! Чтение файла в вектор - специализация
template<> inline
bool readFile<std::wstring, char>( const std::wstring &filename, std::vector<char> &filedata, FileStat *pFileStat = 0, bool ignoreSizeErrors = true /* read something */ )
{
    throw std::runtime_error("Not implemented: readFile not specialized for this StringType");
}

//------------------------------
//! Получение текущего рабочего каталога - специализация для std::wstring
template<> inline
std::string getCurrentDirectory<std::string>()
   {
    size_t size = 1024;
    char *buf = (char*)_alloca(size*sizeof(buf[0]));
    char* pRes = getcwd( buf, int(size));
    while(!pRes && errno==ERANGE)
       {
        size *= 2;
        buf = (char*)_alloca(size*sizeof(buf[0]));
        pRes = getcwd( buf, int(size));
       }
    if (!pRes) return ::std::string();
    return ::std::string(pRes);
   }

//----------------------------------------------------------------------------

#endif

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
//! Получение имени текущего диска
template<typename StringType> inline
StringType getCurrentDrive()
{
    #if defined(WIN32) || defined(_WIN32)

    namespace ustrp = umba::string_plus;

    StringType curDir = filename::makeCanonical<StringType>(getCurrentDirectory<StringType>());
    std::vector< StringType > parts = ustrp::split(curDir, filename::getNativePathSep<typename StringType::value_type>(), true /* skipEmpty */ );
    if (!parts.empty())
        return ustrp::toupper_copy(parts[0]);
    return StringType();

    #else

    return StringType();

    #endif
}

//----------------------------------------------------------------------------
#if defined(WIN32) || defined(_WIN32)

    //------------------------------

    //! Получение домашнего каталога текущего пользователя в винде
    template<typename StringType>
    StringType getUserFolderPathFromWinApi(); // { return throw std::runtime_error("getUserFolderPathFromWinApi: not specified"); StringType(); }

    //! Получение домашнего каталога текущего пользователя в винде. Специализация
    template<>
    std::string getUserFolderPathFromWinApi<std::string>()
    {
        char buf[4096];
    
        if (SHGetSpecialFolderPathA( 0, buf, CSIDL_PROFILE, false))
        //if (SHGetFolderPathA( 0, buf, CSIDL_PROFILE, false))
           return buf;
    
        return std::string();
    }
    
    //! Получение домашнего каталога текущего пользователя в винде. Специализация
    template<>
    std::wstring getUserFolderPathFromWinApi<std::wstring>()
    {
        wchar_t buf[4096];
    
        if (SHGetSpecialFolderPathW( 0, buf, CSIDL_PROFILE, false))
        //if (SHGetFolderPathA( 0, buf, CSIDL_PROFILE, false))
           return buf;
    
        return std::wstring();
    }


    //------------------------------

    //! Получение временного каталога (текущего пользователя или как придётся) в винде
    template<typename StringType>
    StringType getTempFolderPathFromWinApi(); // { return throw std::runtime_error("getUserFolderPathFromWinApi: not specified"); StringType(); }

    //! Получение временного каталога (текущего пользователя или как придётся) в винде. Специализация
    template<>
    std::string getTempFolderPathFromWinApi<std::string>()
    {
        char buf[4096];
    
        const std::size_t bufSize = sizeof(buf) / sizeof(buf[0]);

        DWORD res = GetTempPathA( bufSize, &buf[0]);
        // Если 0 - это ошибка, если > размера буфера - не влезло - сорян, если не хватило 4Кб под имя фолдера, что-то пошло явно не так
        if (!res || res>bufSize)
            return std::string("C:\\Temp");

        return std::string( &buf[0], res );
    }

    //! Получение временного каталога (текущего пользователя или как придётся) в винде. Специализация
    template<>
    std::wstring getTempFolderPathFromWinApi<std::wstring>()
    {
        wchar_t buf[4096];
    
        const std::size_t bufSize = sizeof(buf) / sizeof(buf[0]);

        DWORD res = GetTempPathW( bufSize, &buf[0]);
        // Если 0 - это ошибка, если > размера буфера - не влезло - сорян, если не хватило 4Кб под имя фолдера, что-то пошло явно не так
        if (!res || res>bufSize)
            return std::wstring(L"C:\\Temp");

        return std::wstring( &buf[0], res );
    }

#endif // defined(WIN32) || defined(_WIN32)

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
//! Получение домашнего каталога текущего пользователя
template<typename StringType> inline
StringType getCurrentUserHomeDirectory()
{
    #if defined(WIN32) || defined(_WIN32)

        StringType res;
        if (umba::env::getVar(umba::string_plus::make_string<StringType>("USERPROFILE"), res))
        {
            return res;
        }
       
        return getUserFolderPathFromWinApi<StringType>();

    #else

        //TODO: Переделать !!!
        // https://stackoverflow.com/questions/2910377/get-home-directory-in-linux
       
        StringType res;
        if (umba::env::getVar(make_string<StringType>("HOME"), res))
        {
            return res;
        }

        return umba::string_plus::make_string<StringType>(getpwuid(getuid())->pw_dir);

    #endif

    return StringType();
}

//----------------------------------------------------------------------------
//! Получение TEMP каталога
template<typename StringType> inline
StringType getTempFolderPath()
{
    #if defined(WIN32) || defined(_WIN32)

        return getTempFolderPathFromWinApi<StringType>();

    #else

        // Пытаемся найти юзерский TEMP
        StringType res;
        if (umba::env::getVar(make_string<StringType>("TMPDIR"), res))
        {
            return res;
        }

        return make_string<StringType>("/tmp");

    #endif
}

//----------------------------------------------------------------------------





//----------------------------------------------------------------------------

// End of UMBA_FILESYSTEM
/*! @}*/

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------

} // namespace filesys
} // namespace umba

// umba::filesys::




