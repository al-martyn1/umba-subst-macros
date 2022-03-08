/*! 
    \file
    \brief Реализация RGB четвертяка. Хранит RGB и Alpha-канал компоненты цвета
 */
//-----------------------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------------------

#include "umba.h"
#include "format_utils.h"

#if !defined(UMBA_MCU_USED)
    #include "string_plus.h"
#endif


//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------

/*
Must be included before this file
#include <QColor>
#include <QBrush>
*/

#if !defined(UMBA_MCU_USED)
    #include <string>
#endif


#include "parse_utils.h"
#include "const_string.h"

#include <cstring>

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------

// https://doc.qt.io/qt-5/qcolorconstants.html#svg-colors
// https://www.w3.org/TR/SVG11/types.html#ColorKeywords

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
namespace umba
{




//-----------------------------------------------------------------------------
struct RgbQuad_NameColorPair;
const RgbQuad_NameColorPair* RgbQuad_NameColorPair_getColorPairs();

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
#include "pushpack1.h"
//! RGB четвертяк, хранит компоненты цвета
/*! Не привязываемся как какому-либо фреймворку, вполне может использоваться и на MCU.

    Иныалидный цвет - RGBA(1,1,1,1). Хотя, теоретически, цвет RGBA(1,1,1,1) - вполне валидный, но, так-то, 
    невозможно найти невалидное значения без расширения типа.
    Вероятность же, что в конфиге будет задан цвет RGBA(1,1,1,1) (почти черный, и с прозрачностью 1) - стремится к нулю.
    В других же местах (кроме обработки конфигов и какого-либо пользовательского ввода) - маркер инвалидности обычно не нужен.

    При конвертации в/из 16ти-битных величин  используем по 4 бита на цветовую компоненту, 12 бит на цвет (без альфа-канала), 
    итого - 4096 цветов. Для MCU, да и для других применений вполне может прокатывать (а сделано - просто потому, что было несложно 
    копипастой сделать пару методов).
 */
struct RgbQuad
{
    uint8_t      blue  ; //!< Синяя компонента цвета
    uint8_t      green ; //!< Зелёная компонента цвета
    uint8_t      red   ; //!< Красная компонента цвета
    uint8_t      alpha ; //!< Альфа компонента цвета - прозрачность

    static const std::size_t  max_name_buf_size = 32; //!< Размер буфера для форматирования в него цвета

    //! Конструктор по умолчанию. Конструирует инвалидный цвет. 
    RgbQuad()                                                 : blue (1), green(1), red  (1), alpha(1) { } // invalid value
    //! Конструктор из цветовых компонент
    RgbQuad( uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0 ) : blue (b), green(g), red  (r), alpha(a) { }
    //! Конструктор копирования
    RgbQuad( const RgbQuad &q )                               : blue (q.blue ) , green(q.green) , red  (q.red  ) , alpha(q.alpha) { }
    //! Конструктор из 32х-битного беззнакового целого
    RgbQuad( uint32_t argb, bool useAlpha = false )           : blue (), green(), red  (), alpha()     { fromUnsigned(argb, useAlpha); }
    //! Конструктор из 16ти-битного беззнакового целого
    RgbQuad( uint16_t argb, bool useAlpha = false )           : blue (), green(), red  (), alpha()     { fromUnsigned(argb, useAlpha); }
    //! Конструктор из 32х-битного знакового целого
    RgbQuad(  int32_t argb, bool useAlpha = false )           : blue (), green(), red  (), alpha()     { fromUnsigned((uint32_t)argb, useAlpha); }
    //! Конструктор из 16ти-битного знакового целого
    RgbQuad(  int16_t argb, bool useAlpha = false )           : blue (), green(), red  (), alpha()     { fromUnsigned((uint16_t)argb, useAlpha); }

