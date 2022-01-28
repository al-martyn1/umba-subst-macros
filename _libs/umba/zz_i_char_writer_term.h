#pragma once

#if !defined(UMBA_I_CHAR_WRITER_H)
    #error "zz_i_char_writer_term.h must be included from i_char_writer.h only"
#endif


#define UMBA_X_I_CHAR_WRITER_TERM_H

//-----------------------------------------------------------------------------

// https://ru.wikipedia.org/wiki/%D0%A3%D0%BF%D1%80%D0%B0%D0%B2%D0%BB%D1%8F%D1%8E%D1%89%D0%B8%D0%B5_%D0%BF%D0%BE%D1%81%D0%BB%D0%B5%D0%B4%D0%BE%D0%B2%D0%B0%D1%82%D0%B5%D0%BB%D1%8C%D0%BD%D0%BE%D1%81%D1%82%D0%B8_ANSI

//! Определения, относящиеся к терминалам
/*!
 */
namespace term 
{


#include "zz_i_char_writer_term_console_type.h"
#include "zz_i_char_writer_term_colors.h"
#include "zz_i_char_writer_term_utils.h"


//-----------------------------------------------------------------------------





// 0 Reset/Normal
// 1 Жирный или увеличить яркость
// 5 Мигание: Медленно
// 7 Отображение: Негатив
// 25 Мигание: выкл.

// 30–37 Установить цвет текста (на переднем плане)
// 40–47 Установить цвет фона

//! Формирует управляющую Escape-последовательность для установки цвета в ANSI-терминале
inline
char* formatAnsiTermColor( char* pBuf, colors::SgrColor clr, bool fastBlink = false)
{
    //#define UMBA_TERM_DEBUG_TERM_ESC_CSI

    clr &= ~colors::full_mask;

    #if defined(UMBA_TERM_DEBUG_TERM_ESC_CSI)
    *pBuf++ = '\\';
    *pBuf++ = 'e';
    #else
    *pBuf++ = '\x1B';
    #endif
    *pBuf++ = '[';
    *pBuf++ = '0'; // reset/default colors  - 3 bytes written

    colors::SgrColor clrFg = 0;
    colors::SgrColor clrBg = 0;
    bool fBright   = false;
    bool fInvert   = false;
    bool fBlink    = false;

    colors::splitToOptions( clr, clrFg, clrBg, fBright, fInvert, fBlink );

    if ((clrFg&colors::color_default)==0) // +3 bytes
    {
        *pBuf++ = ';';
        pBuf = utils::formatUnsignedDec( pBuf, 30+clrFg);
    }

    if ((clrBg&colors::color_default)==0) // +3 bytes
    {
        *pBuf++ = ';';
        pBuf = utils::formatUnsignedDec( pBuf, 40+clrBg);
    }

    if (fBright) // +2 bytes
    {
        *pBuf++ = ';';
        *pBuf++ = '1';
    }

    if (fInvert) // +2 bytes
    {
        *pBuf++ = ';';
        *pBuf++ = '7';
    }

    if (fBlink) // +2 bytes
    {
        *pBuf++ = ';';
        if (fastBlink)
            *pBuf++ = '6';
        else
            *pBuf++ = '5';
    }

    *pBuf++ = 'm'; // +1
    #if defined(UMBA_TERM_DEBUG_TERM_ESC_CSI)
    *pBuf++ = ' '; // +1
    #endif
    *pBuf++ = 0;

    return pBuf; // max - 16 bytes len
}


#if defined(WIN32) || defined(_WIN32)

//! Подсистема Win32 для работы с терминалом (с консолью Windows)
namespace win32
{

/*
   WORD getConsoleTextAttribute( HANDLE hCon )
   WORD setConsoleTextAttribute( HANDLE hCon, WORD newAttr )
   colors::SgrColor convertColorFromConsole( WORD conColor )
   colors::SgrColor convertColorFromConsole( WORD conColor )
   WORD convertColorToConsole( colors::SgrColor clr )
*/


