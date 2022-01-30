/*! \file
\brief Простой форматированный вывод
*/

#pragma once

#include <cmath>
#include <cstring>
#include <climits>
#include "umba/stl.h"

#include "umba/i_char_writer.h"
#include "umba/assert.h"

#if !defined(UMBA_MCU_USED)
    #include <string>
#endif

#define UMBA_SIMPLE_FORMATTER_H



namespace umba
{


//-----------------------------------------------------------------------------

namespace format_utils
{


static const size_t integral_max_bits = ( (sizeof(uintptr_t) >= sizeof(uintmax_t)) ? (sizeof(uintptr_t)*CHAR_BIT) : (sizeof(uintmax_t)*CHAR_BIT));

//-----------------------------------------------------------------------------
template<typename IntType, typename BaseType>
size_t formatIntImpl( IntType val, BaseType base, const char *digits, char *pBuf, int width, char fillCh
                    , int groupSize, char groupSep
                    , int &grpSepCounter, int &digitsCounter
                    , std::false_type signedIntType
                    )
{
    if (groupSize<1)
        groupSize = 0;

    grpSepCounter = 0;
    digitsCounter = 0;

    char *pBufEnd = pBuf;
    while(val)
    {
        if (groupSize && digitsCounter && (digitsCounter % groupSize)==0 && val)
        {
            *pBufEnd++ = groupSep;
            grpSepCounter++;
        }
        
        IntType d = val % (IntType)base;
        *pBufEnd++ = digits[d];
        val /= (IntType)base;
        //*pBufEnd = 0; // tmp for debug

        digitsCounter++;
    }

    if (pBufEnd == pBuf)
    {
       *pBufEnd++ = '0';
       digitsCounter++;
    }

    if (width<0)
        width = 0;

    while((int)(pBufEnd - pBuf) < width)
    {
        if (fillCh=='0')
        {
            if (groupSize && digitsCounter && (digitsCounter % groupSize)==0 )
            {
                *pBufEnd++ = groupSep;
                grpSepCounter++;
            }
            digitsCounter++;
        }
        *pBufEnd++ = fillCh;
        //*pBufEnd = 0; // tmp for debug
    }

    std::reverse(pBuf, pBufEnd);

    return (size_t)(pBufEnd - pBuf);
}

// buf must points to an array at least 128 chars length
template<typename IntType, typename BaseType>
size_t formatIntImpl( IntType val, BaseType base, const char *digits, char *pBuf, int width, char fillCh
                    , int groupSize, char groupSep
                    , int &grpSepCounter, int &digitsCounter
                    , std::true_type signedIntType )
{
    UMBA_USED(width);
    UMBA_USED(fillCh);

    if (groupSize<1)
        groupSize = 0;

    grpSepCounter = 0;
    digitsCounter = 0;

    char *pBufEnd = pBuf;
    while(val)
    {
        if (groupSize && digitsCounter && (digitsCounter % groupSize)==0 && val)
        {
            *pBufEnd++ = groupSep;
            grpSepCounter++;
        }

        IntType d = val % (IntType)base;
        *pBufEnd++ = d<0 ? digits[-d] : digits[d];
        val /= (IntType)base;

        digitsCounter++;
    }

    if (pBufEnd == pBuf)
    {
       *pBufEnd++ = '0';
       digitsCounter++;
    }

    std::reverse(pBuf, pBufEnd);

    return (size_t)(pBufEnd - pBuf);
}

template<typename IntType, typename BaseType>
size_t formatIntImpl(IntType val, BaseType base, bool uppercase, char *pBuf, int width, char fillCh, int groupSize, char groupSep, int &grpSepCounter, int &digitsCounter )
{
    if (base<2)
        base = 2;

    if (base>16)
        base = 16;

    static const char upperDigits[] = "0123456789ABCDEF";
    static const char lowerDigits[] = "0123456789abcdef";
    return formatIntImpl(val, base, uppercase ? upperDigits : lowerDigits, pBuf, width, fillCh, groupSize, groupSep, grpSepCounter, digitsCounter, std::is_signed<IntType>());
}


}; // namespace format_utils


namespace formatter_utils
{

class SimpleBoolStack
{
public:

    SimpleBoolStack() : m_stack(0) {}

    void push( bool val )
    {
        m_stack <<= 1;
        if (val)
            m_stack |= 1;
    }

    void pop()
    {
        m_stack >>= 1;
    }

    bool top() const
    {
        return (m_stack&1) ? true : false;
    }


protected:

    uint32_t    m_stack;

}; // class SimpleBoolStack

} // namespace formatter_utils


//-----------------------------------------------------------------------------
class SimpleFormatter;


class SimpleFormatterManipSentry
{
public:
    SimpleFormatterManipSentry( SimpleFormatter &simpleFormatter );
private:
    // disable copying
    SimpleFormatterManipSentry(const SimpleFormatterManipSentry&);
    SimpleFormatterManipSentry& operator=(const SimpleFormatterManipSentry&);
}; // class SimpleFormatterManipSentry


//-----------------------------------------------------------------------------
class SimpleFormatterOutputSentry
{
public:
    SimpleFormatterOutputSentry( SimpleFormatter &simpleFormatter );
    ~SimpleFormatterOutputSentry();
private:
    // disable copying
    SimpleFormatterOutputSentry(const SimpleFormatterOutputSentry&);
    SimpleFormatterOutputSentry& operator=(const SimpleFormatterOutputSentry&);

    SimpleFormatter &m_simpleFormatter;
}; // class SimpleFormatterOutputSentry




enum class ColoringLevel
{
                       // Цвет по умолчанию
    emergency     = 0, // черный текст на красном фоне
    alert         = 1, // черный текст на желтом фоне
    critical      = 2, // черный текст на желтом фоне
    error         = 3, // красный текст на дефолтном фоне
    warning       = 4, // ярко красный (розовый) текст на дефолтном фоне
    notice        = 5, // ярко желтый текст на дефолтном фоне
    info          = 6, // ярко-белый на дефолтном фоне
    debug         = 7, // голубой на дефолтном фоне
    good          = 8, // светло-зеленый на дефолтном фоне
    caption       = 9, // магента на дефолтном фоне
    normal        = 10,  // дефолтный цвет
    num_levels    = 11
};



//-----------------------------------------------------------------------------
namespace omanip
{
    typedef SimpleFormatter& (*SimpleManip)(SimpleFormatter& fmt);

    struct SgrColorManipHelper
    {
        typedef SimpleFormatter& (*SgrColorManip)(SimpleFormatter& fmt, umba::term::colors::SgrColor clr);

        SgrColorManip                  m_manipFn;
        umba::term::colors::SgrColor   m_clr;

        SgrColorManipHelper( SgrColorManip manip, umba::term::colors::SgrColor clr ) : m_manipFn(manip), m_clr(clr) {}

    }; // struct SgrColorManipHelper

    struct ColoringLevelManipHelper
    {
        typedef SimpleFormatter& (*ColoringLevelManip)(SimpleFormatter& fmt, ColoringLevel lvl);

        ColoringLevelManip             m_manipFn;
        ColoringLevel                  m_lvl;

        ColoringLevelManipHelper( ColoringLevelManip manip, ColoringLevel lvl ) : m_manipFn(manip), m_lvl(lvl) {}

    }; // struct ColoringLevelManipHelper

    struct IntManipHelper
    {
        typedef SimpleFormatter& (*SimpleIntManip)(SimpleFormatter& fmt, int i);

        SimpleIntManip  m_manipFn;
        int             m_i;

