#pragma once

//-----------------------------------------------------------------------------

/*! 
    \file
    \brief Хелперы для работы с консолью под Windows
 */

//-----------------------------------------------------------------------------
//#include <ntstatus.h>

#include <winternl.h>

#ifdef UNICODE
    #undef UNICODE
    #include <tlhelp32.h>
    #define UNICODE
#else
    #include <tlhelp32.h>
#endif


#if defined(WIN32) || defined(_WIN32)

    #include <sys/types.h>
    #include <sys/stat.h>
    #include <stdio.h>
    #include <io.h>

#else

    #include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>   // for isatty()
    #include <stdio.h>    // for fileno()

    //! Compatibility macro - возвращает true (1), если хэндл относится к терминалу
    #define _isatty isatty
    //! Compatibility macro
    #define _fileno fileno
    //! Compatibility macro
    #define _tell   tell
    //! Compatibility macro
    #define S_IFCHR _S_IFCHR

#endif

#if !defined(S_ISCHR)

    //! Compatibility macro
    #define S_ISCHR(m) ((m)&(_S_IFCHR))

#endif


#include <string>
#include <vector>
#include <set>

#ifdef DEBUG_WINCONHELPERS
    
    #include <io.h>
    #include <fcntl.h>
    #include <sys/types.h>
    #include <sys/stat.h>

    #ifndef _MSC_VER
        #define _write write
    #endif

    //! Макрос записи в stdout - какой-то хелпер - XZ
    #define  DEBUG_WINCONHELPERS_WRITE_STR(str) do { write( 1 /* stdout */ , (const void*)str, strlen((const char*)str) ); } while(0)
    //! Макрос printf'а в stdout - какой-то хелпер - XZ
    #define  DEBUG_WINCONHELPERS_PRINTF( ... )  do { printf(__VA_ARGS__); fflush( stdout ); } while(0)

#else

    //! Макрос записи в stdout - какой-то хелпер - XZ - затычка
    #define  DEBUG_WINCONHELPERS_WRITE_STR(str) str
    //! Макрос printf'а в stdout - какой-то хелпер - XZ - затычка
    #define  DEBUG_WINCONHELPERS_PRINTF( ... )

#endif

//! Хелперы для виндовой консоли
namespace winhelpers
{

//! Утилиты для хелперов для виндовой консоли
namespace utils
{

//-----------------------------------------------------------------------------
//! Просто потому, что standalone модуль и не тащить
inline
char toUpper( char ch )
{
    if (ch>='a' && ch<='z')
        return ch - 'a' + 'A';
    return ch;
}

//! Просто потому, что standalone модуль и не тащить
inline
char toLower( char ch )
{
    if (ch>='A' && ch<='Z')
        return ch - 'A' + 'a';
    return ch;
}

//-----------------------------------------------------------------------------
//! Просто потому, что standalone модуль и не тащить
inline
std::string toUpper( std::string s )
{
    //for (char& ch : s) 
    for(std::string::iterator it = s.begin(); it != s.end(); ++it)
    {
        //ch = toUpper(ch);
        *it = toUpper(*it);
    }

    return s;
}

//! Просто потому, что standalone модуль и не тащить
inline
std::string toLower( std::string s )
{
    //for (char& ch : s) 
    for(std::string::iterator it = s.begin(); it != s.end(); ++it)
    {
        //ch = toLower(ch);
        *it = toLower(*it);
    }

    return s;
}

//-----------------------------------------------------------------------------
//! Просто потому, что standalone модуль и не тащить
inline
std::vector<std::string> toUpper( std::vector<std::string> v )
{
    //for (std::string& s : v) 
    for(std::vector<std::string>::iterator it = v.begin(); it != v.end(); ++it)
    {
        //s = toUpper(s);
        *it = toUpper(*it);
    }

    return v;
}

//! Просто потому, что standalone модуль и не тащить
inline
std::vector<std::string> toLower( std::vector<std::string> v )
{
    //for (std::string& s : v) 
    for(std::vector<std::string>::iterator it = v.begin(); it != v.end(); ++it)
    {
        //s = toLower(s);
        *it = toLower(*it);
    }

    return v;
}


} // namespace utils

//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
//! Чекает сикаблюсь на файлвом дескрипторе
inline
bool seekableChecker(int fd)
{
    off_t off = _tell(fd);
    if (off==(off_t)-1)
        return false;

    // Проблемы под Win10 - простой tell не работает
    // https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/lseek-lseeki64?view=vs-2017
    long seekRes = _lseek(fd, 0, SEEK_SET);
    if (seekRes==-1l)
        return false;

    // В начало сикнулись без проблем

    seekRes = _lseek(fd, 0, SEEK_END);
    if (seekRes==-1l)
        return false;

    // в конец тоже сикнулись без проблем

    // Восстанавливаем, раз уж сикнулись
    _lseek(fd, off, SEEK_SET);

    return true;
}

//-----------------------------------------------------------------------------
//! Чекает, приаттачен ли файловый дескриптор к консольке
inline
bool isConsoleAttached( int fd )
{
    if (seekableChecker(fd))
        return false; // seekable stream - not a console

    if (!_isatty(fd))
        return false;

    struct stat stats;
    fstat( fd, &stats);
    if (!S_ISCHR(stats.st_mode))
        return false;

    return true;
}

//-----------------------------------------------------------------------------
//! Печатает инфу по процессу - для отладки
inline
void showProcessInformation()
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot)
    {
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hSnapshot, &pe32))
        {
            do
            {
                #ifdef DEBUG_WINCONHELPERS
                printf("pid %d %s\n", pe32.th32ProcessID, pe32.szExeFile);
                #endif
            }
            while (Process32Next(hSnapshot, &pe32));
        }
        CloseHandle(hSnapshot);
    }
}

