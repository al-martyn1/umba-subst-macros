#pragma once

#include <string>
#include <vector>
#include <list>
#include <map>
#include <utility>
#include <algorithm>

#include "umba/program_location.h"
#include "umba/enum_helpers.h"
#include "umba/flag_helpers.h"

#include "umba/macros.h"
#include "umba/macro_helpers.h"


//----------------------------------------------------------------------------





//----------------------------------------------------------------------------
enum class VerbosityLevel
{
     invalid      = -1,
     begin        = 0,

     quet         = 0,   // quet 
     normal       = 1,   // normal - print common details
     config       = 2,   // print common details and app config
     detailed     = 3,   // print common details, app config and all declarations, found in user files
     //extra        = 4,    // print common details, app config and all found declarations

     end          = detailed

};

UMBA_ENUM_CLASS_IMPLEMENT_RELATION_OPERATORS(VerbosityLevel)

//----------------------------------------------------------------------------





//----------------------------------------------------------------------------
struct AppConfig
{

    //------------------------------
    static const unsigned                    ofEmptyOptionFlags      = 0x0000;
    // static const unsigned                    ofNoOutput              = 0x0010; // Do not actually write output files
    // static const unsigned                    ofMain                  = 0x0020; // Print only main files (whish contains main or other entry point)
    // static const unsigned                    ofHtml                  = 0x0040; // Print output in html format
    // static const unsigned                    ofSkipUndocumented      = 0x0080; // Skip undocumented files
    // static const unsigned                    ofRemovePath            = 0x0100; // Remove path from output names

    static const unsigned                    ofKeepUnknown           = 0x0001; // umba::macros::keepUnknownVars ;
    static const unsigned                    ofConditionals          = 0x0002; // umba::macros::conditionAllowed;
    static const unsigned                    ofArgs                  = 0x0004; // umba::macros::argsAllowed;


    static const unsigned                    ofOverwrite             = 0x0010;
    static const unsigned                    ofStdin                 = 0x0020;
    static const unsigned                    ofStdout                = 0x0040;

    static const unsigned                    ofBatch                 = 0x0100;
    static const unsigned                    ofRaw                   = 0x1000;

/*
const int substFlagsDefault                   = smf_Default                        ;
const int argsAllowed                         = smf_ArgsAllowed                    ;
const int conditionAllowed                    = smf_ConditionAllowed               ;
const int appendVarValueAllowed               = smf_AppendVarValueAllowed          ;
const int setVarValueSubstitutionAllowed      = smf_SetVarValueSubstitutionAllowed ;
const int changeDot                           = smf_changeDot                      ;
const int changeSlash                         = smf_changeSlash                    ;
const int uppercaseNames                      = smf_uppercaseNames                 ;
const int lowercaseNames                      = smf_lowercaseNames                 ;
const int disableRecursion                    = smf_DisableRecursion               ;
const int keepUnknownVars                     = smf_KeepUnknownVars                ;
*/

    //------------------------------
    umba::macros::StringStringMap<std::string> macros;
    std::list< std::string >                 macrosOrder;
    std::map<std::string,bool>               expandedMacros;

    std::vector< std::pair<std::string,std::string> >  rawSubstitutions;



    unsigned                                 optionFlags = ofKeepUnknown; // 0; // ofNormalizeFilenames; // ofEmptyOptionFlags;

    VerbosityLevel                           verbosityLevel = VerbosityLevel::normal;

    // std::string                              inputFilename;
    // std::string                              outputFilename;

    std::vector< std::pair< std::string, std::string > >   filesToProcess;

    //------------------------------

    typedef std::string StdString;
    UMBA_ENUM_CLASS_IMPLEMENT_STRING_CONVERTERS_MEMBER( StdString , VerbosityLevel, "quet", "normal", "config", "verbose" /* , "extra" */  )

    //------------------------------



    //------------------------------
    void setVerbosityLevel(VerbosityLevel lvl) { verbosityLevel = lvl; }

    //! Проверяет уровень lvl на предмет допустимости детализации выхлопа в лог для данного уровня.
    /*! Уровень детализации lvl должен быть меньше или равен заданному в конфиге.
     */
    bool testVerbosity(VerbosityLevel lvl) const
    {
        return (verbosityLevel==VerbosityLevel::invalid)
             ? false
             : lvl<=verbosityLevel
             ;
    }

    std::string testVerbosityStringRes(VerbosityLevel lvl) const
    {
        return testVerbosity(lvl) ? "true" : "false";
    }

    //------------------------------

    
    