        IntManipHelper( SimpleIntManip manip, int i ) : m_manipFn(manip), m_i(i) {}

    }; // struct IntManipHelper

    struct Int2ManipHelper
    {
        typedef SimpleFormatter& (*SimpleInt2Manip)(SimpleFormatter& fmt, int i1, int i2);

        SimpleInt2Manip m_manipFn;
        int             m_i1;
        int             m_i2;

        Int2ManipHelper( SimpleInt2Manip manip, int i1, int i2 ) : m_manipFn(manip), m_i1(i1), m_i2(i2) {}

    }; // struct IntManipHelper


} // namespace omanip



//-----------------------------------------------------------------------------
class SimpleFormatter
{

public:

    typedef unsigned FormatFlags;

    static const FormatFlags   dec           = 0x0000; //!< use decimal base for integer I/O
    static const FormatFlags   bin           = 0x0001; //!< use bin base for integer I/O
    static const FormatFlags   oct           = 0x0002; //!< use octal base for integer I/O
    static const FormatFlags   hex           = 0x0003; //!< use hex base for integer I/O
    static const FormatFlags   basefield     = 0x0003; //!< dec|oct|hex|bin. Useful for masking operations

    static const FormatFlags   right         = 0x0000; //!< right adjustment (adds fill characters to the left)
    static const FormatFlags   left          = 0x0004; //!< left adjustment (adds fill characters to the right)
    static const FormatFlags   internal      = 0x0008; //!< default (right adjustment with some exceptions)
    static const FormatFlags   adjustfield   = 0x000C; //!< left|right|default

    static const FormatFlags   boolalpha     = 0x0010; //!< insert and extract bool type in alphanumeric format
    static const FormatFlags   showbase      = 0x0020; //!< generate a prefix indicating the numeric base for integer output 0x, 0 , 0b
    static const FormatFlags   showpoint     = 0x0040; //!< generate a decimal-point character unconditionally for floating-point number output
    static const FormatFlags   showpos       = 0x0080; //!< generate a + character for non-negative numeric output
    static const FormatFlags   uppercase     = 0x0100; //!< replace certain lowercase letters with their uppercase equivalents in certain output operations
    static const FormatFlags   uppercasebase = 0x0200; //!< used with showbase
    static const FormatFlags   uppercaseall  = 0x0300; //!< uppercase|uppercasebase
    static const FormatFlags   fmtauto       = 0x0400; //!< автоматическое форматирование 8,16ти-ричных, двоичных чисел и указателей. Ширина выбирется в зависимости от размера типа, fill - '0', символы - uppercase, префикс - lowercase (указатели без префикса). Также, для целых чисел, если указан флаг showpos, для нуля знак не будет выводится

    static const FormatFlags   fixed         = 0x1000; //!< если задан, то выводится фиксированное количество цифр после запятой, как задано в precision (по умолчанию - 3). Если не задан, то незначащие нули опускаются, а отображением десятичной точки управляет showpoint
    static const FormatFlags   scientific    = 0x2000; //!< не используется, задано для совместимости с std::iostreams
    static const FormatFlags   floatfield    = 0x3000; //!< маска

    //-------------------
    struct FormatState
    {
        FormatFlags flags     = dec | right | fmtauto | showbase | uppercase; // default
        int         width     = 0; // width - auto
        int         precision = 3; // 3 digits after decimal point
        char        fill      = ' ';
        char        decGroupSep = '`';
        char        groupSep = '_';
        int         decGroupSize = 0;
        int         groupSize = 4;
        char        decimalPoint = '.';

    }; // struct FormatState

    //-------------------
    SimpleFormatter(ICharWriter *charWriter);
    explicit SimpleFormatter();

    FormatFlags flags() const;
    FormatFlags flags( FormatFlags flags );
    FormatFlags setf( FormatFlags flags );
    FormatFlags setf( FormatFlags flags, FormatFlags mask );
    FormatFlags unsetf( FormatFlags mask );
    FormatState getState() const;
    void setState( FormatState &st );
    int width() const;
    int width( int w );
    int precision() const;
    int precision( int p );
    char fill() const;
    char fill( char c );
    int base() const;
    int base(int b);
    void flush();
    void waitFlushDone();
    int groupsize() const;
    int groupsize(int size);
    int decgroupsize() const;
    int decgroupsize(int size);
    char groupsep() const;
    char groupsep(char sepCh);
    char decgroupsep() const;
    char decgroupsep(char sepCh);
    char decpoint() const;
    char decpoint(char sepCh);


    void putEndl();
    void putCR();
    void putFF();
    void writeBuf( const uint8_t *pBuf, size_t sz );
    void writeBuf( const char *pBuf, size_t sz );

    void pushLock( bool disableOutput = true );
    void popLock();


    unsigned getColor(ColoringLevel lvl);
    void setColor(ColoringLevel lvl, umba::term::colors::SgrColor clr );
    void coloring( ColoringLevel lvl );
    void coloring( umba::term::colors::SgrColor clr );


    // for iomanips - each manip must call saveFormatState
    void saveFormatState();

    // each builtin output operation must call restoreFormatState before exiting
    void restoreFormatState();

    //-------------------
    // http://ru.cppreference.com/w/cpp/types/enable_if
    // http://scrutator.me/post/2016/12/12/sfinae.aspx
    // http://pcdev.ru/type-traits-enable-if/
    // http://qaru.site/questions/60336/enableif-method-specialization
    // 
    // https://habr.com/post/54762/

    //-------------------
    template<typename T > 
    void formatUnsigned( T val, const FormatState fmtState )
    {
        char numBuf[ 2 * format_utils::integral_max_bits ];

        int fmtBase = baseFromFlags(fmtState.flags);

        const char* prefix = "";
        if (fmtState.flags&showbase)
        {
            prefix = getUnsignedPrefix( fmtState.flags, fmtState.flags&uppercasebase ? true : false );
        }

        int prefixLen = std::strlen(prefix);

        int groupSize = fmtBase==10 ? fmtState.decGroupSize : fmtState.groupSize;
        char groupSep = fmtBase==10 ? fmtState.decGroupSep : fmtState.groupSep;

        int grpSepCounter = 0;
        int digitsCounter = 0;

        int numStrLen = (int)format_utils::formatIntImpl( val, fmtBase
                                                        , (fmtState.flags&uppercase) ? true : false
                                                        , numBuf, fmtState.width - prefixLen, fmtState.fill
                                                        , groupSize, groupSep
                                                        , grpSepCounter, digitsCounter
                                                        );
        
        int totalWidth = numStrLen + prefixLen;
        int fillW      = fmtState.width - totalWidth;
        FormatFlags align = fmtState.flags & adjustfield;

        switch(align)
           {
            case left:
                 if (prefix && prefixLen)
                     writeBuf((const uint8_t*)prefix, prefixLen);
                 writeBuf((const uint8_t*)numBuf, numStrLen);
                 makeFill( fillW, fmtState.fill );
                 break;

            case right:
                 makeFill( fillW, fmtState.fill );
                 if (prefix && prefixLen)
                     writeBuf((const uint8_t*)prefix, prefixLen);
                 writeBuf((const uint8_t*)numBuf, numStrLen);
                 break;

            default: // internal
                 if (prefix && prefixLen)
                     writeBuf((const uint8_t*)prefix, prefixLen);
                 makeFill( fillW, fmtState.fill );
                 writeBuf((const uint8_t*)numBuf, numStrLen);
           }
    }