//-----------------------------------------------------------------------------
//! Возвращает имя экзешника процесса по его PID в std::string - пока хватало и без юникода
inline
bool GetProcessExeName( ULONG_PTR /* DWORD */  pid, std::string &processExeName)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot)
    {
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hSnapshot, &pe32))
        {
            do
            {
                if (pid==pe32.th32ProcessID)
                {
                    processExeName = pe32.szExeFile;
                    CloseHandle(hSnapshot);
                    return true;
                }
            }
            while (Process32Next(hSnapshot, &pe32));
        }
        CloseHandle(hSnapshot);
    }

    return false;
}

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
//#include <winternl.h>
#if defined(_MSC_VER) && _MSC_VER<=1400
//! Для старых студий костылёк
typedef enum _PROCESSINFOCLASS {
    ProcessBasicInformation = 0,
    ProcessWow64Information = 26
} PROCESSINFOCLASS;
#endif

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
//! Динамически линкуем NTDLL.DLL!NtQueryInformationProcess
inline
LONG /* NTSTATUS */  NtQueryInformationProcessImpl(
  IN HANDLE           ProcessHandle,
  IN PROCESSINFOCLASS ProcessInformationClass,
  OUT PVOID           ProcessInformation,
  IN ULONG            ProcessInformationLength,
  OUT PULONG          ReturnLength
)
{
    typedef
    LONG (WINAPI *NtQueryInformationProcessFnPtrT)(HANDLE ProcessHandle, ULONG ProcessInformationClass,
            PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength);

    #ifdef __GNUC__
        #pragma GCC diagnostic push
        //#pragma GCC diagnostic ignored "-Wcast-function-type"

        // Ебаный GCC
        // https://en.cppreference.com/w/cpp/numeric/bit_cast
       
        NtQueryInformationProcessFnPtrT NtQueryInformationProcessFnPtr = 0;
        {
            auto procAddr = GetProcAddress(LoadLibraryA("NTDLL.DLL"), "NtQueryInformationProcess");
            std::memcpy( (void*)&NtQueryInformationProcessFnPtr, (void*)&procAddr, sizeof(NtQueryInformationProcessFnPtrT) );
        }

    #else

        // Для нормальных компиляторов оставлю как было
        NtQueryInformationProcessFnPtrT NtQueryInformationProcessFnPtr = (NtQueryInformationProcessFnPtrT)GetProcAddress(LoadLibraryA("NTDLL.DLL"), "NtQueryInformationProcess");
       
        // NtQueryInformationProcessFnPtrT NtQueryInformationProcessFnPtr = reinterpret_cast<NtQueryInformationProcessFnPtrT>(GetProcAddress(LoadLibraryA("NTDLL.DLL"), "NtQueryInformationProcess"));

    #endif

    

    #ifdef __GNUC__
        #pragma GCC diagnostic pop
    #endif


    if (!NtQueryInformationProcessFnPtr)
    {
        return (LONG)0xC0000002L;
    }

    return NtQueryInformationProcessFnPtr( ProcessHandle, ProcessInformationClass, ProcessInformation, ProcessInformationLength, ReturnLength);
}

