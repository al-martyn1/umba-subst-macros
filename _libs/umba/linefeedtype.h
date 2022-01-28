#pragma once

//-----------------------------------------------------------------------------

/*! 
    \file
    \brief Объявление типа перевода строки и разные хелперы
 */

//-----------------------------------------------------------------------------

#include "exception.h"

#include <string>
#include <vector>

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
namespace umba
{

/*! \ingroup UMBA_CORE
    \addtogroup UMBA_INTERNALS
 *  \addtogroup UMBA_LINEINFO
 *  @{
 */


//----------------------------------------------------------------------------
//! Тип перевода строки
/*! 
    Немного подробностей есть тут - https://ru.wikipedia.org/wiki/%D0%9F%D0%B5%D1%80%D0%B5%D0%B2%D0%BE%D0%B4_%D1%81%D1%82%D1%80%D0%BE%D0%BA%D0%B8

    Не реализованы дополнительные Юникодные переводы строки.

    Реализован (ошибочно) детект LF CR последовательности
 */
enum LineFeedType
{
    lineFeedUnknown,  //!< Тип перевода строки неизвестен
    lineFeedCRLF,     //!< CR LF - Исторически используемый в Windows перевод строки
    lineFeedLFCR,     //!< LF CR - Ошибочно предполагалось, что такой перевод строки используется в MAC'ах
    lineFeedCR,       //!< CR - Mac OS до версии 9
    lineFeedLF        //!< LF - Перевод строки, традиционно используемый в *nix-подобных системах, а также в Mac OS X
};

//#if !defined(UMBA_MCU_USED)

//----------------------------------------------------------------------------
//! Шаблон, который возвращает текстовое представление перевода строки (для отладки) в виде сырого указателя.
/*! 
    \tparam CharType тип символов в возвращаемой строке
 */
template< typename CharType > inline
const CharType* getLineFeedTypeDebugPresentationPChar(LineFeedType lineFeedType)
{
    UMBA_ASSERT_FAIL();
    return (CharType*)0;
}

//----------------------------------------------------------------------------
//! \ingroup UMBA_INTERNALS
/*! Кишочное */
#define UMBA_LINEFEEDTYPE_DEFINE_ALL_CRLF_STATIC_CONSTS()               \
                static const char *crlf =  "<CR><LF>";                  \
                static const char *lfcr =  "<LF><CR>";                  \
                static const char *cr   =  "<CR>"    ;                  \
                static const char *lf   =  "<LF>"    /* oops */

#define UMBA_LINEFEEDTYPE_DEFINE_ALL_CRLF_STATIC_CONSTS_L( charType )   \
                static const wchar_t *crlf =  L"<CR><LF>";              \
                static const wchar_t *lfcr =  L"<LF><CR>";              \
                static const wchar_t *cr   =  L"<CR>"    ;              \
                static const wchar_t *lf   =  L"<LF>"    /* oops */

//! \ingroup UMBA_INTERNALS
/*! Кишочное */
#define UMBA_LINEFEEDTYPE_MAKELINEFEEDTYPEDEBUGPRESENTATIONSTR_SWITCH(charType)  \
                switch(lineFeedType)                                             \
                {                                                                \
                    case lineFeedCRLF: return crlf;                              \
                    case lineFeedLFCR: return lfcr;                              \
                    case lineFeedCR  : return cr;                                \
                    case lineFeedLF  : return lf;                                \
                    default          : return ( charType *)0;                    \
                }

//----------------------------------------------------------------------------
//! Шаблон, который возвращает текстовое представление перевода строки (для отладки) в виде сырого указателя. Специализация для char
template<> inline
const char* getLineFeedTypeDebugPresentationPChar<char>(LineFeedType lineFeedType)
{
    UMBA_LINEFEEDTYPE_DEFINE_ALL_CRLF_STATIC_CONSTS();
    UMBA_LINEFEEDTYPE_MAKELINEFEEDTYPEDEBUGPRESENTATIONSTR_SWITCH( char );
}

//----------------------------------------------------------------------------
//! Шаблон, который возвращает текстовое представление перевода строки (для отладки) в виде сырого указателя. Специализация для wchar_t
template<> inline
const wchar_t* getLineFeedTypeDebugPresentationPChar<wchar_t>(LineFeedType lineFeedType)
{
    UMBA_LINEFEEDTYPE_DEFINE_ALL_CRLF_STATIC_CONSTS_L();
    UMBA_LINEFEEDTYPE_MAKELINEFEEDTYPEDEBUGPRESENTATIONSTR_SWITCH( wchar_t );
}

//----------------------------------------------------------------------------
//! Шаблон, который возвращает текстовое представление перевода строки (для отладки) в виде строки.
/*! 
    \tparam StringType тип строки
 */
template<typename StringType > inline
StringType makeLineFeedTypeDebugPresentationStr(LineFeedType lineFeedType)
{
    switch(lineFeedType)
    {
        case lineFeedCRLF: //return ( umba::string_plus::make_string<StringType>("<CR><LF>") );
        case lineFeedLFCR: //return ( umba::string_plus::make_string<StringType>("<LF><CR>") );
        case lineFeedCR  : //return ( umba::string_plus::make_string<StringType>("<CR>") );
        case lineFeedLF  : //return ( umba::string_plus::make_string<StringType>("<LF>") );
                           return StringType(getLineFeedTypeDebugPresentationPChar< typename StringType::value_type >(lineFeedType) );
        default          : return ( umba::string_plus::make_string<StringType>("") );
    }
}

//----------------------------------------------------------------------------
//! Возвращает длину перевода строки в символах
inline
std::size_t getLineFeedTypeLength(LineFeedType lineFeedType)
{
    switch(lineFeedType)
    {
        case lineFeedCRLF: return 2;
        case lineFeedLFCR: return 2;
        case lineFeedCR  : return 1;
        case lineFeedLF  : return 1;
        default          : return 0;
    }
    
}

//----------------------------------------------------------------------------
//! Возвращает текстовое представление перевода строки (для отладки) в виде строки по исходной строке с переводом строки.
template<typename StringType > inline
StringType makeStringDebugPresentationStr( const StringType &str /*!< Строка с сырым переводом строки */ )
{
    typedef typename StringType::value_type CharType;
    StringType res; res.reserve(str.size() + 2*str.size()/48);

    typename StringType::size_type i = 0, sz = str.size();
    for(; i!=sz; ++i)
    {
        CharType ch = str[i];
        switch(ch)
        {
            case (CharType)'\r': res.append( getLineFeedTypeDebugPresentationPChar<CharType>(lineFeedCR) ); break;
            case (CharType)'\n': res.append( getLineFeedTypeDebugPresentationPChar<CharType>(lineFeedLF) ); break;
            default            : res.append( 1, ch );
        }
    }

    return res;
}

// End of UMBA_LINEINFO
/*! @}*/


} // namespace umba