    //------------------------------
    void ofSet  ( unsigned ofFlags )       { optionFlags |=  ofFlags; }
    void ofReset( unsigned ofFlags )       { optionFlags &= ~ofFlags; }
    bool ofGet  ( unsigned ofFlags ) const { return (optionFlags&ofFlags)==ofFlags; }
    void ofSet  ( unsigned ofFlags , bool setState )
    {
        if (setState) ofSet  (ofFlags);
        else          ofReset(ofFlags);
    }

    static std::string getOptValAsString(unsigned opt)    { return opt ? "Yes" : "No"; }

    static std::string getOptNameString(unsigned ofFlag)
    {
        switch(ofFlag)
        {
            case ofKeepUnknown    : return "Keep unknown macros";
            case ofConditionals   : return "Allow conditional macros";
            case ofArgs           : return "Allow macro arguments";
            case ofOverwrite      : return "Overwrite output file";
            case ofStdin          : return "Use STDIN as unput";
            case ofStdout         : return "Use STDOUT as output";
            case ofBatch          : return "Batch mode";
            case ofRaw            : return "Raw mode";

            default                      : return "Multiple flags taken!!!";
        }
    }

    #define UMBA_PRETTY_HEADERS_APPC_CONFIG_DECLARE_SET_GET_OPT( opt ) \
                void setOpt##opt( bool q ) { ofSet(of##opt,q);      }  \
                bool getOpt##opt( )  const { return ofGet(of##opt); }

    UMBA_PRETTY_HEADERS_APPC_CONFIG_DECLARE_SET_GET_OPT(KeepUnknown )
    UMBA_PRETTY_HEADERS_APPC_CONFIG_DECLARE_SET_GET_OPT(Conditionals)
    UMBA_PRETTY_HEADERS_APPC_CONFIG_DECLARE_SET_GET_OPT(Args        )
    UMBA_PRETTY_HEADERS_APPC_CONFIG_DECLARE_SET_GET_OPT(Overwrite   )
    UMBA_PRETTY_HEADERS_APPC_CONFIG_DECLARE_SET_GET_OPT(Stdin       )
    UMBA_PRETTY_HEADERS_APPC_CONFIG_DECLARE_SET_GET_OPT(Stdout      )
    UMBA_PRETTY_HEADERS_APPC_CONFIG_DECLARE_SET_GET_OPT(Batch       )
    UMBA_PRETTY_HEADERS_APPC_CONFIG_DECLARE_SET_GET_OPT(Raw         )
    // UMBA_PRETTY_HEADERS_APPC_CONFIG_DECLARE_SET_GET_OPT()

    void setOptQuet( bool q ) { q ? setVerbosityLevel(VerbosityLevel::quet) : setVerbosityLevel(VerbosityLevel::normal); }
    //bool getOptQuet( )  const { return testVerbosity(VerbosityLevel::quet); }

    bool getOptShowConfig( )  const { return testVerbosity(VerbosityLevel::config); }

    //------------------------------



    //------------------------------



    //------------------------------
    template<typename StreamType>
    StreamType& printVerbosity( StreamType &s ) const
    {
        s << "Verbosity      : " << VerbosityLevel_toStdString(verbosityLevel) << "\n";
        return s;
    }

    bool isMacroExpanded( const std::string &name ) const
    {
        auto it = expandedMacros.find(name);
        if (it==expandedMacros.end())
            return false;

        return it->second;
    }

    int getMacrosSubstitutionFlags( ) const
    {
        int msFlags = 0;

        if (optionFlags&ofKeepUnknown)
            msFlags |= umba::macros::keepUnknownVars;

        if (optionFlags&ofConditionals)
            msFlags |= umba::macros::conditionAllowed;

        if (optionFlags&ofConditionals)
            msFlags |= umba::macros::argsAllowed;

        return msFlags;
    }

    void setMacro( const std::string &name, std::string val, bool deffered )
    {
        auto orderIt = std::find(macrosOrder.begin(), macrosOrder.end(), name);
        if (orderIt!=macrosOrder.end())
           macrosOrder.erase(orderIt);

        macrosOrder.push_back(name);

        if (deffered)
        {
            expandedMacros[name] = false;
            macros[name]         = val;
            return;
        }

        val = umba::macros::substMacros( val
                                       , umba::macros::MacroTextFromMapOrEnvRef<std::string>(macros, true /* envAllowed */ )
                                       , getMacrosSubstitutionFlags()
                                       );


        expandedMacros[name] = true;
        macros[name]         = val;

    }


