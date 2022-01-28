#pragma once

#include <string>
#include <map>


#include "string_plus.h"



/*

: - in reference to macro perform a parameter substitution (like params in function call)
CALLED_MACRO := $(%0) $(%1) $(%2) $(%3)
                 ^ - number of passed args
usage 
CALLED_MACRO(A:B) expands to '2 A B ' (trailing space included)
CALLED_MACRO(A:B:C) expands to '3 A B C' (no trailing space)


conditions
CONDITIONAL_TEST_EXISTENCE := $(SOME_MACRO_NAME?*A:B)
$(CONDITIONAL_TEST_EXISTENCE) expands to B (macro SOME_MACRO_NAME not exists)

CONDITIONAL_TEST_EXISTENCE := $(SOME_MACRO_NAME?*A:B)
SOME_MACRO_NAME:=
$(CONDITIONAL_TEST_EXISTENCE) expands to A (macro SOME_MACRO_NAME exists)

CONDITIONAL_TEST_EXISTENCE := $(SOME_MACRO_NAME?+A:B)
SOME_MACRO_NAME:=
$(CONDITIONAL_TEST_EXISTENCE) expands to B (macro SOME_MACRO_NAME exists, but empty)

CONDITIONAL_TEST_EXISTENCE := $(%1?+A:B)
SOME_MACRO_NAME:=C
$(CONDITIONAL_TEST_EXISTENCE:SOME_MACRO_NAME) expands to A %1 is a macro, its text is SOME_MACRO_NAME
$(CONDITIONAL_TEST_EXISTENCE:) expands to B

*/

// umba::macros::
namespace umba{
namespace macros{



// subst macros flags
const int smf_Default                         = 0x0000;
const int smf_ArgsAllowed                     = 0x0001;
const int smf_ConditionAllowed                = 0x0002;
const int smf_AppendVarValueAllowed           = 0x0004;
const int smf_SetVarValueSubstitutionAllowed  = 0x0008;
const int smf_changeDot                       = 0x0010;
const int smf_changeSlash                     = 0x0020;
const int smf_uppercaseNames                  = 0x0040;
const int smf_lowercaseNames                  = 0x0080;
const int smf_DisableRecursion                = 0x0200;
const int smf_KeepUnknownVars                 = 0x0400;
// const int smf_EnvVarsAllowed                  = 0x0100;


// subst macros flags
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
// const int smf_EnvVarsAllowed                  = envVarsAllowed                ;




//----------------------------------------------------------------------------
template<typename StringType>
struct IMacroTextGetter
{

    virtual bool operator()(const StringType &name, StringType &text) const = 0;
};

//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
namespace util{




//----------------------------------------------------------------------------
//! Находит символ Ch, которые не заключен в скобки startBr/endBr
template < char startBr
         , char endBr
         , char Ch
         , typename CharType
         , typename Traits
         , typename Allocator
         > inline
typename ::std::basic_string<CharType, Traits, Allocator>::size_type 
findChar(const ::std::basic_string<CharType, Traits, Allocator> &str, typename ::std::basic_string<CharType, Traits, Allocator>::size_type pos = 0)
{
    int depth = 0;
    typename ::std::basic_string<CharType, Traits, Allocator>::size_type size = str.size();
    for(; pos<size; ++pos)
    {
        if (str[pos]==(CharType)startBr) { ++depth; continue; }
        if (str[pos]==(CharType)endBr)   { --depth; continue; }
        if (!depth && str[pos]==(CharType)Ch) return pos;
    }
    return ::std::basic_string<CharType, Traits, Allocator>::npos;
}

//----------------------------------------------------------------------------
//! 
template < typename CharType
         , typename Traits
         , typename Allocator
         > inline
::std::basic_string<CharType, Traits, Allocator> 
prepareMacroName( const ::std::basic_string<CharType, Traits, Allocator>  &name, int flags )
   {
    if (flags&smf_uppercaseNames)
       return umba::string_plus::toupper_copy(name); //, ::std::locale("C"));
    else if (flags&smf_lowercaseNames)
       return umba::string_plus::tolower_copy(name); //, ::std::locale("C"));
    return name;
   }

//-----------------------------------------------------------------------------
template < typename CharType
         , typename Traits
         , typename Allocator
         > inline
::std::basic_string<CharType, Traits, Allocator> filterDotsSlashes(const ::std::basic_string<CharType, Traits, Allocator> &str, int flags )
   {

    ::std::basic_string<CharType, Traits, Allocator> res = str;
    //res.reserve(str.size());
    for(typename ::std::basic_string<CharType, Traits, Allocator>::iterator it = res.begin(); it!=res.end(); ++it)
       {
        if ((*it==(CharType)'\\' || *it==(CharType)'/') && (flags&changeSlash))
           {
            *it = (CharType)'_';
            continue;
           }
        if (*it==(CharType)'.' && (flags&changeDot))
           {
            *it = (CharType)'_';
            continue;
           }
       }
    return res;
   }

//-----------------------------------------------------------------------------
template<typename StringType /* , typename OrgGetter */ >
struct MacroTextGetterProxy : public IMacroTextGetter<StringType>
{
    const std::map<StringType,StringType> &m;
    //const OrgGetter                       &orgGetter;
    const IMacroTextGetter<StringType>    &orgGetter;

