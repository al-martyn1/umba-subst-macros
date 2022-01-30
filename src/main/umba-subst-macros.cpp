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

#include "umba/debug_helpers.h"
#include "umba/string_plus.h"
#include "umba/program_location.h"
#include "umba/scope_exec.h"
#include "umba/macro_helpers.h"
#include "umba/macros.h"

#include "umba/time_service.h"


#include "utils.h"


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



int main(int argc, char* argv[])
{
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
        argsParser.args.push_back("@..\\tests\\macros.rsp");
        // argsParser.args.push_back(umba::string_plus::make_string(""));
        // argsParser.args.push_back(umba::string_plus::make_string(""));
        // argsParser.args.push_back(umba::string_plus::make_string(""));
    }

    programLocationInfo = argsParser.programLocationInfo;

    // Job completed - may be, --where option found
    if (argsParser.mustExit)
        return 0;

    if (!argsParser.quet)
    {
        printNameVersion();
    }

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

    if (!appConfig.outputFilename.empty())
        appConfig.setOptQuet(true);

    if (appConfig.getOptShowConfig())
    {
        printInfoLogSectionHeader(logMsg, "Actual Config");
        argsParser.printBuiltinFileNames( logMsg );
        // logMsg << appConfig;
        appConfig.print(logMsg) << "\n";
    }

    //-------------



    std::istream *pIn = &std::cin;

    std::ifstream inFile;
    if (!appConfig.inputFilename.empty())
    {
        inFile.open( appConfig.inputFilename, std::ios_base::in );
        if (!inFile)
        {
            LOG_ERR_OPT<<"failed to open input file '"<<appConfig.inputFilename<<"'\n";
            return 1;
        }

        pIn = &inFile;
    }

    std::istream &in = *pIn;



    std::ostream *pOut = &std::cout;

    std::ofstream outFile;
    if (!appConfig.outputFilename.empty())
    {
        std::string openMode = "w";
        if (!appConfig.getOptOverwrite())
            openMode.append("x");

        errno = 0;
        std::FILE* pFile = std::fopen( appConfig.outputFilename.c_str(), openMode.c_str() );
        if (!pFile)
        {
            auto errCode = errno;
            LOG_ERR_OPT<<"failed to open output file '"<<appConfig.outputFilename<<"' - " << std::string(std::strerror(errCode)) << "\n";
            return 1;
        }

        outFile.open( appConfig.outputFilename, std::ios_base::out | std::ios_base::trunc );
        std::fclose(pFile);
        if (!outFile)
        {
            LOG_ERR_OPT<<"failed to open output file '"<<appConfig.outputFilename<<"'\n";
            return 1;
        }

        pOut = &outFile;
    }

    std::ostream &out = *pOut;


    auto getter = umba::macros::MacroTextFromMapOrEnvRef<std::string>(appConfig.macros, false /* envAllowed */ );

    std::string line;
    while( std::getline( in, line ) )
    {
        out << umba::macros::substMacros( line, getter, appConfig.getMacrosSubstitutionFlags() ) << std::endl;
    }


    return 0;
}


