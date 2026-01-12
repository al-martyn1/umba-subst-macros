/*! \file
    \brief Утилита umba-subst-macros - подстановка макросов вида $(MacroName) во входном файле с записью в выходной
 */

#include "umba/umba.h"
#include "umba/simple_formatter.h"
#include "umba/char_writers.h"
//
#include "umba/debug_helpers.h"
//
#include "encoding/encoding.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <cstdio>
#include <cerrno>
#include <cstring>

#if (defined(WIN32) || defined(_WIN32)) && defined(_MSC_VER)
    #include <stdio.h>
    #include <stdlib.h>
    #include <crtdbg.h>
#endif

#include "umba/debug_helpers.h"
#include "umba/string_plus.h"
#include "umba/program_location.h"
#include "umba/scope_exec.h"
#include "umba/macro_helpers.h"
#include "umba/macros.h"
#include "umba/time_service.h"

#include "encoding/encoding.h"

#include "marty_cpp/src_normalization.h"
#include "marty_cpp/c_escape.h"


#include "utils.h"


#define UMBA_SUBST_MACROS_USE_FOPEN

#if defined(_MSC_VER)

// https://docs.microsoft.com/ru-ru/cpp/preprocessor/predefined-macros?view=msvc-170
/*
    Multi-threaded (/MT)
    Multi-threaded Debug (/MTd)
    Multi-threaded DLL (/MD)
    Multi-threaded Debug DLL (/MDd)

    _DLL — определяется как 1, если заданы параметры компилятора /MD или /MDd (Многопоточная библиотека DLL).
           В противном случае — не определяется.

    _MT — определяется как 1, если задан параметр _MT либо /MD
 */


    // Детектим сборку с DLL рантаймом
    // std::fopen в DLL рантайме падает при наличии опции "x"
    // Или надо обновить рантайм - https://docs.microsoft.com/en-us/cpp/windows/universal-crt-deployment?view=msvc-170
    #if defined(_DLL)
        #if defined(UMBA_SUBST_MACROS_USE_FOPEN)
            #undef UMBA_SUBST_MACROS_USE_FOPEN
        #endif
    #endif

#endif




umba::StdStreamCharWriter coutWriter(std::cout);
umba::StdStreamCharWriter cerrWriter(std::cerr);
umba::NulCharWriter       nulWriter;

// umba::SimpleFormatter umbaLogStreamErr(&coutWriter);
// umba::SimpleFormatter umbaLogStreamMsg(&cerrWriter);
// umba::SimpleFormatter umbaLogStreamNul(&nulWriter);
//
// bool logWarnType   = true;
// bool umbaLogGccFormat   = false; // true;
// bool umbaLogSourceInfo  = false;

umba::SimpleFormatter logMsg(&coutWriter);
umba::SimpleFormatter logErr(&cerrWriter);
umba::SimpleFormatter logNul(&nulWriter);

bool logWarnType   = true;
bool logGccFormat  = false;
bool logSourceInfo = false;

// bool bOverwrite    = false;


#include "log.h"
#include "utils.h"


umba::program_location::ProgramLocation<std::string>   programLocationInfo;


#include "umba/cmd_line.h"


#include "app_ver_config.h"
#include "print_ver.h"

#include "arg_parser.h"



#if (defined(WIN32) || defined(_WIN32)) && defined(_MSC_VER)

#if _MSC_VER >= 1900  // Visual Studio 2015 and above
    #define UMBA_SUBST_MACROS_NOEXCEPT noexcept
#else
    #define UMBA_SUBST_MACROS_NOEXCEPT throw()
#endif

void __cdecl myInvalidParameterHandler(const wchar_t* expression,
   const wchar_t* function,
   const wchar_t* file,
   unsigned int line,
   uintptr_t pReserved) UMBA_SUBST_MACROS_NOEXCEPT
{
   UMBA_USED(pReserved);
   wprintf(L"Invalid parameter detected in function %s."
            L" File: %s Line: %d\n", function, file, line);
   wprintf(L"Expression: %s\n", expression);
   abort();
}
#endif


//----------------------------------------------------------------------------
inline
void umbaDelay(unsigned ticks)
{
    // umba::time_service::TimeTick startTick = umba::time_service::getCurTimeMs();
    // umba::time_service::TimeTick curTick   = umba::time_service::getCurTimeMs();
    //  
    // while()

    umba::time_service::delayMs((umba::time_service::TimeTick)ticks);
}

//----------------------------------------------------------------------------
// inline
// bool cheForReadWrite(const std::string &fileName)
// {
//     ate
// }


