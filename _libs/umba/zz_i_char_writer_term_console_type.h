#pragma once

#if !defined(UMBA_I_CHAR_WRITER_H)
    #error "zz_i_char_writer_term_console_type.h must be included from zz_i_char_writer_term.h only"
#endif


#ifdef UMBA_CXX_HAS_STD11

    
    //! Типы консолей/терминалов
    enum class ConsoleType
    {
        file,              //!< Обычный файл, не терминал
        windows_console,   //!< Виндовая консоль
        ansi_terminal      //!< ANSI-терминал
    };

    //! Обычный файл, не терминал
    #define UMBA_CONSOLETYPE_FILE               ConsoleType::file
    //! Виндовая консоль
    #define UMBA_CONSOLETYPE_WINDOWS_CONSOLE    ConsoleType::windows_console
    //! ANSI-терминал
    #define UMBA_CONSOLETYPE_ANSI_TERMINAL      ConsoleType::ansi_terminal

#else

    //! Типы консолей/терминалов
    enum ConsoleType
    {
        ConsoleType_file,              //!< Обычный файл, не терминал
        ConsoleType_windows_console,   //!< Виндовая консоль         
        ConsoleType_ansi_terminal      //!< ANSI-терминал            
    };

    //! Обычный файл, не терминал
    #define UMBA_CONSOLETYPE_FILE               ConsoleType_file
    //! Виндовая консоль
    #define UMBA_CONSOLETYPE_WINDOWS_CONSOLE    ConsoleType_windows_console
    //! ANSI-терминал
    #define UMBA_CONSOLETYPE_ANSI_TERMINAL      ConsoleType_ansi_terminal

#endif