//-----------------------------------------------------------------------------
//! Получаем PID родительского процесса по хэндлу
inline
ULONG_PTR GetParentProcessIdByHandle( HANDLE hCurProcess = GetCurrentProcess() )
{
    ULONG_PTR pbi[6];
    ULONG ulSize = 0;

    if (NtQueryInformationProcessImpl( hCurProcess, (PROCESSINFOCLASS)0, &pbi, sizeof(pbi), &ulSize) >= 0 && ulSize == sizeof(pbi))
        return pbi[5];
    return (ULONG_PTR)-1;
}

//-----------------------------------------------------------------------------
//! Получаем PID родительского процесса по PIDу текущего
inline
ULONG_PTR GetParentProcessIdByPid( ULONG_PTR /* DWORD */  pid = GetCurrentProcessId() )
{
    #if defined(_MSC_VER) && _MSC_VER<=1400
    HANDLE h = OpenProcess( PROCESS_ALL_ACCESS, FALSE, (DWORD)pid );
    #else
    HANDLE h = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pid );
    #endif
    //HANDLE h = OpenProcess( SYNCHRONIZE, FALSE, pid );
    //HANDLE h = OpenProcess(SYNCHRONIZE, TRUE, pid);
    
    if (h!=0)
    {
        ULONG_PTR res = GetParentProcessIdByHandle( h );
        CloseHandle(h);
        return res;
    }
    return (ULONG_PTR)-1;
}

//-----------------------------------------------------------------------------
//! Получаем набор имен исполняемых файлов родительских процессов (вверх по дереву)
inline
std::vector<std::string> getProcessParentNames( ULONG_PTR pid = GetCurrentProcessId() )
{
    DEBUG_WINCONHELPERS_WRITE_STR("getProcessParentNames enter\n");

    std::set<ULONG_PTR> pidSet; // strange bug - sometimes we got next:
/*
    getProcessParentNames, pid: 5336
    getProcessParentNames, parent name: explorer.exe
    getProcessParentNames - put parent name
    getProcessParentNames, pid: 5240
    getProcessParentNames, parent name: chrome.exe
    getProcessParentNames - put parent name
    getProcessParentNames, pid: 4528
    getProcessParentNames, parent name: chrome.exe
    getProcessParentNames - put parent name
    getProcessParentNames, pid: 5336
    getProcessParentNames, parent name: explorer.exe
    getProcessParentNames - put parent name
    getProcessParentNames, pid: 5240
    getProcessParentNames, parent name: chrome.exe
    getProcessParentNames - put parent name
    getProcessParentNames, pid: 4528
    getProcessParentNames, parent name: chrome.exe
*/

    std::vector<std::string> res;
    bool bFirst = true;
    while(pid!=(ULONG_PTR)-1 && pidSet.find(pid)==pidSet.end())
    {
        DEBUG_WINCONHELPERS_PRINTF("getProcessParentNames, pid: %d\n", (unsigned)pid );
        std::string exeName;
        //#if defined(_MSC_VER) && _MSC_VER<=1400
        //if (!GetProcessExeName( (DWORD)pid, exeName ))
        //#else
        if (!GetProcessExeName( pid, exeName ))
        //#endif
        {
            DEBUG_WINCONHELPERS_WRITE_STR("getProcessParentNames - invalid pid\n");
            pid = (ULONG_PTR)-1;
        }
        else
        {
            if (bFirst)
            {
                DEBUG_WINCONHELPERS_WRITE_STR("getProcessParentNames - dont put self name\n");
                bFirst = false;
                pidSet.insert(pid);
            }
            else
            {
                DEBUG_WINCONHELPERS_PRINTF("getProcessParentNames, parent name: %s\n", exeName.c_str() );
                DEBUG_WINCONHELPERS_WRITE_STR("getProcessParentNames - put parent name\n");
                res.push_back( utils::toLower(exeName) );
                pidSet.insert(pid);
            }
            
            pid = GetParentProcessIdByPid(pid);
        }
    }

    DEBUG_WINCONHELPERS_WRITE_STR("getProcessParentNames leave\n");

    return res;
}

