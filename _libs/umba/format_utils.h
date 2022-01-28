#pragma once

//----------------------------------------------------------------------------

/*! \file
    \brief Утилиты для форматирования
*/

//----------------------------------------------------------------------------

#include "umba/umba.h"
#include "umba/parse_utils.h"

#include <algorithm>

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// umba::format_utils

namespace umba
{



//----------------------------------------------------------------------------
//! Утилиты для форматирования
namespace format_utils
{



//----------------------------------------------------------------------------
//! Режим форматирования - нижний регистр/верхний регистр
enum class CaseParam
{
    lower,  //!< Нижний регистр
    upper   //!< Верхний регистр
};

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
//! Перевод произвольной цифры в символ. Цифры больше 9 переводятся в буквы A-Z в верхнем или нижнем регистре
inline
char digitToChar( unsigned d, CaseParam caseParam = CaseParam::upper )
{
    return (char)( d<10
                 ? '0' + d 
                 : (caseParam==CaseParam::lower ? 'a' : 'A') - 10 + d 
                 );
}

//----------------------------------------------------------------------------
//! Форматирует число
/*! Возвращает число символов, помещенных в буфер.
    Не учитывает и не помещает в конец буфера завершающий 0.

    Форматирование производится с учётом заданной ширины вывода, пустые позиции заполняются символом fillChar, по умолчанию используется пробел.
 */
inline
size_t formatNumber( char* pBuf, size_t bufSize, uint64_t val, int ss, size_t fmtWidth = 0, char fillChar = ' ', CaseParam caseParam = CaseParam::upper )
{
    UMBA_ASSERT(pBuf);
    UMBA_ASSERT(bufSize>0);

    if (!fmtWidth)
        fmtWidth = 1;

    if (fmtWidth>bufSize)
        fmtWidth = bufSize;

    //char* pBufOrg = pBuf;

    size_t placedDigitsCount = 0;

    while( val )
    {
        unsigned d = (unsigned)( val % (uint64_t)ss);

        char ch = fillChar;
        if (!val)
        {
            if (!placedDigitsCount)
                ch = fillChar; // '0';
        }
        else
        {
            ch = digitToChar( d, caseParam );
        }

        val /= (uint64_t)ss;

        if (placedDigitsCount<bufSize)
           pBuf[placedDigitsCount++] = ch;
    }

    while(placedDigitsCount<fmtWidth && placedDigitsCount<bufSize)
    {
        pBuf[placedDigitsCount++] = fillChar;
    }

    if (!placedDigitsCount)
    {
        pBuf[0] = '0';
        ++placedDigitsCount;
    }

    std::reverse( &pBuf[0], &pBuf[placedDigitsCount] );

    return placedDigitsCount;
}

//----------------------------------------------------------------------------
//! Форматирует число
/*! Возвращает число символов, помещенных в буфер.
    Не учитывает и не помещает в конец буфера завершающий 0.

    Форматирование производится с учётом заданной ширины вывода, пустые позиции заполняются символом fillChar, по умолчанию используется пробел.

    \note Версия для uint_t буфера
 */
inline
size_t formatNumber( uint8_t* pBuf, size_t bufSize, uint64_t val, int ss, size_t fmtWidth = 0, char fillChar = ' ' , CaseParam caseParam = CaseParam::upper)
{
    return formatNumber( (char*)pBuf, bufSize, val, ss, fmtWidth, fillChar, caseParam );
}

//----------------------------------------------------------------------------
//! Форматирует число. Знаковая версия
inline
size_t formatNumber( char* pBuf, size_t bufSize, int64_t val, int ss, size_t fmtWidth = 0, char fillChar = ' ', CaseParam caseParam = CaseParam::upper )
{
    if (ss!=10)
        return formatNumber( pBuf, bufSize, (uint64_t)val, ss, fmtWidth, fillChar, caseParam );

    UMBA_ASSERT(pBuf);

    if (val<0)
    {
        UMBA_ASSERT(bufSize>1);
        *pBuf++ = '-';
        bufSize--;
        return 1 + formatNumber( pBuf, bufSize, (uint64_t)-val, ss, fmtWidth ? fmtWidth-1 : (size_t)0, fillChar, caseParam );
    }
    else
    {
        UMBA_ASSERT(bufSize>0);
        return formatNumber( pBuf, bufSize, (uint64_t)val, ss, fmtWidth, fillChar, caseParam );
    }
}

//----------------------------------------------------------------------------
//! Форматирует число. Знаковая версия
/*! 
    \note Версия для uint_t буфера
 */
inline
size_t formatNumber( uint8_t* pBuf, size_t bufSize, int64_t val, int ss, size_t fmtWidth = 0, char fillChar = ' ', CaseParam caseParam = CaseParam::upper )
{
    return formatNumber( (char*)pBuf, bufSize, val, ss, fmtWidth, fillChar, caseParam );
}

//----------------------------------------------------------------------------
//! Форматирует число с фиксированной точкой
inline
size_t formatDecimalFixedPoint( char * pBuf, size_t bufSize, int64_t val, size_t valPrec, size_t fmtPrec, size_t fmtWidth = 0, char sepChar = '.', char fillChar = ' ' )
{
    val = umba::parse_utils::mulPow10( val, (int)fmtPrec - (int)valPrec );

    int64_t rest = (val<0 ? -val : val ) % umba::parse_utils::pow10( (int)fmtPrec );
    val = umba::parse_utils::mulPow10( val, -(int)fmtPrec );

    size_t intPartWidth = fmtWidth;
    if (intPartWidth >= (fmtPrec+1))
        intPartWidth -=  fmtPrec+1;
    else
        intPartWidth = 0;

    size_t intPartLen = formatNumber( pBuf, bufSize, val, 10, intPartWidth, fillChar, CaseParam::upper );
    if (intPartLen>=bufSize)
       return intPartLen;

    pBuf[intPartLen++] = sepChar;
    if (intPartLen>=bufSize)
       return intPartLen;

    return intPartLen + formatNumber( &pBuf[intPartLen], bufSize-intPartLen, (uint64_t)rest, 10, fmtPrec, '0', CaseParam::upper );
}

//----------------------------------------------------------------------------
//! Форматирует число с фиксированной точкой
/*! 
    \note Версия для uint_t буфера
 */
inline
size_t formatDecimalFixedPoint( uint8_t * pBuf, size_t bufSize, int64_t val, size_t valPrec, size_t fmtPrec, size_t fmtWidth = 0, char sepChar = '.', char fillChar = ' ' )
{
    return formatDecimalFixedPoint( (char*)pBuf, bufSize, val, valPrec, fmtPrec, fmtWidth, sepChar, fillChar );
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
} // namespace format_utils
} // namespace umba



