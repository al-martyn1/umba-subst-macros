#pragma once

#if !defined(UMBA_I_CHAR_WRITER_H)
    #error "zz_i_char_writer_term_colors.h must be included from zz_i_char_writer_term.h only"
#endif


//! Определения для цветов в терминале
/*! Константы цвета задаются для текста (foreground).
    Для получения значений цвета для фона их надо сдвинуть на 8 бит влево.
    Яркость, мерцание и инверсия задаются для знакоместа
 */
namespace colors 
{

    //------------------------------

    //! Производит сборку атрибута цвета для знакоместа
    #define UMBA_TERM_COLORS_MAKE_COMPOSITE( fgColor, bgColor, fBright, fInvert, fBlink )     \
                      ( ((fgColor)  & umba::term::colors::color_mask)                         \
                      | (((bgColor) & umba::term::colors::color_mask)<<8)                     \
                      | ((fBright)  ? umba::term::colors::bright : 0u /* umba::term::colors::black */ ) \
                      | ((fInvert)  ? umba::term::colors::invert : 0u /* umba::term::colors::black */ ) \
                      | ((fBlink)   ? umba::term::colors::blink  : 0u /* umba::term::colors::black */ ) \
                      )


    //------------------------------

    typedef unsigned SgrColor; //!< Select Graphic Rendition (SGR) terminal parameters - Color - used for terminal output

    // foreground
    static const unsigned black          = 0x00000000;  //!< Цвет (текста) - черный
    static const unsigned red            = 0x00000001;  //!< Цвет (текста) - красный
    static const unsigned green          = 0x00000002;  //!< Цвет (текста) - зелёный
    static const unsigned yellow         = 0x00000003;  //!< Цвет (текста) - желтый
    static const unsigned blue           = 0x00000004;  //!< Цвет (текста) - синий
    static const unsigned magenta        = 0x00000005;  //!< Цвет (текста) - магента
    static const unsigned cyan           = 0x00000006;  //!< Цвет (текста) - циан
    static const unsigned white          = 0x00000007;  //!< Цвет (текста) - белый

    static const unsigned raw_mask       = 0x00000007;  //!< Какая-то сырая маска
    static const unsigned color_default  = 0x00000008;  //!< Использовать цвет по умолчанию
    static const unsigned color_mask     = 0x0000000F;  //!< Маска цвета текста

    // background colors can be obtained as FG << 8

    static const unsigned bright         = 0x00010000;  //!< Признак яркости
    static const unsigned blink          = 0x00020000;  //!< Признак мерцания
    static const unsigned invert         = 0x00040000;  //!< Признак инверсии

    static const unsigned full_mask      = 0x0007FFFF;  //!< Признак инверсии


    //------------------------------
    //! Производит сборку атрибута цвета для знакоместа
    inline
    SgrColor makeComposite( SgrColor fgColor, SgrColor bgColor, bool fBright = false, bool fInvert = false, bool fBlink = false )
    {
        return UMBA_TERM_COLORS_MAKE_COMPOSITE(fgColor, bgColor, fBright, fInvert, fBlink);
        // (fgColor & color_mask) | ((bgColor & color_mask)<<8) | (fBright?bright:black) | (fBlink?blink:black);
    }

    //------------------------------
    //! Производит разборку атрибута цвета для знакоместа на составляющие
    inline
    void splitToOptions( SgrColor clrComposite, SgrColor &clrFg, SgrColor &clrBg, bool &fBright, bool &fInvert, bool &fBlink )
    {
        clrFg = clrComposite & color_mask;
        clrBg = (clrComposite>>8) & color_mask;

        fBright = (clrComposite & bright)  ? true : false;
        fBlink  = (clrComposite & blink )  ? true : false;
        fInvert = (clrComposite & invert ) ? true : false;
    }

} // namespace colors




