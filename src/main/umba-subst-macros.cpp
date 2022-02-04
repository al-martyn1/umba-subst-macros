/*! \file
    \brief Утилита umba-subst-macros - подстановка макросов вида $(MacroName) во входном файле с записью в выходной
 */

#include "umba/umba.h"
#include "umba/simple_formatter.h"
#include "umba/char_writers.h"

#include "umba/debug_helpers.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <filesystem>

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
    #if defined(_DLL)
        #if defined(UMBA_SUBST_MACROS_USE_FOPEN)
            #undef UMBA_SUBST_MACROS_USE_FOPEN
        #endif
    #endif

#endif




umba::StdStreamCharWriter coutWriter(std::cout);
umba::StdStreamCharWriter cerrWriter(std::cerr);
umba::NulCharWriter       nulWriter;

umba::SimpleFormatter logMsg(&coutWriter);
umba::SimpleFormatter logErr(&cerrWriter);
umba::SimpleFormatter logNul(&nulWriter);

bool logWarnType   = true;
bool logGccFormat  = false;
bool logSourceInfo = false;


#include "log.h"
#include "utils.h"


umba::program_location::ProgramLocation<std::string>   programLocationInfo;


#include "umba/cmd_line.h"


#include "app_ver_config.h"
#include "print_ver.h"

#include "arg_parser.h"



#if (defined(WIN32) || defined(_WIN32)) && defined(_MSC_VER)
void myInvalidParameterHandler(const wchar_t* expression,
   const wchar_t* function, 
   const wchar_t* file, 
   unsigned int line, 
   uintptr_t pReserved)
{
   wprintf(L"Invalid parameter detected in function %s."
            L" File: %s Line: %d\n", function, file, line);
   wprintf(L"Expression: %s\n", expression);
   abort();
}
#endif



int main(int argc, char* argv[])
{
    // abort();

    #if (defined(WIN32) || defined(_WIN32)) && defined(_MSC_VER)
        _invalid_parameter_handler oldHandler, newHandler;
        newHandler = myInvalidParameterHandler;
        oldHandler = _set_invalid_parameter_handler(newHandler);
       
        // Disable the message box for assertions.
        _CrtSetReportMode(_CRT_ASSERT, 0);
    #endif

    umba::time_service::init();
    umba::time_service::start();

    umba::time_service::TimeTick startTick = umba::time_service::getCurTimeMs();


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

    // Force set CLI arguments while running under debugger
    if (umba::isDebuggerPresent())
    {
        argsParser.args.clear();
        argsParser.args.push_back("@E:\\_github\\test_tasks\\test-txt-01.rsp");
        // argsParser.args.push_back(umba::string_plus::make_string(""));
        // argsParser.args.push_back(umba::string_plus::make_string(""));
        // argsParser.args.push_back(umba::string_plus::make_string(""));
    }

    programLocationInfo = argsParser.programLocationInfo;

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


    appConfig = appConfig.getAdjustedConfig(programLocationInfo);
    //pAppConfig = &appConfig;

    if (appConfig.outputFilename.empty())
        appConfig.setOptQuet(true);

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


    // if (appConfig.testVerbosity(VerbosityLevel::detailed))
    //     LOG_MSG_OPT<<"initializing input stream" << endl;

    std::istream *pIn = &std::cin;

    std::ifstream inFile;
    if (!appConfig.inputFilename.empty())
    {
        // if (appConfig.testVerbosity(VerbosityLevel::detailed))
        //     LOG_MSG_OPT<<"opening input file" << endl;

        inFile.open( appConfig.inputFilename, std::ios_base::in );
        if (!inFile)
        {
            LOG_ERR_OPT<<"failed to open input file '"<<appConfig.inputFilename<<"'\n";
            return 1;
        }

        pIn = &inFile;
    }

    std::istream &in = *pIn;



    // if (appConfig.testVerbosity(VerbosityLevel::detailed))
    //     LOG_MSG_OPT<<"initializing output stream" << endl;

    std::ostream *pOut = &std::cout;

    std::ofstream outFile;
    if (!appConfig.outputFilename.empty())
    {
        #if defined(UMBA_SUBST_MACROS_USE_FOPEN)

            // Это - не работает. Падает при сборке в Release (MSVC2019)
            // Хз, почему
          
            if (appConfig.testVerbosity(VerbosityLevel::detailed))
                LOG_MSG_OPT<<"opening output file" << endl;
          
            std::string openMode = "w";
            if (!appConfig.getOptOverwrite())
                openMode.append("x"); // This flag forces the function to fail if the file exists, instead of overwriting it.
          
            if (appConfig.testVerbosity(VerbosityLevel::detailed))
                LOG_MSG_OPT<<"try to create file '" << appConfig.outputFilename << "', open mode: '" << openMode << "'" << endl;
          
            //errno = 0;
            std::FILE* pFile = std::fopen( appConfig.outputFilename.c_str(), openMode.c_str() );
          
            if (appConfig.testVerbosity(VerbosityLevel::detailed))
                LOG_MSG_OPT<<"got pFile: " << pFile << endl;
          
            if (!pFile)
            {
                auto errCode = errno;
                LOG_ERR_OPT<<"failed to open output file '"<<appConfig.outputFilename<<"' - " << std::string(std::strerror(errCode)) << "\n";
                return 1;
            }
          
            if (appConfig.testVerbosity(VerbosityLevel::detailed))
                LOG_MSG_OPT<<"try to open file for writting" << endl;
          
            outFile.open( appConfig.outputFilename, std::ios_base::out | std::ios_base::trunc );
          
            if (pFile)
                std::fclose(pFile);
          
            if (!outFile)
            {
                LOG_ERR_OPT<<"failed to open output file '"<<appConfig.outputFilename<<"'\n";
                return 1;
            }
          
            if (appConfig.testVerbosity(VerbosityLevel::detailed))
                LOG_MSG_OPT<<"output file opened successfully" << endl;

        #else

            std::filesystem::path outputFilenamePath = appConfig.outputFilename;
            if (std::filesystem::exists(outputFilenamePath) && !appConfig.getOptOverwrite())
            {
                auto errCode = errno;
                LOG_ERR_OPT<<"failed to open output file '"<<appConfig.outputFilename<<"' - file already exists\n";
                return 1;
            }
           
            outFile.open( appConfig.outputFilename, std::ios_base::out | std::ios_base::trunc );
           
            if (!outFile)
            {
                LOG_ERR_OPT<<"failed to open output file '"<<appConfig.outputFilename<<"'\n";
                return 1;
            }

        #endif

        pOut = &outFile;

    }

    std::ostream &out = *pOut;

    // if (appConfig.testVerbosity(VerbosityLevel::detailed))
    //     LOG_MSG_OPT<<"ready to process input" << endl;

    auto getter = umba::macros::MacroTextFromMapOrEnvRef<std::string>(appConfig.macros, false /* envAllowed */ );

    std::size_t lineNo = 0;
    std::string line;
    while( std::getline( in, line ) )
    {
        ++lineNo;
        // if (appConfig.testVerbosity(VerbosityLevel::detailed))
        //     LOG_MSG_OPT<<"processing input line #" << lineNo << endl;

        out << umba::macros::substMacros( line, getter, appConfig.getMacrosSubstitutionFlags() ) << std::endl;
    }


    return 0;
}