    MacroTextGetterProxy( const std::map<StringType,StringType> &_m
                        , const IMacroTextGetter<StringType>    &_orgGetter
                        ) : m(_m), orgGetter(_orgGetter) {}

    virtual bool operator()(const StringType &name, StringType &text) const override
    {
        if (getMacroTextFromMap(m, name, text))
            return true;
        return orgGetter(name, text);
    }

}; // struct MacroTextGetterProxy

//-----------------------------------------------------------------------------
template<typename StringType, typename IntType> inline
StringType toString(IntType i)
{
    if constexpr (sizeof(typename StringType::value_type)>sizeof(char)) // is wide version?
        return std::to_wstring(i);
    else
        return std::to_string(i);

}

//-----------------------------------------------------------------------------




} // namespace util

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
template < typename CharType
         , typename Traits
         , typename Allocator
         >
::std::basic_string<CharType, Traits, Allocator>
substMacros( const ::std::basic_string<CharType, Traits, Allocator>                          &str
           , const IMacroTextGetter< ::std::basic_string<CharType, Traits, Allocator> >      &getMacroText
           , int                                                                             flags
           , std::set< ::std::basic_string<CharType, Traits, Allocator> >                    &usedMacros
           )
   {
    namespace util = ::umba::macros::util;

    typedef ::std::basic_string<CharType, Traits, Allocator> StringType;

    StringType percentZero(1, (CharType)'%'); percentZero.append(1, (CharType)'0');

    typename StringType::const_iterator it = str.begin(), mstartIt = str.end();
    StringType res; res.reserve(str.size());
    //bool prevSlash = false;

    for(; it!=str.end(); ++it)
       {
        if (*it!=(CharType)'$')
           res.append(1, *it);
        else
           {
            mstartIt = it;
            ++it;
            if (it==str.end()) continue;
            
            if (*it==(CharType)'$')
               { 
                res.append(1, *it);
                continue;
               }
            
            if (*it!=(CharType)'(')
               { 
                res.append(1, (CharType)'$');
                res.append(1, *it);
                continue;
               }

            ++it;
            if (it==str.end()) continue;

            typename StringType::const_iterator start = it;
            int brCnt = 1;            
            for(; it!=str.end(); ++it)
               {
                if (*it==(CharType)'(') { ++brCnt; continue; }
                if (*it==(CharType)')') 
                   {
                    --brCnt;
                    if (!brCnt) break;
                   }
               }

            if (it==str.end()) 
               {
                res.append(start, it);
                continue;
               }

            StringType macroName = StringType(start, it);
            //std::string::size_type qPos = macroName.find('?', 0);
            typename StringType::size_type qPos = util::findChar<'(', ')', '?'>(macroName);
            //findChar(const std::string str)
            // ? not found, not an conditional
            if (qPos==StringType::npos)
               {
                std::vector< StringType > parts;
                typename StringType::size_type startPos = 0, nextPos = util::findChar<'(', ')', ':'>(macroName, 0);
                do {
                    if (nextPos!=StringType::npos)
                       {
                        parts.push_back(StringType(macroName, startPos, nextPos-startPos));
                        startPos = nextPos+1;
                        nextPos = util::findChar<'(', ')', ':'>(macroName, startPos);
                       }
                    else
                       {
                        parts.push_back(StringType(macroName, startPos));
                        break;
                       }
                    
                   } while(1);

                //
                if (parts.size()<=1)
                   {
                    StringType 
                           macroNameChanged = util::prepareMacroName(util::filterDotsSlashes(macroName, flags), flags);
                    if (usedMacros.find(macroNameChanged)!=usedMacros.end())
                       continue; // allready used

                    StringType macroText;
                    if (!getMacroText(macroNameChanged, macroText))
                       {
                        if (flags&smf_KeepUnknownVars)
                           {
                            typename StringType::const_iterator endIt = it;
                            ++endIt;
                            res.append(mstartIt,endIt);
                           }
                        continue; // macro not found
                       }

                    std::set< StringType > usedMacrosCopy = usedMacros;
                    usedMacrosCopy.insert(macroNameChanged);
                    res.append((flags&smf_DisableRecursion) ? macroText : substMacros(macroText, getMacroText, flags, usedMacrosCopy));
                    continue;
                   }
                else
                   {
                    if (!(flags&smf_ArgsAllowed))
                       {
                        throw std::runtime_error("Parametrized macros not allowed");
                       }

                    StringType macroNameChanged = util::prepareMacroName(util::filterDotsSlashes(parts[0], flags), flags);
                    
                    if (usedMacros.find(macroNameChanged)!=usedMacros.end())
                       continue; // allready used

                    StringType macroText;
                    if (!getMacroText(macroNameChanged, macroText))
                       continue; // macro not found

                    std::set< StringType > usedMacrosCopy = usedMacros;
                    usedMacrosCopy.insert(macroNameChanged);

                    std::map< StringType, StringType > tmpMacros; // = macros;
                    
                    typename StringType::size_type pi = 1, piSize = parts.size();

                    for(; pi<piSize; ++pi)
                       {
                        std::set< StringType > usedMacrosCopy2 = usedMacrosCopy;
                        parts[pi] = substMacros(parts[pi], getMacroText, flags, usedMacrosCopy2);
                       }

                    tmpMacros[percentZero] = util::toString<StringType>(int(parts.size()) - 1);
                        // util::intToStr< CharType, ::std::char_traits<CharType>, ::std::allocator<CharType> >(int(parts.size())-1);
                    pi = 1;

                    for(; pi<piSize; ++pi)
                       {
                        StringType idxStr = util::toString<StringType>(int(pi)); // ( util::intToStr< CharType, ::std::char_traits<CharType>, ::std::allocator<CharType> >(int(pi)));
                        StringType paramMacroName(1, (CharType)'%'); paramMacroName.append(idxStr);
                        tmpMacros[ paramMacroName ] = parts[pi];
                       }
                    
                    #if 1
                        //!!! Чего-то с прокси не срослось - компилятор помирает от вложенности шаблонов
                        // Порешал, сделав getter нешаблонным параметром с виртуальным оператором ()
                        res.append(substMacros(macroText, util::MacroTextGetterProxy<StringType>(tmpMacros, getMacroText), flags, usedMacrosCopy));
                    #else
                        //!!! Пока не будем ничего делать, потом разберёмся
                        res.append(macroText);
                    #endif
                    continue;
                   }
               }

            //flags = smf_ArgsAllowed|smf_ConditionAllowed
            if (!(flags&smf_ConditionAllowed))
               {
                throw std::runtime_error("Conditional macros not allowed");
               }

            StringType macroNameCond = util::filterDotsSlashes(StringType(macroName, 0, qPos), flags);
            macroNameCond = util::prepareMacroName(macroNameCond, flags);
            ++qPos;
            if (qPos>=macroName.size())
               {
                continue; // no true or false branches
               }

            if (macroName[qPos]!='*' && macroName[qPos]!='+')
               {
                throw std::runtime_error( ::std::string("Conditional macro inclusion (body: '")
                                        + umba::string_plus::make_string<::std::string>(macroName) // MARTY_CON_NS str2con(macroName)
                                        + ::std::string("') - invalid condition, ?* nor ?+ used")
                                        );
                //std::cout<<"Conditional macro inclusion (body: '"<<macroName<<"') - invalid condition, ?* nor ?+ used\n";
                continue;
               }

            bool onlyExist = macroName[qPos]=='*';
            typename StringType::size_type truthBranchStart = ++qPos;
            if (truthBranchStart>=macroName.size())
               {
                continue; // no true or false branches
               }

            typename StringType::size_type colonPos = util::findChar<'(', ')', ':'>(macroName, truthBranchStart);
            
            StringType truthPart, falsePart;
            if (colonPos==StringType::npos || colonPos>=macroName.size())
               {
                truthPart = StringType(macroName, truthBranchStart);
               }
            else
               {
                typename StringType::size_type truthBranchLen = colonPos-truthBranchStart;
                truthPart = StringType(macroName, truthBranchStart, truthBranchLen);
                falsePart = StringType(macroName, truthBranchStart + truthBranchLen+1);
               }

            bool cond = false;
            StringType macroText;
            if (getMacroText(macroNameCond, macroText))
               { // macro exist
                if (onlyExist)
                   cond = true;
                else
                   {
                    #if 0
                    if (! /* ::boost::algorithm:: */ ::marty::util::trim_copy(macroText, ::marty::util::CIsSpace<CharType>()).empty())
                    #endif
                    if (!umba::string_plus::trim_copy(macroText, umba::string_plus::space_pred<typename StringType::value_type>()).empty())
                       cond = true;
                   }
               }

            macroText = cond ? truthPart : falsePart;
            StringType 
                str = (flags&smf_DisableRecursion) ? macroText : substMacros(macroText, getMacroText, flags, usedMacros);
            res.append(str);
           }
       }

    return res;
   }

//-----------------------------------------------------------------------------
template < typename MacroTextGetter
         , typename CharType
         , typename Traits
         , typename Allocator
         >
::std::basic_string<CharType, Traits, Allocator>
substMacros( const ::std::basic_string<CharType, Traits, Allocator> &str
           , const MacroTextGetter                                  &getMacroText
           , int                                                     flags = smf_KeepUnknownVars // smf_ArgsAllowed|smf_ConditionAllowed
           )
   {
    std::set< ::std::basic_string<CharType, Traits, Allocator> > usedMacros;
    return substMacros(str, getMacroText, flags, usedMacros);
   }





} // namespace macros
} // namespace umba


// umba::macros::