    //-------------------
    template<typename T > 
    typename std::enable_if< std::is_integral<T>::value
                          && std::is_unsigned<T>::value
                           >::type
    formatValue( T val )
    {

        FormatState uintFmt = m_formatState;

        if ( ((uintFmt.flags&basefield) != dec) && (uintFmt.flags&fmtauto) )
        {
            // автоматическое форматирование 8,16ти-ричных, двоичных чисел.
            // Ширина выбирется в зависимости от размера типа, fill - '0', символы - uppercase, 
            // префикс - lowercase (указатели без префикса)

            //uintFmt.flags |= showbase;
            uintFmt.flags |= uppercase;

            unsigned numBits = sizeof(T)*CHAR_BIT;
            int fmtBase = baseFromFlags(uintFmt.flags);
            int widthNumDigits = 0;
            int prefixLen = 0;
            
            switch(fmtBase)
               {
                case 2:
                     //uintFmt.width = (int)numBits + 2; 
                     widthNumDigits = (int)numBits;
                     prefixLen = 2; // prefix '0b' len - 2
                     break;
                case 8:
                     //uintFmt.width = (int)numBits/3 + 1; // prefix '0' len - 1
                     widthNumDigits = (int)numBits/3;
                     prefixLen = 2; // prefix '0' len - 1
                     break;
                default:
                     //uintFmt.width = (int)numBits/4 + 2; // prefix '0x' len - 2
                     widthNumDigits = (int)numBits/4;
                     prefixLen = 2; // prefix '0x' len - 2
               }

            if (!(uintFmt.flags & showbase))
                prefixLen = 0;

            int numSeps = 0;
            if (uintFmt.groupSize>0)
            {
                numSeps = widthNumDigits / uintFmt.groupSize;
                if ((widthNumDigits%uintFmt.groupSize) == 0 /* numSeps*uintFmt.groupSize */ )
                    numSeps -= 1;
            }
            uintFmt.width = widthNumDigits + numSeps + prefixLen;

            uintFmt.fill = '0';
        }

        formatUnsigned( val, uintFmt );
    }

    //-------------------
    template<typename T > 
    typename std::enable_if< std::is_integral<T>::value
                          && std::is_signed<T>::value
                           >::type
    formatValue( T val )
    {
        if (base()!=10)
        {
            formatValue( typename std::make_unsigned<T>::type(val) );
            return;
        }

        char numBuf[ 2 * format_utils::integral_max_bits ];

        int grpSepCounter = 0;
        int digitsCounter = 0;

        bool showSign = false;
        char sign = '-';
        if (val<0)
        {
            showSign = true;
        }
        else if (m_formatState.flags & showpos)
        {
            // автоматическое форматирование целых - если указан флаг showpos, для нуля знак не будет выводится
            if ( val!=0 || !(m_formatState.flags & fmtauto) )
            {
                showSign = true;
                sign = '+';
            }
        }

        int numStrLen = (int)format_utils::formatIntImpl( val, base(), false, numBuf
                                                        , 0 /* width*/, ' ' /* fillCh */
                                                        , m_formatState.decGroupSize
                                                        , m_formatState.decGroupSep
                                                        , grpSepCounter
                                                        , digitsCounter
                                                        );

        int totalWidth = numStrLen;
        if (showSign)
            totalWidth++;

        int fillW = m_formatState.width - totalWidth;
        FormatFlags align = m_formatState.flags & adjustfield;

        switch(align)
           {
            case left:
                 if (showSign)
                     writeBuf(&sign, 1);
                 writeBuf((const uint8_t*)numBuf, numStrLen);
                 makeFill( fillW, m_formatState.fill );
                 break;

            case right:
                 makeFill( fillW, m_formatState.fill );
                 if (showSign)
                     writeBuf(&sign, 1);
                 writeBuf((const uint8_t*)numBuf, numStrLen);
                 break;

            default: // internal
                 if (showSign)
                     writeBuf(&sign, 1);
                 makeFill( fillW, m_formatState.fill );
                 writeBuf((const uint8_t*)numBuf, numStrLen);
           }
    }

    //-------------------
    template<typename T >
    typename std::enable_if< std::is_floating_point<T>::value >::type
    formatValue( T val )
    {
        bool isUpper = m_formatState.flags&uppercase ? true : false;

        if (std::isnan(val))
        {
            formatValue( getNanStr(isUpper) );
            return;
        }

        bool bNeg = false;
        if (val<0.0f)
        {
            bNeg = true;
            val = -val;
        }

        bool bZero = false;

        char numBuf[ 2 * format_utils::integral_max_bits / 3 ];
        char* pStrNum = (char*)getInfStr(isUpper);
        char* pStrNumCurPos = numBuf;

        if (std::isinf(val))
        {
            pStrNumCurPos = pStrNum + std::strlen(pStrNum);
        }
        else
        {
            pStrNum = numBuf;

            int prec = m_formatState.precision;
            if (prec<0)
                prec = -prec;
            if (prec>12)
                prec = 12;

            // 18 446 744 073 709 551 616 max uint64_t
            uint64_t fixedPointVal = ((long double)val) * std::pow( 10.0L, prec+1 );
            uint8_t fixedPointDigits[ format_utils::integral_max_bits / 3 + 1 ];
            std::memset( (void*)fixedPointDigits, (int)0, sizeof(fixedPointDigits) );

            bZero = (fixedPointVal==0);

            // Десятичные разряды располагаются в памяти от младших к старшим
            size_t numDigits = 0;
            while(fixedPointVal)
            {
                fixedPointDigits[numDigits++] = fixedPointVal%10;
                fixedPointVal /= 10;
            }

            if (numDigits<(prec+1))
                numDigits = prec+1;

            // Округляем
            if (fixedPointDigits[0]>4)
            {
                size_t incIdx = 1;
                while(true)
                {
                    fixedPointDigits[incIdx] += 1;
                    if (fixedPointDigits[incIdx] < 10)
                        break;
                    fixedPointDigits[incIdx] -= 10;
                    incIdx++;
                }
                incIdx++;
                if (incIdx>numDigits)
                    numDigits = incIdx;
            }

            fixedPointDigits[0] = 0;
            size_t fractionPartBeginIdx  = 1;
            size_t intPartBeginIdx = prec + 1;

            if (!(m_formatState.flags&fixed))
            {
                // Отбрасываем незначащие нули
                while( (fixedPointDigits[fractionPartBeginIdx]==0) && (fractionPartBeginIdx!=intPartBeginIdx) )
                    fractionPartBeginIdx++;
            }

            //bool hasVisibleFractionPart = true;
            
            if (fractionPartBeginIdx==intPartBeginIdx)
            {
                // Нет дробной части
                /*
                if (prec>0)
                {
                    fractionPartBeginIdx--;
                }
                else
                */
                {
                    
                    if (m_formatState.flags&showpoint)
                    {
                        // force show point
                        if (prec>0) // если точность задана нулевая, точку не отображаем
                        {
                            *pStrNumCurPos++ = '0';
                            *pStrNumCurPos++ = m_formatState.decimalPoint;
                        }
                    }
                    else
                    {
                     //hasVisibleFractionPart = false;
                    }
                }
            }
            else
            {
                for(; fractionPartBeginIdx!=intPartBeginIdx; ++fractionPartBeginIdx)
                {
                    *pStrNumCurPos++ = '0' + fixedPointDigits[fractionPartBeginIdx];
                }
                *pStrNumCurPos++ = m_formatState.decimalPoint;
            }

            // форматируем целую часть

           int groupSize = m_formatState.decGroupSize;
           if (groupSize<1)
              groupSize = 0;

           int grpSepCounter = 0;
           int intPartDigitsCounter = 0;
           char groupSep = m_formatState.decGroupSep;

           //while(fixedPointDigits[intPartBeginIdx])
           while(intPartBeginIdx!=numDigits)
           {
               if (groupSize && intPartDigitsCounter && (intPartDigitsCounter % groupSize)==0 && fixedPointDigits[intPartBeginIdx+1])
               {
                   *pStrNumCurPos++ = groupSep;
                   grpSepCounter++;
               }
        
               *pStrNumCurPos++ = '0' + fixedPointDigits[intPartBeginIdx];
               intPartBeginIdx++;
               intPartDigitsCounter++;
           }

           //if (!intPartDigitsCounter && ( !hasVisibleFractionPart || (m_formatState.flags&fixed) ) )
           if (!intPartDigitsCounter )
           {
               *pStrNumCurPos++ = '0';
               intPartBeginIdx++;
               intPartDigitsCounter++;
           }

           std::reverse( pStrNum, pStrNumCurPos );
        }

        bool showSign = false;
        char sign = '-';
        if (bNeg)
        {
            showSign = true;
        }
        else if (m_formatState.flags & showpos)
        {
            // автоматическое форматирование целых - если указан флаг showpos, для нуля знак не будет выводится
            if ( bZero || !(m_formatState.flags & fmtauto) )
            {
                showSign = true;
                sign = '+';
            }
        }

        int numStrLen = (int)(pStrNumCurPos - pStrNum);

        int totalWidth = numStrLen;
        if (showSign)
            totalWidth++;

        int fillW = m_formatState.width - totalWidth;
        FormatFlags align = m_formatState.flags & adjustfield;

        switch(align)
           {
            case left:
                 if (showSign)
                     writeBuf(&sign, 1);
                 writeBuf((const uint8_t*)numBuf, numStrLen);
                 makeFill( fillW, m_formatState.fill );
                 break;

            case right:
                 makeFill( fillW, m_formatState.fill );
                 if (showSign)
                     writeBuf(&sign, 1);
                 writeBuf((const uint8_t*)numBuf, numStrLen);
                 break;

            default: // internal
                 if (showSign)
                     writeBuf(&sign, 1);
                 makeFill( fillW, m_formatState.fill );
                 writeBuf((const uint8_t*)numBuf, numStrLen);
           }
    }
/*

    static const FormatFlags   showpoint     = 0x0040; //!< generate a decimal-point character unconditionally for floating-point number output
    static const FormatFlags   fixed         = 0x1000; //!< если задан, то выводится фиксированное количество цифр после запятой, как задано в precision (по умолчанию - 3). Если не задан, то незначащие нули опускаются, а отображением десятичной точки управляет showpoint
    static const FormatFlags   scientific    = 0x2000; //!< не используется, задано для совместимости с std::iostreams
    static const FormatFlags   floatfield    = 0x3000; //!< маска


        FormatFlags flags     = dec | right | fmtauto | showbase | uppercase; // default
        int         width     = 0; // width - auto
        int         precision = 3; // 3 digits after decimal point
        char        fill      = ' ';
        char        decGroupSep = '`';
        char        groupSep = '_';
        int         decGroupSize = 0;
        int         groupSize = 4;
        char        decimalPoint = '.';

*/
    //<cmath>