    template<typename StreamType>
    StreamType& print( StreamType &s ) const
    {
        s << "\n";
        printVerbosity(s) << "\n";

        //------------------------------

        //s << "Output Name    : " << outputName << "\n"; // endl;

        s << "\n";

        s << "Option Flags   :\n";
        s << "  " << getOptNameString(ofKeepUnknown )            << ": " << getOptValAsString(optionFlags&ofKeepUnknown ) << "\n";
        s << "  " << getOptNameString(ofConditionals)            << ": " << getOptValAsString(optionFlags&ofConditionals) << "\n";
        s << "  " << getOptNameString(ofArgs        )            << ": " << getOptValAsString(optionFlags&ofArgs        ) << "\n";
        s << "  " << getOptNameString(ofOverwrite   )            << ": " << getOptValAsString(optionFlags&ofOverwrite   ) << "\n";
        s << "  " << getOptNameString(ofBatch       )            << ": " << getOptValAsString(optionFlags&ofBatch       ) << "\n";
        s << "  " << getOptNameString(ofRaw         )            << ": " << getOptValAsString(optionFlags&ofRaw         ) << "\n";
        //s << "    " << getOptNameString(ofStdin       )            << ": " << getOptValAsString(optionFlags&ofStdin       ) << "\n";
        //s << "    " << getOptNameString(ofStdout      )            << ": " << getOptValAsString(optionFlags&ofStdout      ) << "\n";
        //s << "    " << getOptNameString(of)            << ": " << getOptValAsString(optionFlags&of) << "\n";
        s << "\n";

        //------------------------------

        s << "\n";
        s << "Macros:\n";

        umba::macros::printMacros(s, umba::string_plus::make_string<std::string>("  "), macros);
        // for(const auto& [name,val] : macros)
        // {
        //     s << "    " << name << " - [" << val << "], expanded: " << (isMacroExpanded(name) ? "true" : "false") << "\n";
        // }

        s << "\n";

        //------------------------------

        // appConfig.macros             = macros        ;
        // appConfig.macrosOrder        = macrosOrder   ;
        // appConfig.expandedMacros     = expandedMacros;


        return s;
    }


    AppConfig getAdjustedConfig( const umba::program_location::ProgramLocation<std::string> &programLocation ) const
    {
        AppConfig appConfig;

        appConfig.optionFlags        = optionFlags;
        appConfig.verbosityLevel     = verbosityLevel;

    // std::pair<std::string,std::string>       rawSubstitutions;
    // unsigned                                 optionFlags = ofKeepUnknown; // 0; // ofNormalizeFilenames; // ofEmptyOptionFlags;


        auto macrosWithLocation      = programLocation.mergeProgramLocationMacros(macros);
        //appConfig.macros             = programLocation.mergeProgramLocationMacros(macros);

        auto getter = umba::macros::MacroTextFromMapOrEnvRef<std::string>(macrosWithLocation, true /* envAllowed */ );

        for(auto orderIt = macrosOrder.begin(); orderIt!=macrosOrder.end(); ++orderIt)
        {
            appConfig.macrosOrder.push_back(*orderIt);

            auto mit = macros.find(*orderIt);
            if (mit==macros.end())
                continue;

            appConfig.rawSubstitutions.push_back( std::make_pair(mit->first, mit->second) );


            if (isMacroExpanded(*orderIt))
            {
                // Already expanded
                appConfig.macros[*orderIt] = mit->second;
            }
            else
            {
                appConfig.macros[*orderIt] = umba::macros::substMacros( mit->second, getter, getMacrosSubstitutionFlags() );
            }

            appConfig.expandedMacros[*orderIt] = true;

        }

        for( const auto &fp : filesToProcess )
        {
            auto in  = umba::macros::substMacros( fp.first  , getter, getMacrosSubstitutionFlags() );
            auto out = umba::macros::substMacros( fp.second , getter, getMacrosSubstitutionFlags() );
            appConfig.filesToProcess.push_back( std::make_pair(in,out) );
        }

        if (appConfig.filesToProcess.empty())
            appConfig.filesToProcess.push_back( std::make_pair(std::string(),std::string()) );

        return appConfig;
    }


    bool isStdoutUsed() const
    {
        for( const auto &fp : filesToProcess )
        {
            if (fp.second.empty())
                return true;
        }

        return false;
    }

}; // struct AppConfig


template<typename StreamType> inline
StreamType& operator<<(StreamType &s, const AppConfig &cfg)
{
    return cfg.print( s );
}