    //! Оператор сравнения на равенство
    bool operator==( const RgbQuad rq ) const
    {
        return blue  == rq.blue
            && green == rq.green
            && red   == rq.red  
            && alpha == rq.alpha
            ;
    }

    //! Оператор сравнения на неравенство
    bool operator!=( const RgbQuad rq ) const
    {
        return blue  != rq.blue
            || green != rq.green
            || red   != rq.red  
            || alpha != rq.alpha
            ;
    }

    //! Возвращает инвалидный цвет
    static
    RgbQuad getInvalidValue()
    {
        //return RgbQuad(); // default constructor can be changed
        return RgbQuad( 1,1,1,1 );
    }

    //! Парсит 32х-битное беззнаковое целое
    void fromUnsigned( uint32_t argb /* clr */ , bool useAlpha = false )
    {
        blue  = (argb    ) & 0xFFu;
        green = (argb>>8 ) & 0xFFu;
        red   = (argb>>16) & 0xFFu;
        if (useAlpha)
           alpha = (argb>>24) & 0xFFu;
        else
           alpha = 0x00u;
    }

    //! Парсит 16ти-битное беззнаковое целое
    void fromUnsigned( uint16_t argb /* clr */ , bool useAlpha = false )
    {
        blue  = ((argb    ) & 0xFu)<<4;
        green = ((argb>>4 ) & 0xFu)<<4;
        red   = ((argb>>8 ) & 0xFu)<<4;
        if (useAlpha)
           alpha = ((argb>>12) & 0xFu)<<4;
        else
           alpha = 0x00u;
    }

    //! Парсит 32х-битное знаковое целое
    void fromInt( int32_t argb /* clr */ , bool useAlpha = false )
    {
        fromUnsigned( (uint32_t)argb, useAlpha );
    }

    //! Парсит 16ти-битное знаковое целое
    void fromInt( int16_t argb /* clr */ , bool useAlpha = false )
    {
        fromUnsigned( (uint16_t)argb, useAlpha );
    }

    //! Парсит 32х-битное беззнаковое целое - для универсальности имени fromInt
    void fromInt( uint32_t argb /* clr */ , bool useAlpha = false )
    {
        fromUnsigned( argb, useAlpha );
    }

    //! Парсит 16ти-битное беззнаковое целое - для универсальности имени fromInt
    void fromInt( uint16_t argb /* clr */ , bool useAlpha = false )
    {
        fromUnsigned( (uint16_t)argb, useAlpha );
    }

    //! Конвертирует в 32х-битное беззнаковое целое - оставлено для совместимости со старым кодом
    uint32_t toUnsigned() const
    {
        return
        (((uint32_t)blue )    ) |
        (((uint32_t)green)<< 8) |
        (((uint32_t)red  )<<16) |
        (((uint32_t)alpha)<<24) ;
    }

    //! Конвертирует в 32х-битное беззнаковое целое
    uint32_t toUnsigned32() const
    {
        return toUnsigned();
    }

    //! Конвертирует в 16ти-битное беззнаковое целое
    uint16_t toUnsigned16() const
    {
        return
        (((uint16_t)((blue >>4)&0x0F))    ) |
        (((uint16_t)((green>>4)&0x0F))<< 4) |
        (((uint16_t)((red  >>4)&0x0F))<< 8) |
        (((uint16_t)((alpha>>4)&0x0F))<<12) ;
    }

    //! Конвертирует в 32х-битное знаковое целое
    int32_t toInt32() const
    {
        return (int32_t)toUnsigned32();
    }

    //! Конвертирует в 16ти-битное знаковое целое
    int16_t toInt6() const
    {
        return (int16_t)toUnsigned16();
    }



public:

    //! Конвертирует из строки
    /*! Строковое представление может быть в 16тиричном в формате вида \#AARRGGBB, или именем цвета из предопределённого набора
     */
    bool fromString( const char* str );