    //-------------------
    template<typename T >
    typename std::enable_if<std::is_pointer<T>::value >::type
    formatValue( T val )
    {
        // автоматическое форматирование указателей. Ширина выбирется в зависимости от размера типа, fill - '0', символы - uppercase, без префикса

        FormatState uintFmt = m_formatState;

        if ( uintFmt.flags&fmtauto )
        {
            uintFmt.flags &= ~showbase;
            uintFmt.flags |= uppercase;
            uintFmt.flags &= basefield;
            uintFmt.flags |= hex;

            uintFmt.width = (int)(sizeof(T)*CHAR_BIT)/4;
            uintFmt.fill = '0';
        }

        formatUnsigned( (uintptr_t)val, uintFmt );
    }

/*
    template<typename T >
    typename std::enable_if< std::is_pointer<T>::value 
                          && std::is_object< typename std::remove_pointer<T>::type >::value
                          && std::is_base_of< umba::IUnknown, typename std::remove_pointer<T>::type >::value
                           >::type
    formatValue( T val )
    {
        m_charWriter->writeString("UNK");
    }
*/

    // is_polymorphic
    // std::is_class 

/*
        if (auto res = eventSource->queryInterface<umba::INamedResourceOwner>())
        {
           //res.value->eventSource->getResourceName();
           if (res.value->getResourceName())
               m_out<<"["<<res.value->getResourceName()<<"] : ";
           else
               m_out<<"[unknown] : ";
        }
        else
        {
        }
*/

    //-------------------
    void formatValue( const char* str )
    {
        int strLen = 0;
        if (str)
            strLen = std::strlen(str);

        int fillW = m_formatState.width - strLen;

        FormatFlags align = m_formatState.flags & adjustfield;

        if (align==left)
        {
             if (strLen)
                 writeBuf((const uint8_t*)str, strLen);
             makeFill( fillW, m_formatState.fill );
        }
        else // right, internal 
        {
             makeFill( fillW, m_formatState.fill );
             if (strLen)
                 writeBuf((const uint8_t*)str, strLen);
        }
    }

    #if !defined(UMBA_MCU_USED)
    void formatValue( const std::string &s )
    {
        formatValue( s.c_str() );
    }
    #endif

    //-------------------
    void formatValue( char ch )
    {

        FormatFlags align = m_formatState.flags & adjustfield;
        if (align==left)
        {
             writeBuf(&ch, 1);
             makeFill( m_formatState.width - 1, m_formatState.fill );
        }
        else // right, internal 
        {
             makeFill( m_formatState.width - 1, m_formatState.fill );
             writeBuf(&ch, 1);
        }

    }

    //-------------------
    void formatValue( bool b )
    {
        static const char* bools[] = { "0", "1"
                                     , "false", "true"
                                     , "FALSE", "TRUE"
                                     };
        unsigned idx = b ? 1 : 0;
        if (m_formatState.flags&boolalpha)
        {
            idx += 2;
            if (m_formatState.flags&uppercase)
                idx += 2;
        }

        formatValue( bools[idx] );
    }

    //-------------------
    template<typename T>
    SimpleFormatter& operator<<( T t )
    {
        SimpleFormatterOutputSentry sentry(*this);
        formatValue(t);
        return *this;
    }

    SimpleFormatter& operator<<( const char* t )
    {
        SimpleFormatterOutputSentry sentry(*this);
        formatValue(t);
        return *this;
    }

    SimpleFormatter& operator<<( char* t )
    {
        SimpleFormatterOutputSentry sentry(*this);
        formatValue((const char*)t);
        return *this;
    }

    //-------------------
    SimpleFormatter& operator<<( omanip::SimpleManip manip )
    {
        SimpleFormatterManipSentry sentry(*this);
        return manip(*this);
    }

    //-------------------
    SimpleFormatter& operator<<( omanip::IntManipHelper manipHelper )
    {
        SimpleFormatterManipSentry sentry(*this);
        return manipHelper.m_manipFn(*this, manipHelper.m_i);
    }

    //-------------------
    SimpleFormatter& operator<<( omanip::Int2ManipHelper manipHelper )
    {
        SimpleFormatterManipSentry sentry(*this);
        return manipHelper.m_manipFn(*this, manipHelper.m_i1, manipHelper.m_i2);
    }

    //-------------------
    SimpleFormatter& operator<<( omanip::SgrColorManipHelper manipHelper )
    {
        return manipHelper.m_manipFn(*this, manipHelper.m_clr);
    }