   //! Получение атрибутов текста для хэндла консоли
   inline
   WORD getConsoleTextAttribute( HANDLE hCon )
   {
       CONSOLE_SCREEN_BUFFER_INFO csb;
       ::GetConsoleScreenBufferInfo(hCon,&csb);
       return csb.wAttributes;
   }

   //! Установка атрибутов текста для хэндла консоли
   inline
   WORD setConsoleTextAttribute( HANDLE hCon, WORD newAttr )
   {
       CONSOLE_SCREEN_BUFFER_INFO csb;
       ::GetConsoleScreenBufferInfo(hCon,&csb);
       ::SetConsoleTextAttribute( hCon, newAttr );
       return csb.wAttributes;
   }

   //! Хелпер преобразования Win32-атрибута текста консоли в colors::SgrColor
   inline
   colors::SgrColor convertColorFromConsoleHelper( WORD conColor )
   {
       colors::SgrColor clr = 0;

       if (conColor&FOREGROUND_INTENSITY)
       {
           conColor &= ~FOREGROUND_INTENSITY;
           clr = colors::bright;
       }

       switch(conColor)
       {
           case 0                                                   : return clr | colors::black;
           case FOREGROUND_BLUE                                     : return clr | colors::blue;
           case FOREGROUND_GREEN                                    : return clr | colors::green;
           case FOREGROUND_RED                                      : return clr | colors::red;
           case (FOREGROUND_GREEN|FOREGROUND_RED  )                 : return clr | colors::yellow;
           case (FOREGROUND_BLUE |FOREGROUND_RED  )                 : return clr | colors::magenta;
           case (FOREGROUND_GREEN|FOREGROUND_BLUE )                 : return clr | colors::cyan;
           case (FOREGROUND_RED  |FOREGROUND_GREEN|FOREGROUND_BLUE) : return clr | colors::white;
           default: return clr | colors::black;
       }

   }

   //! Преобразование Win32-атрибута текста консоли в colors::SgrColor 
   inline
   colors::SgrColor convertColorFromConsole( WORD conColor )
   {
       return convertColorFromConsoleHelper( conColor&0x0F )
            | convertColorFromConsoleHelper( (conColor>>4)&0x0F );
   }

   //! Хелпер преобразования цвета colors::SgrColor в Win32-атрибут текста консоли
   inline
   WORD convertColorToConsoleHelper( colors::SgrColor clr, WORD defAttr )
   {

       switch(clr&colors::color_mask)
          {
           case colors::red    : return FOREGROUND_RED;
           case colors::green  : return FOREGROUND_GREEN;
           case colors::yellow : return FOREGROUND_GREEN|FOREGROUND_RED;
           case colors::blue   : return FOREGROUND_BLUE;
           case colors::magenta: return FOREGROUND_RED|FOREGROUND_BLUE;
           case colors::cyan   : return FOREGROUND_GREEN|FOREGROUND_BLUE;
           case colors::white  : return FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE;
           case colors::color_default: return defAttr & 0x0F;
           default: return 0;
          }
   }

   //! Преобразование цвета colors::SgrColor в Win32-атрибут текста консоли
   inline
   WORD convertColorToConsole( colors::SgrColor clr, WORD defAttr )
   {
       colors::SgrColor fgClr = clr      & colors::color_mask;
       colors::SgrColor bgClr = (clr>>8) & colors::color_mask;

       WORD fg = convertColorToConsoleHelper(fgClr, defAttr);
       if (fgClr!=colors::color_default)
       {
           if (clr&colors::bright)
               fg |= FOREGROUND_INTENSITY;
       }

       WORD bg = convertColorToConsoleHelper(bgClr, defAttr>>4);
       if (bgClr!=colors::color_default)
       {
           if (clr&colors::bright)
               bg |= FOREGROUND_INTENSITY;
       }

       if (clr&colors::invert)
           std::swap(fg,bg);

       return fg | (bg<<4); //  | bright;
   }



} // namespace win32    

#endif

} // namespace term