#ifdef UMBA_CXX_HAS_STD11

//-----------------------------------------------------------------------------
//! Тип консоли
enum class ConsoleType
{
    file,
    windows_console,
    ansi_terminal
};

//! Тип консоли - файл
#define UMBA_WCH_CONSOLETYPE_FILE               ConsoleType::file
//! Тип консоли - Windows-консоль
#define UMBA_WCH_CONSOLETYPE_WINDOWS_CONSOLE    ConsoleType::windows_console
//! Тип консоли - ANSI-терминал
#define UMBA_WCH_CONSOLETYPE_ANSI_TERMINAL      ConsoleType::ansi_terminal

#else
//! Тип консоли
enum ConsoleType
{
    ConsoleType_file,            //!< Тип консоли - файл
    ConsoleType_windows_console, //!< Тип консоли - Windows-консоль
    ConsoleType_ansi_terminal    //!< Тип консоли - ANSI-терминал
};

//! Тип консоли - файл
#define UMBA_WCH_CONSOLETYPE_FILE               ConsoleType_file
//! Тип консоли - Windows-консоль
#define UMBA_WCH_CONSOLETYPE_WINDOWS_CONSOLE    ConsoleType_windows_console
//! Тип консоли - ANSI-терминал
#define UMBA_WCH_CONSOLETYPE_ANSI_TERMINAL      ConsoleType_ansi_terminal

#endif

//! Определение типа консоли
inline
ConsoleType detectConsoleType( int fd )
{
    std::vector<std::string> parentNames = getProcessParentNames( );
    std::vector<std::string>::iterator pnIt = parentNames.begin();
    for(; pnIt != parentNames.end(); ++pnIt)
    //for( auto parentName : parentNames )
    {
        std::string & parentName = *pnIt;
        std::string::size_type /* auto */  pos = parentName.find_last_of('.');
        if (pos!=parentName.npos)
        {
            parentName.erase( pos, parentName.npos );

            if (parentName=="bash")
            {
                // Виндовый bash от гита всегда аттачит потоки stdout/stderr к файлам, 
                // потом видимо сам парсит и обрабатывает управляющие escape-последовательности
                // и если вывод на консоль - то раскрашивает, если в файл - то отрезает
                return UMBA_WCH_CONSOLETYPE_ANSI_TERMINAL;
            }
        }
    }

    if (isConsoleAttached( fd ))
        return UMBA_WCH_CONSOLETYPE_WINDOWS_CONSOLE;


    return UMBA_WCH_CONSOLETYPE_FILE;
}


} // namespace winhelpers


//DWORD GetCurrentProcessId();

/*
HANDLE GetCurrentProcess(

);

OpenProcess(PROCESS_ALL_ACCESS, TRUE, procId);

HANDLE OpenProcess(
  DWORD dwDesiredAccess,
  BOOL  bInheritHandle,
  DWORD dwProcessId
);
SYNCHRONIZE

IsWow64Process2 - https://docs.microsoft.com/ru-ru/windows/desktop/api/wow64apiset/nf-wow64apiset-iswow64process2
*/



/* 

SetConsoleTextAttribute работает

Far under ConEmu
192 - rdlc.exe
1080 - cmd.exe
15260 - ConEmuC64.exe
10188 - Far.exe
2628 - ConEmuC.exe
9296 - ConEmu.exe

6028 - rdlc.exe
11228 - cmd.exe
5724 - explorer.exe


git bash - SetConsoleTextAttribute не работает

14820 - rdlc.exe
12128 - cmd.exe
8932 - bash.exe
15000 - bash.exe


*/



