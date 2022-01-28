#pragma once

#include "umba/umba.h"
#include "umba/preprocessor.h"
#include "stl.h"
#include "umba/string_plus.h"
#include "umba/enum_helpers.h"

#include <vector>




//----------------------------------------------------------------------------
//! Реализует битовые операции для флагового enum-типа
#define UMBA_ENUM_CLASS_IMPLEMENT_FLAG_OPERATORS(FlagType)                             \
             UMBA_ENUM_CLASS_IMPLEMENT_BIT_OPERATORS( FlagType )                       \
             UMBA_ENUM_CLASS_IMPLEMENT_UNDERLYING_TYPE_BIT_OPERATORS( FlagType )       \
             UMBA_ENUM_CLASS_IMPLEMENT_UNDERLYING_TYPE_EQUAL_OPERATORS( FlagType )



//----------------------------------------------------------------------------
// umba::flag::
namespace umba{
namespace flag_helpers{



//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// umba::flag::util::
namespace util{




//----------------------------------------------------------------------------
template< typename StringType, typename FlagType > inline
FlagType getAllFlags( const std::map<FlagType,StringType> &flagsMap )
{
    FlagType res = (FlagType)0;
    for(auto [key,val] : flagsMap)
    {
        res |= key;
    }

    return res;
}

//----------------------------------------------------------------------------
template< typename StringType, typename FlagType, typename IntType > inline
StringType toStringImpl2( const std::map<FlagType,StringType> &flagsMap, FlagType flagsVal )
{
    auto intFlagsVal = (IntType)flagsVal;

    IntType intAllFlags = (IntType)getAllFlags(flagsMap);

    // Special case - no flags set - looking up for this case
    // Or - if ORing all flags result is zero
    if (intFlagsVal==0 || intAllFlags==0)
    {
        std::map<FlagType,StringType>::const_iterator it = flagsMap.find(flagsVal);
        if (it==flagsMap.end())
            return umba::string_plus::make_string<StringType>("0");
        else
            return it->second;
    }

    std::vector<StringType> resStrVec;

    for(auto [flag,flagName] : flagsMap)
    {
        IntType iFlags = (IntType)flag;
        if ((intFlagsVal&iFlags)==iFlags)
        {
            resStrVec.push_back(flagName);
            //intAllFlags &= ~iFlags;
            intFlagsVal &= ~iFlags;
        }

        if (intFlagsVal==0)
            break;
    }

    return umba::string_plus::merge<StringType>(resStrVec, umba::string_plus::make_string<StringType>("|") );

}

template< typename StringType, typename FlagType, typename std::enable_if<std::is_enum<FlagType>{}, bool>::type = true > inline
StringType toStringImpl( const std::map<FlagType,StringType> &flagsMap, FlagType flagsVal )
{
    return toStringImpl2< StringType, FlagType, typename std::underlying_type< FlagType >::type >( flagsMap, flagsVal );
}

template< typename StringType, typename FlagType, typename std::enable_if<!std::is_enum<FlagType>{} && std::is_integral< FlagType >{} /* ::value */ , bool>::type = true > inline
StringType toStringImpl( const std::map<FlagType,StringType> &flagsMap, FlagType flagsVal )
{
    return toStringImpl2< StringType, FlagType, FlagType >( flagsMap, flagsVal );
}


} // namespace util

// typedef typename std::underlying_type< EnumType >::type    EnumUnderlyingType;


//----------------------------------------------------------------------------


} // namespace flag_helpers
} // namespace umba

