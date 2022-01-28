/*! \file
    \brief Утилита umba-brief-scanner
 */

#include "umba/umba.h"
#include "umba/simple_formatter.h"
#include "umba/char_writers.h"

#include "umba/debug_helpers.h"

#include <iostream>
#include <iomanip>
#include <string>
// #include <cstdio>
#include <filesystem>

#include "umba/debug_helpers.h"
#include "umba/string_plus.h"
#include "umba/program_location.h"
#include "umba/scope_exec.h"
#include "umba/macro_helpers.h"
#include "umba/macros.h"

#include "umba/time_service.h"


#include "utils.h"
#include "brief_info.h"


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
#include "scan_folders.h"

//#include "scan_sources.h"

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
        argsParser.args.push_back("@..\\tests\\data\\test01.rsp");
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

    if (appConfig.getOptShowConfig())
    {
        printInfoLogSectionHeader(logMsg, "Actual Config");
        // logMsg << appConfig;
        appConfig.print(logMsg) << "\n";
    }

    if (appConfig.outputName.empty())
    {
        LOG_ERR_OPT << "output name not taken" << endl;
        return 1;
    }



    std::vector<std::string> foundFiles, excludedFiles;
    std::set<std::string>    foundExtentions;
    scanFolders(appConfig, foundFiles, excludedFiles, foundExtentions);


    if (appConfig.testVerbosity(VerbosityLevel::detailed))
    {
        if (!foundFiles.empty())
            printInfoLogSectionHeader(logMsg, "Files for Processing");

        for(const auto & name : foundFiles)
        {
            logMsg << name << endl;
        }


        if (!excludedFiles.empty())
            printInfoLogSectionHeader(logMsg, "Files Excluded from Processing");

        for(const auto & name : excludedFiles)
        {
            logMsg << name << endl;
        }


        if (!foundExtentions.empty())
            printInfoLogSectionHeader(logMsg, "Found File Extentions");

        for(const auto & ext : foundExtentions)
        {
            if (ext.empty())
                logMsg << "<EMPTY>" << endl;
            else
                logMsg << "." << ext << endl;
        }
    }


    if (appConfig.testVerbosity(VerbosityLevel::detailed))
        printInfoLogSectionHeader(logMsg, "Processing");

    std::map<std::string, BriefInfo>  briefInfo;


    for(const auto & filename : foundFiles)
    {
        // logMsg << name << endl;
        std::vector<char> filedata;
        if (!umba::filesys::readFile( filename, filedata ))
        {
            LOG_WARN_OPT("open-file-failed") << "failed to open file '" << filename << "'\n";
            continue;
        }

        BriefInfo  info;
        bool bFound = findBriefInfo( filedata, appConfig.entryNames, info );
        briefInfo[filename] = info;

        if (appConfig.testVerbosity(VerbosityLevel::detailed))
        {
            logMsg << (info.briefFound ? '+' : '-')
                   << (info.entryPoint ? 'E' : ' ')
                   << "    " << filename
                   << "\n";
        }

    }

    std::ofstream infoStream;
    if (!appConfig.getOptNoOutput())
    {

        // if (!createDirectory(path))
        // {
        //     LOG_WARN_OPT("create-dir-failed") << "failed to create directory: " << path << endl;
        //     continue;
        // }

        infoStream.open( appConfig.outputName, std::ios_base::out | std::ios_base::trunc );
        if (!infoStream)
        {
            LOG_WARN_OPT("create-file-failed") << "failed to create output file: " << appConfig.outputName << endl;
            return 1;
        }
    }


    std::string titleStr = "Brief Description for Project Sources";
    std::string sepLine  = "-------------------------------------";

    if (!appConfig.getOptHtml())
    {
        infoStream << titleStr << "\n" << sepLine << "\n\n";
    }
    else
    {
        infoStream << "<!DOCTYPE html>\n<html>\n";
        infoStream << "<head>\n<title>" << titleStr << "</title>\n</head>\n";
        infoStream << "<body>\n";
    }


    auto printInfo = [&]( bool bMain )
    {
        //std::map<std::string, BriefInfo>
        for( const auto& [name,info] : briefInfo)
        {
            if (info.entryPoint!=bMain)
                continue;

            if (appConfig.getOptSkipUndocumented())
            {
                if (!info.briefFound)
                    continue;
            }

            umba::StdStreamCharWriter infoWriter(infoStream);
            umba::SimpleFormatter uinfoStream(&infoWriter);

            auto relName = appConfig.getScanRelativeName(name);

            if (appConfig.getOptRemovePath())
                relName = umba::filename::getFileName( relName );

            if (!appConfig.getOptHtml())
            {
                uinfoStream << width(32) << left << relName << " - " << info.infoText << "\n";
                //infoStream << relName << " - " << info.infoText << "\n";
            }
            else
            {
                //TODO: !!! Add HTML output here
            }
        
        }

    };


    printInfo(true);
        
    if (!appConfig.getOptMain())
    {
        // print all

        if (!appConfig.getOptHtml())
        {
            infoStream << "\n";
        }
        else
        {
            //TODO: !!! Add HTML line break here
        }

        printInfo(false);

    }


    if (appConfig.getOptHtml())
    {
        infoStream << "<body>\n";
        infoStream << "<html>\n";
    }


    if (appConfig.testVerbosity(VerbosityLevel::normal))
        logMsg << "Done";


    return 0;
}