    //! Конвертирует цвет в 16тиричное строковое представление
    /*! 
        В буфере nameBuf должно быть достаточно места для имени или шестнадцатиричного представления. 
        Самое длинное имя цвета - "lightgoldenrodyellow" - длина 20 символов.
        Длинна буфера в 32 символа достаточна всегда.

        Если noAlpha==true, то при преобразовании альфаканал отбрасывается.
     */
    void toString( char* nameBuf       //!< Указатель на буфер
                 , bool useAlpha = false //!< Игнорировать альфа канал
                 ) const;


    #if !defined(UMBA_MCU_USED)

    //! Конвертирует из строки std::string
    bool fromString( const std::string &s )
    {
        return fromString(s.c_str());
    }

    //! Конвертирует из строки std::wstring
    bool fromString( const std::wstring &s )
    {
        return fromString(umba::string_plus::make_string<std::string>( s ));
    }

    //! Конвертирует в строку
    template < typename StringType >
    StringType toString( bool useAlpha = false /*!< Игнорировать альфа канал */ ) const
    {
        char buf[ max_name_buf_size ];
        toString( &buf[0] );
        return umba::string_plus::make_string<std::string>( &buf[0] );
    }

    #endif


    #if defined(QCOLOR_H) 
    /* Qt guard name */

    //! Преобразование в QColor - совместимость с Qt
    operator QColor() const
    {
        if (!alpha) // ����� - ������ �� ������. ���� �� ��������� ����������, � ������ ����� � ������ - �� ���� �� ����� ���� ������ ������������
            return QColor( (int)(unsigned)red, (int)(unsigned)green, (int)(unsigned)blue );
        else
            return QColor( (int)(unsigned)red, (int)(unsigned)green, (int)(unsigned)blue, (int)(unsigned)alpha );
    }

    #endif


    #if defined(QBRUSH_H) /* Qt guard name */

    //! Преобразование в QBrush - совместимость с Qt и упрощение использования для рисования
    operator QBrush() const
    {
        return QBrush( QColor(*this) ); // Qt::SolidPattern as second args
    }