    //-------------------
    SimpleFormatter& operator<<( omanip::ColoringLevelManipHelper manipHelper )
    {
        return manipHelper.m_manipFn(*this, manipHelper.m_lvl);
    }

    bool isTextMode()
    {
        if (!m_charWriter)
            return false;

        return m_charWriter->isTextMode();
    }

    size_t getNonBlockMax()
    {
        if (!m_charWriter)
            return 0;

        return m_charWriter->getNonBlockMax();
    }


    class CharWriterProxy : public ICharWriter
    {
    
        friend class SimpleFormatter;

        virtual
        void writeChar( char ch ) override
        {
            m_pFormatter->writeBuf( &ch, 1 );
        }

    
        virtual
        void writeBuf( const uint8_t* pBuf, size_t len ) override
        {
            m_pFormatter->writeBuf( pBuf, len );
        }
    
        virtual
        void writeString( const char* str ) override
        {
            m_pFormatter->writeBuf( str, std::strlen(str) );
        }
    
        virtual
        void flush() override
        {
            m_pFormatter->flush();
        }
    
        virtual
        void waitFlushDone() override
        {
            m_pFormatter->waitFlushDone();
        }
    
        virtual
        void putDefEndl() override
        {
            m_pFormatter->m_charWriter->putDefEndl();
        }

        virtual
        void putEndl() override
        {
            m_pFormatter->putEndl();
        }
    
        virtual
        void putCR() override
        {
            m_pFormatter->putCR();
        }
    
        virtual
        void putFF() override
        {
            m_pFormatter->putFF();
        }

        virtual
        bool isTextMode() override
        {
            return m_pFormatter->isTextMode();
        }
    
        virtual
        bool isTerminal() override
        {
            return m_pFormatter->m_charWriter->isTerminal();
        }
    
        virtual
        bool isAnsiTerminal() override
        {
            return m_pFormatter->m_charWriter->isAnsiTerminal();
        }
    
        virtual
        size_t getNonBlockMax() override
        {
            return m_pFormatter->getNonBlockMax();
        }
    
        virtual void setTermColors(term::colors::SgrColor clr) override
        {
            m_pFormatter->m_charWriter->setTermColors(clr);
        }

        virtual void terminalMove2Abs0()              override { m_pFormatter->m_charWriter->terminalMove2Abs0();  }
        virtual void terminalMove2Down()              override { m_pFormatter->m_charWriter->terminalMove2Down();  }
        virtual void terminalMove2Line0()             override { m_pFormatter->m_charWriter->terminalMove2Line0(); }
        virtual void terminalMove2LinePos( int pos )  override { m_pFormatter->m_charWriter->terminalMove2LinePos( pos ); }
        virtual void terminalMove2Pos( int x, int y ) override { m_pFormatter->m_charWriter->terminalMove2Pos( x, y ); }

        virtual void terminalClearLine( int maxPosToClear=-1 ) override { m_pFormatter->m_charWriter->terminalClearLine(maxPosToClear); }
        virtual void terminalClearRemaining(int maxLines = -1) override { m_pFormatter->m_charWriter->terminalClearRemaining(maxLines); }

        virtual void terminalSetSpinnerMode( bool m ) override { m_pFormatter->m_charWriter->terminalSetSpinnerMode(m); }
        virtual void terminalSetCaret( int csz ) override { m_pFormatter->m_charWriter->terminalSetCaret( csz ); }

    
    protected:
    
        SimpleFormatter *m_pFormatter;

        CharWriterProxy( SimpleFormatter *pFormatter )
        : m_pFormatter(pFormatter)
        {}
    
    };

    friend class CharWriterProxy;


    ICharWriter* getCharWritter()
    {
        return &m_charWriterProxy;
    }

    void setCharWritter( ICharWriter * pCharWriter )
    {
        m_charWriter = pCharWriter;
    }


private:

    const char* getUnsignedPrefixLower( FormatFlags f )
    {
        static const char* prefixes[] = { ""   // dec = 0x0000
                                        , "0b" // bin = 0x0001
                                        , "0o" // oct = 0x0002
                                        , "0x" // hex = 0x0003
                                        };
        return prefixes[ f & basefield ];
    }

    const char* getUnsignedPrefixUpper( FormatFlags f )
    {
        static const char* prefixes[] = { ""   // dec = 0x0000
                                        , "0B" // bin = 0x0001
                                        , "0O" // oct = 0x0002
                                        , "0X" // hex = 0x0003
                                        };
        return prefixes[ f & basefield ];
    }

    const char* getUnsignedPrefix( FormatFlags f, bool bUppercase )
    {
        return bUppercase ? getUnsignedPrefixUpper( f ) : getUnsignedPrefixLower( f );
    }

    const char* getNanStr( bool bUppercase )
    {
        static const char* nans[] = { "nan", "NAN" };
        return nans[ bUppercase ? 1 : 0 ];
    }

    const char* getInfStr( bool bUppercase )
    {
        static const char* infs[] = { "inf", "INF" };
        return infs[ bUppercase ? 1 : 0 ];
    }


    void makeFill( int s, char ch)
    {
        if (s<0)
           s = 0;

        for(int i=0; i!=s; ++i)
        {
            writeBuf(&ch, 1);
        }
    }

    int baseFromFlags(FormatFlags flags) const;
    FormatFlags baseToFlags(int b) const;


    // disable copying
    SimpleFormatter(const SimpleFormatter &);
    SimpleFormatter& operator=(const SimpleFormatter &);


    ICharWriter    *m_charWriter = 0;

    CharWriterProxy m_charWriterProxy;

    FormatState     m_formatState;
    FormatState     m_formatStateSaved;
    bool            m_stateSaved = false;


