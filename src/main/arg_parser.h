#pragma once

#include <stack>

#include "app_config.h"
#include "umba/cmd_line.h"


#if defined(WIN32) || defined(_WIN32)
    #include <shellapi.h>
#endif

AppConfig    appConfig;


struct ArgParser
{

std::stack<std::string> optFiles;


std::string makeAbsPath( std::string p )
{
    std::string basePath;

    if (optFiles.empty())
        basePath = umba::filesys::getCurrentDirectory<std::string>();
    else
        basePath = umba::filename::getPath(optFiles.top());


    return umba::filename::makeAbsPath( p, basePath );

}



// 0 - ok, 1 normal stop, -1 - error
template<typename ArgsParser>
int operator()( const std::string                               &a           //!< строка - текущий аргумент
              , umba::command_line::CommandLineOption           &opt         //!< Объект-опция, содержит разобранный аргумент и умеет отвечать на некоторые вопросы
              , ArgsParser                                      &argsParser  //!< Класс, который нас вызывает, содержит некоторый контекст
              , umba::command_line::ICommandLineOptionCollector *pCol        //!< Коллектор опций - собирает инфу по всем опциям и готов вывести справку
              , bool fBuiltin
              , bool ignoreInfos
              )
{
    //using namespace marty::clang::helpers;

    std::string dppof = "Don't parse predefined options from ";

    if (opt.isOption())
    {
        std::string errMsg;
        int         intVal;
        bool        boolVal;
        std::string strVal;

        if (opt.name.empty())
        {
            LOG_ERR_OPT<<"invalid (empty) option name\n";
            return -1;
        }

       if (opt.isOption("quet") || opt.isOption('q') || opt.setDescription("Operate quetly. Short alias for '--verbose=quet'"))
        {
            argsParser.quet = true;
            appConfig.setOptQuet(true);
        }

        #if defined(WIN32) || defined(_WIN32)
        else if (opt.isOption("home") || opt.setDescription("Open homepage"))
        {
            if (argsParser.hasHelpOption) return 0;
            ShellExecuteA( 0, "open", appHomeUrl, 0, 0, SW_SHOW );
            return 1;
        }
        #endif

        else if (opt.setParam("LEVEL", 1, "0/quet/no/q|" 
                                          "1/normal/n|" 
                                          "2/config/c|" 
                                          "3/detailed/detail/d|" 
                                          // "4/extra/high/e" 
                             )
              || opt.setInitial(1) || opt.isOption("verbose") || opt.isOption('V')
              || opt.setDescription("Set verbosity level. LEVEL parameter can be one of the next values:\n"
                                    "quet - maximum quet mode (same as --quet).\n"
                                    "normal - print common details.\n"
                                    "config - print common details and app config.\n"
                                    "detailed - print common details, app config and all declarations, which are found in user files." // "\n"
                                    // "extra - print common details, app config and all found declarations (from all files)." // "\n"
                                   )
              )
        {
            if (argsParser.hasHelpOption) return 0;

            auto mapper = [](int i) -> VerbosityLevel
                          {
                              //return AppConfig::VerbosityLevel_fromStdString((VerbosityLevel)i);
                              switch(i)
                              {
                                  case  0: case  1: case  2: case  3: case  4: return (VerbosityLevel)i;
                                  default: return VerbosityLevel::begin;
                              }
                          };

            VerbosityLevel lvl;
            if (!opt.getParamValue( lvl, errMsg, mapper ) )
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }

            appConfig.setVerbosityLevel(lvl);
            if (lvl==VerbosityLevel::quet)
                argsParser.quet = true;
        }

        else if ( opt.isBuiltinsDisableOptionMain  () 
               || opt.setDescription( dppof + "main distribution options file '" + argsParser.getBuiltinsOptFileName(umba::program_location::BuiltinOptionsLocationFlag::appGlobal   ) + "'"))
        { } // simple skip - обработка уже сделана

        else if ( opt.isBuiltinsDisableOptionCustom() 
               || opt.setDescription( dppof + "custom global options file '"     + argsParser.getBuiltinsOptFileName(umba::program_location::BuiltinOptionsLocationFlag::customGlobal) + "'"))
        { } // simple skip - обработка уже сделана

        else if ( opt.isBuiltinsDisableOptionUser  () 
               || opt.setDescription( dppof + "user local options file '"        + argsParser.getBuiltinsOptFileName(umba::program_location::BuiltinOptionsLocationFlag::userLocal   ) + "'"))
        { } // simple skip - обработка уже сделана

        else if (opt.isOption("version") || opt.isOption('v') || opt.setDescription("Show version info"))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!ignoreInfos)
            {
                printOnlyVersion();
                return 1;
            }
        }

        else if (opt.isOption("where") || opt.setDescription("Show where the executable file is"))
        {
            if (argsParser.hasHelpOption) return 0;

            LOG_MSG_OPT << programLocationInfo.exeFullName << "\n";
            return 0;
        }

        else if (opt.setParam("CLR", 0, "no/none/file|" 
                                        "ansi/term|" 
                                        #if defined(WIN32) || defined(_WIN32)
                                        "win32/win/windows/cmd/console"
                                        #endif
                             )
              || opt.setInitial(-1) || opt.isOption("color") 
              || opt.setDescription("Force set console output coloring")
              /* ", can be:\nno, none, file - disable coloring\nansi, term - set ansi terminal coloring\nwin32, win, windows, cmd, console - windows console specific coloring method" */
              )
        {
            if (argsParser.hasHelpOption) return 0;

            umba::term::ConsoleType res;
            auto mapper = [](int i) -> umba::term::ConsoleType
                          {
                              switch(i)
                              {
                                  case 0 : return umba::term::ConsoleType::file;
                                  case 1 : return umba::term::ConsoleType::ansi_terminal;
                                  case 2 : return umba::term::ConsoleType::windows_console;
                                  default: return umba::term::ConsoleType::file;
                              };
                          };
            if (!opt.getParamValue( res, errMsg, mapper ) )
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }

            coutWriter.forceSetConsoleType(res);
            cerrWriter.forceSetConsoleType(res);
        }

        //------------

        else if ( opt.isOption("autocomplete-install") 
               || opt.setDescription("Install autocompletion to bash"
                                     #if defined(WIN32) || defined(_WIN32)
                                         "/clink(cmd)"
                                     #endif
                                    )
               )
        {
            if (argsParser.hasHelpOption) return 0;

            //return autocomplete(opt, true);
            return umba::command_line::autocompletionInstaller( pCol, opt, pCol->getPrintHelpStyle(), true, [&]( bool bErr ) -> decltype(auto) { return bErr ? LOG_ERR_OPT : LOG_MSG_OPT; } );
        }
        else if ( opt.isOption("autocomplete-uninstall") 
               || opt.setDescription("Remove autocompletion from bash"
                                     #if defined(WIN32) || defined(_WIN32)
                                         "/clink(cmd)"
                                     #endif
                                    )
                )
        {
            if (argsParser.hasHelpOption) return 0;

            //return autocomplete(opt, false);
            return umba::command_line::autocompletionInstaller( pCol, opt, pCol->getPrintHelpStyle(), false, [&]( bool bErr ) -> decltype(auto) { return bErr ? LOG_ERR_OPT : LOG_MSG_OPT; } );
        }

        else if ( opt.setParam("?MODE",true) 
               || opt.isOption("keep") || opt.isOption('K')
               // || opt.setParam("VAL",true)
               || opt.setDescription("Keep unknown macros (do not replace them to empty string)"))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(boolVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }
            
            appConfig.setOptKeepUnknown(boolVal);
            return 0;
        }

        else if ( opt.setParam("?MODE",false)
               || opt.isOption("conditions") || opt.isOption('C')
               // || opt.setParam("VAL",true)
               || opt.setDescription("Allow conditional macros substitution"))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(boolVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }
            
            appConfig.setOptConditionals(boolVal);
            return 0;
        }

        else if ( opt.setParam("?MODE",false)
               || opt.isOption("parameterized") || opt.isOption('P')
               // || opt.setParam("VAL",true)
               || opt.setDescription("Allow parameterized macros substitution"))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(boolVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }
            
            appConfig.setOptArgs(boolVal);
            return 0;
        }

        else if ( opt.setParam("?MODE",true)
               || opt.isOption("overwrite") || opt.isOption('Y') 
               // || opt.setParam("VAL",true)
               || opt.setDescription("Allow overwrite existing file"))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(boolVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }
            
            appConfig.setOptOverwrite(boolVal);
            return 0;
        }

        else if ( opt.setParam("?MODE",true)
               || opt.isOption("batch") || opt.isOption('B') 
               // || opt.setParam("VAL",true)
               || opt.setDescription("Batch mode - process multiple files instead of a single one. Input and output pairs must be taken in form: InputName=OutputName"))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(boolVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }
            
            appConfig.setOptBatch(boolVal);
            return 0;
        }

        else if ( opt.setParam("?MODE",true)
               || opt.isOption("raw") || opt.isOption('R') 
               // || opt.setParam("VAL",true)
               || opt.setDescription("Raw mode - perform simple text substitutions"))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(boolVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }
            
            appConfig.setOptRaw(boolVal);
            return 0;
        }

        else if ( opt.setParam("NAME:TEXT", umba::command_line::OptionType::optString )
               || opt.isOption("set") || opt.isOption('S')
               // || opt.setParam("VAL",true)
               || opt.setDescription("Set macro NAME with the text TEXT"))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }
            
            std::string name, val;
            umba::string_plus::split_to_pair( strVal, name, val, ':' );
            appConfig.setMacro( name, val, false /* deffered */ );
            return 0;
        }

        else if ( opt.setParam("NAME:TEXT", umba::command_line::OptionType::optString)
               || opt.isOption("deffer") || opt.isOption("set-deffered") || opt.isOption('D')
               // || opt.setParam("VAL",true)
               || opt.setDescription("Set macro NAME with the text TEXT (deffered expansion)"))
        {
            if (argsParser.hasHelpOption) return 0;

            if (!opt.getParamValue(strVal,errMsg))
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }
            
            std::string name, val;
            umba::string_plus::split_to_pair( strVal, name, val, ':' );
            appConfig.setMacro( name, val, true /* deffered */ );
            return 0;
        }

        else if (opt.isHelpStyleOption())
        {
            // Job is done in isHelpStyleOption
        }
        else if (opt.isHelpOption()) // if (opt.infoIgnore() || opt.isOption("help") || opt.isOption('h') || opt.isOption('?') || opt.setDescription(""))
        {
            if (!ignoreInfos)
            {
                if (pCol && !pCol->isNormalPrintHelpStyle())
                    argsParser.quet = true;
                //printNameVersion();
                if (!argsParser.quet)
                {
                    printBuildDateTime();
                    printCommitHash();
                    std::cout<<"\n";
                //printHelp();
                }

                if (pCol && pCol->isNormalPrintHelpStyle() && argsParser.argsNeedHelp.empty())
                {
                    auto helpText = opt.getHelpOptionsString();
                    std::cout << "Usage: " << programLocationInfo.exeName
                              << " [OPTIONS] [input_file [output_file]]\n"
                              << "  If output_file not taken, STDOUT used\n"
                              << "  If input_file not taken, STDIN used\n"
                              << "\nOptions:\n\n"<<helpText;
                }
                
                if (pCol) // argsNeedHelp
                    std::cout<<pCol->makeText( 78, &argsParser.argsNeedHelp );

                return 1;

            }

            return 0; // simple skip then parse builtins
        }
        else
        {
            LOG_ERR_OPT<<"unknown option: "<<opt.argOrg<<"\n";
            return -1;
        }

        return 0;

    } // if (opt.isOption())
    else if (opt.isResponseFile())
    {
        std::string optFileName = makeAbsPath(opt.name);

        optFiles.push(optFileName);

        auto parseRes = argsParser.parseOptionsFile( optFileName );

        optFiles.pop();

        if (!parseRes)
            return -1;

        if (argsParser.mustExit)
            return 1;

        /*
        std::ifstream optFile(opt.name.c_str());
        if (!optFile)
        {
            LOG_ERR_OPT<<"failed to read response file '"<<opt.name<<"'\n";
            return -1;
        }
        else
        {
            std::string optLine;
            while( std::getline( optFile, optLine) )
            {
                umba::string_plus::trim(optLine);
                if (optLine.empty())
                    continue;

                if (umba::command_line::isComment( optLine ))
                    continue;

                int paRes = parseArg( optLine, pCol, false, true );
                if (paRes)
                {
                   return paRes;
                }
            }
        }
        */

        return 0;
    
    }

    //appConfig.clangCompileFlagsTxtFilename.push_back(makeAbsPath(a));

    if (!appConfig.getOptBatch())
    {
        if (appConfig.filesToProcess.empty())
        {
            appConfig.filesToProcess.push_back( std::make_pair(makeAbsPath(a), std::string()) ); // inputFilename
        }

        else if (appConfig.filesToProcess[0].second.empty())
        {
            appConfig.filesToProcess[0].second = makeAbsPath(a); // outputFilename
        }

        else
        {
            LOG_ERR_OPT<<"input/output file names already taken\n";
            return -1;
        }
    }

    else // batch mode
    {
        std::string inputName, outputName;
        if (!umba::string_plus::split_to_pair( a, inputName, outputName, '=' ) || outputName.empty())
        {
            LOG_ERR_OPT<<"output file name not taken in names pair (use '=' to set names pair) \n";
            return -1;
        }

        if (inputName.empty())
        {
            LOG_ERR_OPT<<"input file name not taken in names pair (use '=' to set names pair) \n";
            return -1;
        }

        appConfig.filesToProcess.push_back( std::make_pair(makeAbsPath(inputName), makeAbsPath(outputName)) ); // inputFilename
    
    }


    return 0;

} //


}; // struct ArgParser



class CommandLineOptionCollector : public umba::command_line::CommandLineOptionCollectorImplBase
{
protected:
    virtual void onOptionDup( const std::string &opt ) override
    {
        LOG_ERR_OPT<<"Duplicated option key - '"<<opt<<"'\n";
        throw std::runtime_error("Duplicated option key");
    }

};