//----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    // abort();

    #if (defined(WIN32) || defined(_WIN32)) && defined(_MSC_VER)

        // warning C5039: '_set_invalid_parameter_handler': pointer or reference to potentially throwing function passed to 'extern "C"' function under -EHc. Undefined behavior may occur if this function throws an exception.
        #pragma warning(push)
        #pragma warning(disable:5039)

        _invalid_parameter_handler oldHandler, newHandler;
        newHandler = &myInvalidParameterHandler;
        oldHandler = _set_invalid_parameter_handler(newHandler);

        #pragma warning(pop)

        // Disable the message box for assertions.
        _CrtSetReportMode(_CRT_ASSERT, 0);
    #endif

    umba::time_service::init();
    umba::time_service::start();

    // umba::time_service::TimeTick startTick = umba::time_service::getCurTimeMs();


    using namespace umba::omanip;


    auto argsParser = umba::command_line::makeArgsParser( ArgParser()
                                                        , CommandLineOptionCollector()
                                                        , argc, argv
                                                        , umba::program_location::getProgramLocation
                                                            ( argc, argv
                                                            , false // useUserFolder = false
                                                            //, "" // overrideExeName
                                                            )
                                                        );

    // // Force set CLI arguments while running under debugger
    // if (umba::isDebuggerPresent())
    // {
    //     argsParser.args.clear();
    //     argsParser.args.push_back("@E:\\_github\\test_tasks\\test-txt-01.rsp");
    //     // argsParser.args.push_back(umba::string_plus::make_string(""));
    //     // argsParser.args.push_back(umba::string_plus::make_string(""));
    //     // argsParser.args.push_back(umba::string_plus::make_string(""));
    // }

    if (umba::isDebuggerPresent())
    {
        std::string cwd = umba::filesys::getCurrentDirectory(); // <std::string>();
        std::cout << "Working Dir: " << cwd << "\n";
        std::string rootPath;

        #if (defined(WIN32) || defined(_WIN32))


            if (winhelpers::isProcessHasParentOneOf({"devenv"}))
            {
                // По умолчанию студия задаёт текущим каталогом На  уровень выше от того, где лежит бинарник
                // rootPath = umba::filename::makeCanonical(umba::filename::appendPath<std::string>(cwd, "..\\..\\..\\"));
                // rootPath = umba::filename::makeCanonical(umba::filename::appendPath<std::string>(cwd, ".."));
                rootPath = umba::filename::makeCanonical(umba::filename::appendPath<std::string>(cwd, "..\\..\\.."));
                //argsParser.args.push_back("--batch-output-root=D:/temp/mdpp-test");
            }
            else if (winhelpers::isProcessHasParentOneOf({"code"}))
            {
                // По умолчанию VSCode задаёт текущим каталогом тот, где лежит бинарник
                rootPath = umba::filename::makeCanonical(umba::filename::appendPath<std::string>(cwd, "..\\..\\..\\..\\"));
                //argsParser.args.push_back("--batch-output-root=C:/work/temp/mdpp-test");

            }
            else
            {
                //rootPath = umba::filename::makeCanonical(umba::filename::appendPath<std::string>(cwd, "..\\..\\..\\"));
            }

            //#endif

            if (!rootPath.empty())
                rootPath = umba::filename::appendPathSepCopy(rootPath);

            argsParser.args.clear();

            //-S=Clang 12.0.0 (GNU CLI) - amd64 for MSVC :EqqE
            argsParser.args.push_back("--raw");
            argsParser.args.push_back("--verbose=detailed");
            argsParser.args.push_back("--overwrite");
            argsParser.args.push_back("-S:namespace public {=namespace public_ {");
            argsParser.args.push_back("-S:::public::=::public_::");

            // argsParser.args.push_back("--batch-exclude-dir=_libs,libs,_lib,lib,tests,test,rc,_generators,_distr_conf,src,.msvc2019,boost,icons");
            argsParser.args.push_back("@" + rootPath + "tests/vscode-kits/kits-name-replace.rsp");

            argsParser.args.push_back("" + rootPath + "tests/vscode-kits/cmake-tools-kits.auto-scanned.json");
            argsParser.args.push_back("" + rootPath + "tests/vscode-kits/cmake-tools-kits.result.json");

            // argsParser.args.push_back("--overwrite");
            // //argsParser.args.push_back("--doxyfication=always");
            // argsParser.args.push_back("--scan=" + rootPath + "/src");
            // //argsParser.args.push_back("--scan=" + rootPath + "/_libs");
            // argsParser.args.push_back(rootPath + "/doc/_sources_brief.txt");

            //argsParser.args.push_back("tests/doxy");


            // argsParser.args.clear();
            // argsParser.args.push_back("@..\\tests\\data\\test01.rsp");
            //argsParser.args.push_back("@..\\make_sources_brief.rsp");
            // argsParser.args.push_back(umba::string_plus::make_string(""));
            // argsParser.args.push_back(umba::string_plus::make_string(""));
            // argsParser.args.push_back(umba::string_plus::make_string(""));

        #endif
    }



    programLocationInfo = argsParser.programLocationInfo;

    try
    {
        // Job completed - may be, --where option found
        if (argsParser.mustExit)
            return 0;


        if (!argsParser.parseStdBuiltins())
            return 1;
        if (argsParser.mustExit)
            return 0;

        if (!argsParser.parse())
            return 1;
        if (argsParser.mustExit)
            return 0;

    }
    catch(const std::exception &/*e*/)
    {
        LOG_ERR_OPT<<"Duplicated option key - exiting\n";
        return 1;
    }


    appConfig = appConfig.getAdjustedConfig(programLocationInfo);
    //pAppConfig = &appConfig;

    if (appConfig.isStdoutUsed())
    {
        appConfig.setOptQuet(true);
    }


    if (!argsParser.quet)
    {
        printNameVersion();
    }

    if (appConfig.getOptShowConfig())
    {
        printInfoLogSectionHeader(logMsg, "Actual Config");
        argsParser.printBuiltinFileNames( logMsg );
        // logMsg << appConfig;
        appConfig.print(logMsg) << "\n";
    }

    //-------------


    auto macroGetter = umba::macros::MacroTextFromMapOrEnvRef<std::string>(appConfig.macros, false /* envAllowed */ );


    for( const auto &fp : appConfig.filesToProcess )
    {
        std::string inputText;
        encoding::EncodingsApi::codepage_type inputCp = 0;

        // Предполагается, что fileName в формате UTF8

        if (!fp.first.empty())
        {
            #if defined(WIN32) || defined(_WIN32)
            auto fpFirst = encoding::fromUtf8(fp.first);
            #else
            auto fpFirst = fp.first;
            #endif

            // inline bool isFileExistingExclusiveReadableWrittable( const char *fname )
            // inline bool isPathExist(const std::wstring &path)
            // inline bool isPathFile(const std::wstring &path)
            // inline bool isPathDirectory(const wchar_t *path)

            if (!umba::filesys::isPathExist(fpFirst))
            {
                LOG_WARN_OPT("input-not-exist")<<"failed to open input file '"<<fp.first<<"'\n";
                continue; // return 1;
            }

            if (!umba::filesys::isPathFile(fpFirst))
            {
                LOG_WARN_OPT("input-not-file")<<"failed to open input file '"<<fp.first<<"' - not a file\n";
                continue; // return 1;
            }

            bool canOpen = false;
            for(unsigned tryN=appConfig.ioTryCount; tryN-->0; )
            {
                if (umba::filesys::isFileExistingExclusiveReadableWrittable(fpFirst))
                {
                    canOpen = true;
                    break;
                }

                LOG_WARN_OPT("wait-for-input-file")<<"waiting for '"<<fp.first<<"' input file is ready" << "\n";
                umbaDelay(appConfig.ioDelay);

            }

            if (!canOpen)
            {
                LOG_WARN_OPT("input-not-ready")<<"input file '"<<fp.first<<"' not ready for processing\n";
                continue;
            }

            if (!umba::filesys::readFile(fpFirst, inputText))
            {
                LOG_WARN_OPT("failed-read-input")<<"failed to read input file '"<<fp.first<<"'\n";
                continue;
            }

        }
        else
        {
            if (!umba::filesys::readFile(std::cin, inputText))
            {
                LOG_WARN_OPT("input-not-exist")<<"failed to read data from stdin\n";
                continue; // return 1;
            }
        }


        inputText = encoding::toUnicodeAuto(inputText, &inputCp);

        std::string processedText;
        if (!appConfig.getOptRaw())
            processedText = umba::macros::substMacros(inputText, macroGetter, appConfig.getMacrosSubstitutionFlags());
        else
            processedText = substTextRaw(inputText, appConfig.rawSubstitutions);

        // // делаем правильный перевод строки. Лень делать по-другому
        // std::vector<std::string> processedLines = marty_cpp::splitToLinesSimple(processedText, true /* addEmptyLineAfterLastLf */);
        // std::string finalText = marty_cpp::mergeLines(processedLines, detectedLinefeedType, false /* addTrailingNewLine */);
        // std::string bomFinalText = bom + finalText;
        // umba::filesys::writeFile(out, bomFinalText.data(), bomFinalText.size());

        std::string finalText = encoding::fromUnicodeToCodepage(processedText, inputCp);
        // !!! need to add BOM?

        if (fp.second.empty())
        {
            std::cout << finalText;
        }
        else
        {
            bool overwrite = appConfig.getOptOverwrite() ? true : false;

            for(unsigned tryN=appConfig.ioTryCount; tryN-->0; )
            {
                #if defined(WIN32) || defined(_WIN32)
                if (!umba::filesys::writeFile(encoding::fromUtf8(fp.second), finalText, overwrite))
                #else
                if (!umba::filesys::writeFile(fp.second, finalText, overwrite))
                #endif
                {
                    if (tryN==0) // Последняя попытка
                    {
                        LOG_WARN_OPT("write-output-failed")<<"failed to write output file '"<<fp.second<<"'\n";
                        //continue; // return 1;
                    }
                    else
                    {
                        LOG_WARN_OPT("wait-for-output-file")<<"waiting for '"<<fp.first<<"' output file is reeady for writting" << "\n";
                        umbaDelay(appConfig.ioDelay);
                    }

                }
                else
                {
                    break;
                }
            }

        }

    } // for( const auto &fp : appConfig.filesToProcess )


    return 0;
}