    typedef formatter_utils::SimpleBoolStack disable_stack_type;
    disable_stack_type  m_disableStack;
    bool                m_disableOutput;


    
    umba::term::colors::SgrColor m_coloringLevelColors[ (unsigned)ColoringLevel::num_levels ] = // fgColor, bgColor, fBright, fInvert, fBlink
    { UMBA_TERM_COLORS_MAKE_COMPOSITE( umba::term::colors::black        , umba::term::colors::red          , false ,  false,  false ) // emergency     = 0, // черный текст на красном фоне                  
    , UMBA_TERM_COLORS_MAKE_COMPOSITE( umba::term::colors::black        , umba::term::colors::red          , false ,  false,  false ) // alert         = 1, // красный текст на светло-красном фоне                   
    , UMBA_TERM_COLORS_MAKE_COMPOSITE( umba::term::colors::black        , umba::term::colors::red          , false ,  false,  false ) // critical      = 2, // черный текст на желтом фоне                   
    , UMBA_TERM_COLORS_MAKE_COMPOSITE( umba::term::colors::black        , umba::term::colors::red          , false ,  false,  false ) // error         = 3, // красный текст на дефолтном фоне               
    , UMBA_TERM_COLORS_MAKE_COMPOSITE( umba::term::colors::red          , umba::term::colors::color_default, true  ,  false,  false ) // warning       = 4, // ярко красный (розовый) текст на дефолтном фоне
    , UMBA_TERM_COLORS_MAKE_COMPOSITE( umba::term::colors::yellow       , umba::term::colors::color_default, true  ,  false,  false ) // notice        = 5, // ярко желтый текст на дефолтном фоне           
    , UMBA_TERM_COLORS_MAKE_COMPOSITE( umba::term::colors::white        , umba::term::colors::color_default, true  ,  false,  false ) // info          = 6, // ярко-белый на дефолтном фоне                  
    , UMBA_TERM_COLORS_MAKE_COMPOSITE( umba::term::colors::blue         , umba::term::colors::color_default, true  ,  false,  false ) // debug         = 7, // голубой на дефолтном фоне                     
    , UMBA_TERM_COLORS_MAKE_COMPOSITE( umba::term::colors::green        , umba::term::colors::color_default, true  ,  false,  false ) // good          = 8, // светло-зеленый на дефолтном фоне              
    , UMBA_TERM_COLORS_MAKE_COMPOSITE( umba::term::colors::magenta      , umba::term::colors::color_default, true  ,  false,  false ) // caption       = 9, // магента на дефолтном фоне              
    , UMBA_TERM_COLORS_MAKE_COMPOSITE( umba::term::colors::color_default, umba::term::colors::color_default, false ,  false,  false ) // normal        = 10, // дефолтный цвет                               
    };


}; // class SimpleFormatter



//-----------------------------------------------------------------------------
inline
SimpleFormatterManipSentry::SimpleFormatterManipSentry( SimpleFormatter &simpleFormatter )
{
    simpleFormatter.saveFormatState();
}

//-----------------------------------------------------------------------------
inline
SimpleFormatterOutputSentry::SimpleFormatterOutputSentry( SimpleFormatter &simpleFormatter )
    : m_simpleFormatter(simpleFormatter)
{
}

//-----------------------------------------------------------------------------
inline
SimpleFormatterOutputSentry::~SimpleFormatterOutputSentry()
    {
        m_simpleFormatter.restoreFormatState();
    }

//-----------------------------------------------------------------------------








//-----------------------------------------------------------------------------
namespace omanip
{


//-----------------------------------------------------------------------------
inline
SimpleFormatter& flush( SimpleFormatter& fmt )
{
    SimpleFormatterOutputSentry sentry(fmt);
    fmt.flush();
    return fmt;
}

//-----------------------------------------------------------------------------
inline
SimpleFormatter& wflush( SimpleFormatter& fmt )
{
    SimpleFormatterOutputSentry sentry(fmt);
    fmt.waitFlushDone();
    return fmt;
}

//-----------------------------------------------------------------------------
inline
SimpleFormatter& endl( SimpleFormatter& fmt )
{
    SimpleFormatterOutputSentry sentry(fmt);
/*
    if (fmt.isTextMode())
    {
        // В текстовом режиме низлежащий поток вывода (char writter)
        // сам знает, как обработать перевод строки
        fmt<<'\n';
    }
    else
    {
        // В двоичном режиме простой вывод '\n' не транслируется в последовательность "\r\n"
        #if defined(UMBA_MCU_USED) || defined(_WIN32)
        fmt<<"\r\n"; // Под Windows или при использовании MCU манипулятор новой строки использует последовательность "\r\n"
        #else
        fmt<<"\n"; // Под Linux принято для перевода строки использовать только \n
        #endif
    }
*/
    fmt.coloring( ColoringLevel::normal );
    fmt.putEndl();
    fmt.flush();
    return fmt;
}

//-----------------------------------------------------------------------------
inline
SimpleFormatter& cret( SimpleFormatter& fmt )
{
    SimpleFormatterOutputSentry sentry(fmt);
    fmt.coloring( ColoringLevel::normal );
    fmt.putCR();
    fmt.flush();
    return fmt;
}

//-----------------------------------------------------------------------------
inline
SimpleFormatter& feed( SimpleFormatter& fmt )
{
    SimpleFormatterOutputSentry sentry(fmt);
    fmt.coloring( ColoringLevel::normal );
    fmt.putFF();
    fmt.flush();
    return fmt;
}



//-----------------------------------------------------------------------------
#define UMBA_SIMPLE_FORMATTER_IMPLEMENT_SET_FLAG_MANIP( manipName, manipFlag ) \
    inline                                                                     \
    SimpleFormatter& manipName( SimpleFormatter& fmt )                         \
    {                                                                          \
        fmt.setf( SimpleFormatter :: manipFlag );                              \
        return fmt;                                                            \
    }

#define UMBA_SIMPLE_FORMATTER_IMPLEMENT_UNSET_FLAG_MANIP( manipName, manipFlag ) \
    inline                                                                     \
    SimpleFormatter& manipName( SimpleFormatter& fmt )                         \
    {                                                                          \
        fmt.unsetf( SimpleFormatter :: manipFlag );                            \
        return fmt;                                                            \
    }

#define UMBA_SIMPLE_FORMATTER_IMPLEMENT_SET_UNSET_FLAG_MANIPS( manipName, manipFlag )  \
    UMBA_SIMPLE_FORMATTER_IMPLEMENT_SET_FLAG_MANIP( manipName      , manipFlag )       \
    UMBA_SIMPLE_FORMATTER_IMPLEMENT_UNSET_FLAG_MANIP( no##manipName, manipFlag )

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_IMPLEMENT_SET_UNSET_FLAG_MANIPS( boolalpha  , boolalpha )
UMBA_SIMPLE_FORMATTER_IMPLEMENT_SET_UNSET_FLAG_MANIPS( showbase   , showbase )
UMBA_SIMPLE_FORMATTER_IMPLEMENT_SET_UNSET_FLAG_MANIPS( showpoint  , showpoint )
UMBA_SIMPLE_FORMATTER_IMPLEMENT_SET_UNSET_FLAG_MANIPS( showpos    , showpos )
UMBA_SIMPLE_FORMATTER_IMPLEMENT_SET_UNSET_FLAG_MANIPS( uppercase  , uppercase )
UMBA_SIMPLE_FORMATTER_IMPLEMENT_SET_UNSET_FLAG_MANIPS( uppercasebase , uppercasebase )
UMBA_SIMPLE_FORMATTER_IMPLEMENT_SET_UNSET_FLAG_MANIPS( fmtauto    , fmtauto )
//UMBA_SIMPLE_FORMATTER_IMPLEMENT_SET_UNSET_FLAG_MANIPS(    ,  )



//-----------------------------------------------------------------------------
inline
SimpleFormatter& coloringColorImpl( SimpleFormatter& fmt, umba::term::colors::SgrColor clr )
{
    fmt.coloring( clr );
    return fmt;
}

//-----------------------------------------------------------------------------
inline
SgrColorManipHelper coloring( umba::term::colors::SgrColor clr )
{
    return SgrColorManipHelper( coloringColorImpl, clr );
}

//-----------------------------------------------------------------------------
inline
SgrColorManipHelper color( umba::term::colors::SgrColor clr )
{
    return SgrColorManipHelper( coloringColorImpl, clr );
}

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
inline
SimpleFormatter& coloringLevelImpl( SimpleFormatter& fmt, ColoringLevel lvl )
{
    fmt.coloring( lvl );
    return fmt;
}

//-----------------------------------------------------------------------------
inline
ColoringLevelManipHelper coloring( ColoringLevel lvl )
{
    return ColoringLevelManipHelper( coloringLevelImpl, lvl );
}

//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
inline
SimpleFormatter& emergency( SimpleFormatter& fmt )
{
    fmt.coloring( ColoringLevel::emergency );
    return fmt;
}

//-----------------------------------------------------------------------------
inline
SimpleFormatter& alert( SimpleFormatter& fmt )
{
    fmt.coloring( ColoringLevel::alert );
    return fmt;
}

//-----------------------------------------------------------------------------
inline
SimpleFormatter& critical( SimpleFormatter& fmt )
{
    fmt.coloring( ColoringLevel::critical );
    return fmt;
}

//-----------------------------------------------------------------------------
inline
SimpleFormatter& error( SimpleFormatter& fmt )
{
    fmt.coloring( ColoringLevel::error );
    return fmt;
}

//-----------------------------------------------------------------------------
inline
SimpleFormatter& warning( SimpleFormatter& fmt )
{
    fmt.coloring( ColoringLevel::warning );
    return fmt;
}

//-----------------------------------------------------------------------------
inline
SimpleFormatter& notice( SimpleFormatter& fmt )
{
    fmt.coloring( ColoringLevel::notice );
    return fmt;
}

//-----------------------------------------------------------------------------
inline
SimpleFormatter& info( SimpleFormatter& fmt )
{
    fmt.coloring( ColoringLevel::info );
    return fmt;
}

//-----------------------------------------------------------------------------
inline
SimpleFormatter& debug( SimpleFormatter& fmt )
{
    fmt.coloring( ColoringLevel::debug );
    return fmt;
}

//-----------------------------------------------------------------------------
inline
SimpleFormatter& good( SimpleFormatter& fmt )
{
    fmt.coloring( ColoringLevel::good );
    return fmt;
}

//-----------------------------------------------------------------------------
inline
SimpleFormatter& caption( SimpleFormatter& fmt )
{
    fmt.coloring( ColoringLevel::caption );
    return fmt;
}

//-----------------------------------------------------------------------------
inline
SimpleFormatter& normal( SimpleFormatter& fmt )
{
    fmt.coloring( ColoringLevel::normal );
    return fmt;
}

//-----------------------------------------------------------------------------
inline
SimpleFormatter& bin( SimpleFormatter& fmt )
{
    fmt.base( 2 );
    return fmt;
}

//-----------------------------------------------------------------------------
inline
SimpleFormatter& oct( SimpleFormatter& fmt )
{
    fmt.base( 8 );
    return fmt;
}

//-----------------------------------------------------------------------------
inline
SimpleFormatter& dec( SimpleFormatter& fmt )
{
    fmt.base( 10 );
    return fmt;
}

//-----------------------------------------------------------------------------
inline
SimpleFormatter& hex( SimpleFormatter& fmt )
{
    fmt.base( 16 );
    return fmt;
}

//-----------------------------------------------------------------------------
inline
SimpleFormatter& right( SimpleFormatter& fmt )
{
    fmt.setf( SimpleFormatter::right, SimpleFormatter::adjustfield );
    return fmt;
}

//-----------------------------------------------------------------------------
inline
SimpleFormatter& left( SimpleFormatter& fmt )
{
    fmt.setf( SimpleFormatter::left, SimpleFormatter::adjustfield );
    return fmt;
}

//-----------------------------------------------------------------------------
inline
SimpleFormatter& internal( SimpleFormatter& fmt )
{
    fmt.setf( SimpleFormatter::internal, SimpleFormatter::adjustfield );
    return fmt;
}

//-----------------------------------------------------------------------------
inline
SimpleFormatter& fixed( SimpleFormatter& fmt )
{
    fmt.setf( SimpleFormatter::fixed, SimpleFormatter::floatfield );
    return fmt;
}

//-----------------------------------------------------------------------------
inline
SimpleFormatter& scientific( SimpleFormatter& fmt )
{
    fmt.setf( SimpleFormatter::scientific, SimpleFormatter::floatfield );
    return fmt;
}

//-----------------------------------------------------------------------------
inline
SimpleFormatter& defaultfloat( SimpleFormatter& fmt )
{
    fmt.setf( 0, SimpleFormatter::floatfield );
    return fmt;
}

//-----------------------------------------------------------------------------
// make state given by manipulators persistent
inline
SimpleFormatter& persistent( SimpleFormatter& fmt )
{
    // get state updated by manipulators
    SimpleFormatter::FormatState curState = fmt.getState();

    // restore saved persistent state and clear "saved" flag
    fmt.restoreFormatState();

    fmt.setState( curState );

    return fmt;
}

//-----------------------------------------------------------------------------
#define UMBA_SIMPLE_FORMATTER_BEGIN_IMPLEMENT_INT_MANIP( manipName ) \
    inline                                                           \
    SimpleFormatter& manipName##Impl( SimpleFormatter& fmt, int i )


#define UMBA_SIMPLE_FORMATTER_END_IMPLEMENT_INT_MANIP( manipName )   \
                                                                     \
    inline                                                           \
    IntManipHelper manipName( int i )                                \
    {                                                                \
        return IntManipHelper( manipName##Impl, i );                 \
    }

#define UMBA_SIMPLE_FORMATTER_BEGIN_IMPLEMENT_INT2_MANIP( manipName ) \
    inline                                                           \
    SimpleFormatter& manipName##Impl( SimpleFormatter& fmt, int i1, int i2 )


#define UMBA_SIMPLE_FORMATTER_END_IMPLEMENT_INT2_MANIP( manipName )  \
                                                                     \
    inline                                                           \
    Int2ManipHelper manipName( int i1, int i2 )                      \
    {                                                                \
        return Int2ManipHelper( manipName##Impl, i1, i2 );           \
    }

//-----------------------------------------------------------------------------


UMBA_SIMPLE_FORMATTER_BEGIN_IMPLEMENT_INT_MANIP(base)
{
    fmt.base( i );
    return fmt;
}
UMBA_SIMPLE_FORMATTER_END_IMPLEMENT_INT_MANIP(base)

// same name as iostream manip
UMBA_SIMPLE_FORMATTER_BEGIN_IMPLEMENT_INT_MANIP(setbase)
{
    fmt.base( i );
    return fmt;
}
UMBA_SIMPLE_FORMATTER_END_IMPLEMENT_INT_MANIP(setbase)



UMBA_SIMPLE_FORMATTER_BEGIN_IMPLEMENT_INT_MANIP(fill)
{
    fmt.fill( (char)i );
    return fmt;
}
UMBA_SIMPLE_FORMATTER_END_IMPLEMENT_INT_MANIP(fill)

// same name as iostream manip
UMBA_SIMPLE_FORMATTER_BEGIN_IMPLEMENT_INT_MANIP(setfill)
{
    fmt.fill( (char)i );
    return fmt;
}
UMBA_SIMPLE_FORMATTER_END_IMPLEMENT_INT_MANIP(setfill)



UMBA_SIMPLE_FORMATTER_BEGIN_IMPLEMENT_INT_MANIP(precision)
{
    fmt.precision( i );
    return fmt;
}
UMBA_SIMPLE_FORMATTER_END_IMPLEMENT_INT_MANIP(precision)

// same name as iostream manip
UMBA_SIMPLE_FORMATTER_BEGIN_IMPLEMENT_INT_MANIP(setprecision)
{
    fmt.precision( i );
    return fmt;
}
UMBA_SIMPLE_FORMATTER_END_IMPLEMENT_INT_MANIP(setprecision)



UMBA_SIMPLE_FORMATTER_BEGIN_IMPLEMENT_INT_MANIP(width)
{
    fmt.width( i );
    return fmt;
}
UMBA_SIMPLE_FORMATTER_END_IMPLEMENT_INT_MANIP(width)

// same name as iostream manip
UMBA_SIMPLE_FORMATTER_BEGIN_IMPLEMENT_INT_MANIP(setw)
{
    fmt.width( i );
    return fmt;
}
UMBA_SIMPLE_FORMATTER_END_IMPLEMENT_INT_MANIP(setw)


UMBA_SIMPLE_FORMATTER_BEGIN_IMPLEMENT_INT_MANIP(groupsep)
{
    fmt.groupsep( i );
    return fmt;
}
UMBA_SIMPLE_FORMATTER_END_IMPLEMENT_INT_MANIP(groupsep)

UMBA_SIMPLE_FORMATTER_BEGIN_IMPLEMENT_INT_MANIP(decgroupsep)
{
    fmt.decgroupsep( i );
    return fmt;
}
UMBA_SIMPLE_FORMATTER_END_IMPLEMENT_INT_MANIP(decgroupsep)

UMBA_SIMPLE_FORMATTER_BEGIN_IMPLEMENT_INT_MANIP(groupsize)
{
    fmt.groupsize( i );
    return fmt;
}
UMBA_SIMPLE_FORMATTER_END_IMPLEMENT_INT_MANIP(groupsize)

UMBA_SIMPLE_FORMATTER_BEGIN_IMPLEMENT_INT_MANIP(decgroupsize)
{
    fmt.decgroupsize( i );
    return fmt;
}
UMBA_SIMPLE_FORMATTER_END_IMPLEMENT_INT_MANIP(decgroupsize)

UMBA_SIMPLE_FORMATTER_BEGIN_IMPLEMENT_INT2_MANIP(group)
{
    fmt.groupsize( i1 );
    fmt.groupsep( (char)i2 );
    return fmt;
}
UMBA_SIMPLE_FORMATTER_END_IMPLEMENT_INT2_MANIP(group)

UMBA_SIMPLE_FORMATTER_BEGIN_IMPLEMENT_INT2_MANIP(decgroup)
{
    fmt.decgroupsize( i1 );
    fmt.decgroupsep( (char)i2 );
    return fmt;
}
UMBA_SIMPLE_FORMATTER_END_IMPLEMENT_INT2_MANIP(decgroup)


UMBA_SIMPLE_FORMATTER_BEGIN_IMPLEMENT_INT_MANIP(decpoint)
{
    fmt.decpoint( (char)i );
    return fmt;
}
UMBA_SIMPLE_FORMATTER_END_IMPLEMENT_INT_MANIP(decpoint)




namespace term {


//-----------------------------------------------------------------------------
inline
SimpleFormatter& move2abs0( SimpleFormatter& fmt )
{
    fmt.getCharWritter()->terminalMove2Abs0();
    return fmt;
}

//-----------------------------------------------------------------------------
inline
SimpleFormatter& move2down( SimpleFormatter& fmt )
{
    fmt.getCharWritter()->terminalMove2Down();
    return fmt;
}

//-----------------------------------------------------------------------------
inline
SimpleFormatter& move2line0( SimpleFormatter& fmt )
{
    fmt.getCharWritter()->terminalMove2Line0();
    return fmt;
}

//-----------------------------------------------------------------------------
inline
SimpleFormatter& clear_screen( SimpleFormatter& fmt )
{
    fmt.getCharWritter()->terminalMove2Abs0();
    fmt.getCharWritter()->terminalClearRemaining( -1 ); // Очищаем весь экран, и переустанавливаем точку вывода в самое начало
    return fmt;
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_BEGIN_IMPLEMENT_INT_MANIP(clear) // Очищаем нужное количество строк, считая текущую. Текущую очищаем от текущей позиции. Если 0 - то ничего не очищаем
{
    if (i<0)
        fmt.getCharWritter()->terminalClearRemaining( -1 );
    else if (i==0)
        return fmt;
    else
        fmt.getCharWritter()->terminalClearRemaining( i-1 );
    
    return fmt;
}
UMBA_SIMPLE_FORMATTER_END_IMPLEMENT_INT_MANIP(clear)

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_BEGIN_IMPLEMENT_INT_MANIP(clear_line) // Очищаем N позиций в строке, начиная с текущей. Если <0 - очищаем до конца
{
    fmt.getCharWritter()->terminalClearLine( i );
    return fmt;
}
UMBA_SIMPLE_FORMATTER_END_IMPLEMENT_INT_MANIP(clear_line)

//-----------------------------------------------------------------------------
inline
SimpleFormatter& spinner_endl( SimpleFormatter& fmt )
{
    if (!fmt.getCharWritter()->isTerminal())
    {
        fmt << omanip::endl;
    }
    else
    {
        fmt.getCharWritter()->terminalSetSpinnerMode( false );
        fmt.getCharWritter()->terminalClearRemaining( 2 ); // Текущую и две строки ниже очищаем от возможного слишком длинного текста. Если больше - будет люто тормозить
        //fmt.getCharWritter()->terminalClearLineRemaining();
    }

    return fmt;

#if 0

    SimpleFormatterOutputSentry sentry(fmt);
/*
    if (fmt.isTextMode())
    {
        // В текстовом режиме низлежащий поток вывода (char writter)
        // сам знает, как обработать перевод строки
        fmt<<'\n';
    }
    else
    {
        // В двоичном режиме простой вывод '\n' не транслируется в последовательность "\r\n"
        #if defined(UMBA_MCU_USED) || defined(_WIN32)
        fmt<<"\r\n"; // Под Windows или при использовании MCU манипулятор новой строки использует последовательность "\r\n"
        #else
        fmt<<"\n"; // Под Linux принято для перевода строки использовать только \n
        #endif
    }
*/
    fmt.coloring( ColoringLevel::normal );
    fmt.putEndl();
    fmt.flush();
    return fmt;

#endif

}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_BEGIN_IMPLEMENT_INT_MANIP(move2lpos)
{
    fmt.getCharWritter()->terminalMove2LinePos(i);
    return fmt;
}
UMBA_SIMPLE_FORMATTER_END_IMPLEMENT_INT_MANIP(move2lpos)

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_BEGIN_IMPLEMENT_INT2_MANIP(move2pos)
{
    fmt.getCharWritter()->terminalMove2Pos(i1,i2);
    return fmt;
}
UMBA_SIMPLE_FORMATTER_END_IMPLEMENT_INT2_MANIP(move2pos)

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_BEGIN_IMPLEMENT_INT2_MANIP(spinner)
{
    static char spinnerChars[] = { '-', '\\', '|', '/' };

    if (!fmt.getCharWritter()->isTerminal())
         return fmt;


    char str[3] = { ' ', ' ', 0 };
    if (i1<0)
        i1 = -i1;

    str[0] = spinnerChars[i1%4];

    fmt.getCharWritter()->terminalSetSpinnerMode( true );
    fmt.getCharWritter()->terminalMove2LinePos(i2);
    fmt << (const char*)&str[0];

    return fmt;
}
UMBA_SIMPLE_FORMATTER_END_IMPLEMENT_INT2_MANIP(spinner)

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_BEGIN_IMPLEMENT_INT_MANIP(caret)
{
    fmt.getCharWritter()->terminalSetCaret(i);
    return fmt;
}
UMBA_SIMPLE_FORMATTER_END_IMPLEMENT_INT_MANIP(caret)



//! Установка каретки (курсора) консоли. 0 - выключена, 1 - полоска внизу, 2 - половина, 3 - во всю высоту

static const int caretNo        = 0;       // No caret
static const int caretDisable   = caretNo;
static const int caretHid       = caretNo; // Hidden
static const int caretMin       = 1;
static const int caretMid       = 2;
static const int caretMax       = 3;






} // namespace term


} // namespace omanip



} // namespace umba


#ifdef USE_LOUT

    extern umba::SimpleFormatter lout;

#endif

#if defined(UMBA_SIMPLE_FORMATTER_NO_HEADER_ONLY)
    #define UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
#else
    #define UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION   inline
    #include "../../simple_formatter.cpp"
#endif