    #endif



}; // struct RgbQuad
#include "packpop.h"

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
//! Хранит связку имя - значение цвета
struct RgbQuad_NameColorPair
{
    const char*    name;  //!< Имя цвета
    RgbQuad        color; //!< Цвет
};

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
//! Возвращает таблицу предопределённых имён цветов - спёрто вроде из SVG стандарта, но имхо совместимо и с HTML
inline
const RgbQuad_NameColorPair* RgbQuad_NameColorPair_getColorPairs()
{
    static
    RgbQuad_NameColorPair
    names[] = 
    { { "black"      , 0x000000u }
    , { "white"      , 0xffffffu }
    , { "darkgray"   , 0x606060u }
    , { "darkgrey"   , 0x606060u }
    , { "gray"       , 0xa0a0a0u }
    , { "grey"       , 0xa0a0a0u }
    , { "lightgray"  , 0xc0c0c0u }
    , { "lightgrey"  , 0xc0c0c0u }
    , { "red"        , 0xff0000u }
    , { "green"      , 0x00ff00u }
    , { "blue"       , 0x0000ffu }
    , { "cyan"       , 0x00ffffu }
    , { "magenta"    , 0xff00ffu }
    , { "yellow"     , 0xffff00u }
    , { "darkred"    , 0x800000u }
    , { "darkgreen"  , 0x008000u }
    , { "darkblue"   , 0x000080u }
    , { "darkcyan"   , 0x008080u }
    , { "darkmagenta", 0x800080u }
    , { "darkyellow" , 0x808000u }

    // Advanced color names (not for MCU)
    #if !defined(UMBA_MCU_USED) || defined(UMBA_MCU_ADVANCED_RGBQUAD_NAMES)
    , { "dimgray"    , 0x909090u }
    , { "dimgrey"    , 0x909090u }
    , { "aliceblue"            , { 240, 248, 255 } }
    , { "antiquewhite"         , { 250, 235, 215 } }
    , { "aqua"                 , {   0, 255, 255 } }
    , { "aquamarine"           , { 127, 255, 212 } }
    , { "azure"                , { 240, 255, 255 } }
    , { "beige"                , { 245, 245, 220 } }
    , { "bisque"               , { 255, 228, 196 } }
    , { "blanchedalmond"       , { 255, 235, 205 } }
    , { "blue"                 , {   0,   0, 255 } }
    , { "blueviolet"           , { 138,  43, 226 } }
    , { "brown"                , { 165,  42,  42 } }
    , { "burlywood"            , { 222, 184, 135 } }
    , { "cadetblue"            , {  95, 158, 160 } }
    , { "chartreuse"           , { 127, 255,   0 } }
    , { "chocolate"            , { 210, 105,  30 } }
    , { "coral"                , { 255, 127,  80 } }
    , { "cornflowerblue"       , { 100, 149, 237 } }
    , { "cornsilk"             , { 255, 248, 220 } }
    , { "crimson"              , { 220,  20,  60 } }
    , { "darkblue"             , {   0,   0, 139 } }
    , { "darkcyan"             , {   0, 139, 139 } }
    , { "darkgoldenrod"        , { 184, 134,  11 } }
    , { "darkgreen"            , {   0, 100,   0 } }
    , { "darkgrey"             , { 169, 169, 169 } }
    , { "darkkhaki"            , { 189, 183, 107 } }
    , { "darkmagenta"          , { 139,   0, 139 } }
    , { "darkolivegreen"       , {  85, 107,  47 } }
    , { "darkorange"           , { 255, 140,   0 } }
    , { "darkorchid"           , { 153,  50, 204 } }
    , { "darkred"              , { 139,   0,   0 } }
    , { "darksalmon"           , { 233, 150, 122 } }
    , { "darkseagreen"         , { 143, 188, 143 } }
    , { "darkslateblue"        , {  72,  61, 139 } }
    , { "darkslategray"        , {  47,  79,  79 } }
    , { "darkslategrey"        , {  47,  79,  79 } }
    , { "darkturquoise"        , {   0, 206, 209 } }
    , { "darkviolet"           , { 148,   0, 211 } }
    , { "deeppink"             , { 255,  20, 147 } }
    , { "deepskyblue"          , {   0, 191, 255 } }
    , { "dodgerblue"           , {  30, 144, 255 } }
    , { "firebrick"            , { 178,  34,  34 } }
    , { "floralwhite"          , { 255, 250, 240 } }
    , { "forestgreen"          , {  34, 139,  34 } }
    , { "fuchsia"              , { 255,   0, 255 } }
    , { "gainsboro"            , { 220, 220, 220 } }
    , { "ghostwhite"           , { 248, 248, 255 } }
    , { "gold"                 , { 255, 215,   0 } }
    , { "goldenrod"            , { 218, 165,  32 } }
    , { "greenyellow"          , { 173, 255,  47 } }
    , { "honeydew"             , { 240, 255, 240 } }
    , { "hotpink"              , { 255, 105, 180 } }
    , { "indianred"            , { 205,  92,  92 } }
    , { "indigo"               , {  75,   0, 130 } }
    , { "ivory"                , { 255, 255, 240 } }
    , { "khaki"                , { 240, 230, 140 } }
    , { "lavender"             , { 230, 230, 250 } }
    , { "lavenderblush"        , { 255, 240, 245 } }
    , { "lawngreen"            , { 124, 252,   0 } }
    , { "lemonchiffon"         , { 255, 250, 205 } }
    , { "lightblue"            , { 173, 216, 230 } }
    , { "lightcoral"           , { 240, 128, 128 } }
    , { "lightcyan"            , { 224, 255, 255 } }
    , { "lightgoldenrodyellow" , { 250, 250, 210 } }
    , { "lightgreen"           , { 144, 238, 144 } }
    , { "lightpink"            , { 255, 182, 193 } }
    , { "lightsalmon"          , { 255, 160, 122 } }
    , { "lightseagreen"        , {  32, 178, 170 } }
    , { "lightskyblue"         , { 135, 206, 250 } }
    , { "lightslategray"       , { 119, 136, 153 } }
    , { "lightslategrey"       , { 119, 136, 153 } }
    , { "lightsteelblue"       , { 176, 196, 222 } }
    , { "lightyellow"          , { 255, 255, 224 } }
    , { "lime"                 , {   0, 255,   0 } }
    , { "limegreen"            , {  50, 205,  50 } }
    , { "linen"                , { 250, 240, 230 } }
    , { "maroon"               , { 128,   0,   0 } }
    , { "mediumaquamarine"     , { 102, 205, 170 } }
    , { "mediumblue"           , {   0,   0, 205 } }
    , { "mediumorchid"         , { 186,  85, 211 } }
    , { "mediumpurple"         , { 147, 112, 219 } }
    , { "mediumseagreen"       , {  60, 179, 113 } }
    , { "mediumslateblue"      , { 123, 104, 238 } }
    , { "mediumspringgreen"    , {   0, 250, 154 } }
    , { "mediumturquoise"      , {  72, 209, 204 } }
    , { "mediumvioletred"      , { 199,  21, 133 } }
    , { "midnightblue"         , {  25,  25, 112 } }
    , { "mintcream"            , { 245, 255, 250 } }
    , { "mistyrose"            , { 255, 228, 225 } }
    , { "moccasin"             , { 255, 228, 181 } }
    , { "navajowhite"          , { 255, 222, 173 } }
    , { "navy"                 , {   0,   0, 128 } }
    , { "oldlace"              , { 253, 245, 230 } }
    , { "olive"                , { 128, 128,   0 } }
    , { "olivedrab"            , { 107, 142,  35 } }
    , { "orange"               , { 255, 165,   0 } }
    , { "orangered"            , { 255,  69,   0 } }
    , { "orchid"               , { 218, 112, 214 } }
    , { "palegoldenrod"        , { 238, 232, 170 } }
    , { "palegreen"            , { 152, 251, 152 } }
    , { "paleturquoise"        , { 175, 238, 238 } }
    , { "palevioletred"        , { 219, 112, 147 } }
    , { "papayawhip"           , { 255, 239, 213 } }
    , { "peachpuff"            , { 255, 218, 185 } }
    , { "peru"                 , { 205, 133,  63 } }
    , { "pink"                 , { 255, 192, 203 } }
    , { "plum"                 , { 221, 160, 221 } }
    , { "powderblue"           , { 176, 224, 230 } }
    , { "purple"               , { 128,   0, 128 } }
    , { "red"                  , { 255,   0,   0 } }
    , { "rosybrown"            , { 188, 143, 143 } }
    , { "royalblue"            , {  65, 105, 225 } }
    , { "saddlebrown"          , { 139,  69,  19 } }
    , { "salmon"               , { 250, 128, 114 } }
    , { "sandybrown"           , { 244, 164,  96 } }
    , { "seagreen"             , {  46, 139,  87 } }
    , { "seashell"             , { 255, 245, 238 } }
    , { "sienna"               , { 160,  82,  45 } }
    , { "silver"               , { 192, 192, 192 } }
    , { "skyblue"              , { 135, 206, 235 } }
    , { "slateblue"            , { 106,  90, 205 } }
    , { "slategray"            , { 112, 128, 144 } }
    , { "slategrey"            , { 112, 128, 144 } }
    , { "snow"                 , { 255, 250, 250 } }
    , { "springgreen"          , {   0, 255, 127 } }
    , { "steelblue"            , {  70, 130, 180 } }
    , { "tan"                  , { 210, 180, 140 } }
    , { "teal"                 , {   0, 128, 128 } }
    , { "thistle"              , { 216, 191, 216 } }
    , { "tomato"               , { 255,  99,  71 } }
    , { "turquoise"            , {  64, 224, 208 } }
    , { "violet"               , { 238, 130, 238 } }
    , { "wheat"                , { 245, 222, 179 } }
    , { "whitesmoke"           , { 245, 245, 245 } }
    , { "yellowgreen"          , { 154, 205,  50 } }
    #endif
    , { 0                      , 0 }
    };

    return &names[0];
}

//-----------------------------------------------------------------------------
inline
bool RgbQuad::fromString( const char* str )
{
    if (*str=='#')
    {
        ++str;
        size_t readedDigits = 0;
        uint32_t argb = 0;
        for(; *str; ++str)
        {
            int d = umba::parse_utils::toDigit(*str);
            if (d<0)
                return false;
            argb <<= 16;
            argb += (uint32_t)d;
            ++readedDigits;
        }

        if (readedDigits==8 || readedDigits==6)
        {
            blue  = (argb    ) & 0xFFu;
            green = (argb>>8 ) & 0xFFu;
            red   = (argb>>16) & 0xFFu;
            if (readedDigits==8)
               alpha = (argb>>24) & 0xFFu;
            else
               alpha = 0xFFu;
        }
        else if (readedDigits==4 || readedDigits==3)
        {
            blue  = ((argb    ) & 0xFu)<<4;
            green = ((argb>>4 ) & 0xFu)<<4;
            red   = ((argb>>8 ) & 0xFu)<<4;
            if (readedDigits==4)
               alpha = ((argb>>12) & 0xFu)<<4;
            else
               alpha = 0xFFu;
        }
        else
        {
            return false;
        }

        return true;
    }
    else // name taken
    {
        const RgbQuad_NameColorPair* pColorPair = RgbQuad_NameColorPair_getColorPairs();
        for( ; pColorPair->name; ++pColorPair)
        {
            if (umba::platform_support::str_icase_compare(pColorPair->name, str)==0)
            {
                *this = pColorPair->color;
                return true;
            }
        }
    }

    return false;

}

//-----------------------------------------------------------------------------
inline
void RgbQuad::toString( char* nameBuf, bool useAlpha ) const
{
    RgbQuad cmp = *this;
    if (!useAlpha)
        cmp.alpha = 0;

    const RgbQuad_NameColorPair* pColorPair = RgbQuad_NameColorPair_getColorPairs();
    for( ; pColorPair->name; ++pColorPair)
    {
        if (pColorPair->color == cmp)
        {
            std::strcpy( nameBuf, pColorPair->name );
            return;
        }
    }

    // Appropriate color not found
    // Generate hex representation

    uint32_t uclr = toUnsigned();
    *nameBuf++ = '#';

    char* numericStart = nameBuf;

    // #AARRGGBB
    for( auto i = 0u; i!=8; ++i) // eight digits in 32 bit number
    {
        //  i         0    1    2    3    4    5    6    7
        //  i+1       1    2    3    4    5    6    7    8
        // (i+1)*4    4    8   12   16   20   24   28   32
        // (i+1)*4 = XM
        // XM         4    8   12   16   20   24   28   32
        // need (N)  28   24   20   16   12    8    4    0

        // N = 32-XM (nShift)

        // nShift = N

        auto XM = (i+1)*4;
        auto  N = 32 - XM;

        auto tmp = uclr;
        if (N)
            tmp <<= N;
        tmp &= 0x0F;

        *nameBuf++ = umba::format_utils::digitToChar( tmp );

    }

    *nameBuf = 0;

    if ((uclr & 0xFF000000) == 0)
    {
        std::memmove( (void*)numericStart, (void*)(numericStart+2), 6+1 );
    }

}

//-----------------------------------------------------------------------------




} // namespace umba


