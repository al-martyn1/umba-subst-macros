#pragma once

#include "umba/umba.h"
#include "umba/simple_formatter.h"
#include "umba/string_plus.h"

#include <string>


//----------------------------------------------------------------------------
static const char *infoLogSectionSeparator = // "--------------------------------------"; // 38
"------------------------------------------------------------------------------"; // 78


//----------------------------------------------------------------------------
template<typename StreamType> inline
StreamType& printInfoLogSectionHeader( StreamType &s, std::string secCaption )
{
    using namespace umba::omanip;

    secCaption += ":";

    s << "\n";
    s << infoLogSectionSeparator << "\n";
    s << caption << secCaption << normal << "\n";
    s << std::string(secCaption.size(), '-');
    s << "\n";

    return s;
}

//----------------------------------------------------------------------------
template<typename StreamType> inline
StreamType& printInfoLogSectionHeader( StreamType &s, const char* secCaption )
{
    return printInfoLogSectionHeader( s, std::string(secCaption) );
}

//----------------------------------------------------------------------------

inline
std::string substRawText( std::string text, const std::vector< std::pair<std::string,std::string> >  &rawSubstitutions )
{
    std::vector< std::string::size_type > positions;




}