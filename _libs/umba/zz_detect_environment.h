#pragma once

//----------------------------------------------------------------------------
/*!
\file
\ingroup UMBA_LIBS_TUNING_MACROS
\brief Определение сборочного окружения и целевой платформы.

Для отключения всех устаревших возможностей следует задать макрос
#UMBA_DISABLE_DEPRECATED.

Для отключения всех отладочных проверок (полный релиз) следует задать макрос
#UMBA_NDEBUG.

Препроцессор:
- #UMBA_PP_HAS_COUNTER / #UMBA_PP_NO_COUNTER - есть или нет макрос \b `__COUNTER__`.

В зависимости от версии языка C++ определяются макросы: 
- #UMBA_CXX_HAS_STD03,
- #UMBA_CXX_HAS_STD11,
- #UMBA_CXX_HAS_STD14,
- #UMBA_CXX_HAS_STD17.

\section UMBA_X_DETECT_ENVIRONMENT_USAGE Настройка проекта на целевую платформу

Существует два способа использования: 1) для проекта задаются predefined макросы,
которые настраивают библиотеку - #UMBA_USE_MCU, #UMBA_USE_QT ..., и тогда библиотека 
сама подключает необходимые хидеры, и 2) когда перед подключением заголовков библиотеки
подключаются сначала системные хидеры


\subsection UMBA_X_DETECT_ENVIRONMENT_USAGE_EXPLICIT_TARGET Явное задание целевой платформы
  - #UMBA_USE_MCU, #UMBA_USE_QT - для использования хидеров stm и Qt.
  - #UMBA_USE_WIN32 - не нужно определять, так как при компиляции под Windows всегда определяется макрос \b _WIN32.
  - #UMBA_USE_FREERTOS - для подключения хидеров FreeRTOS


\subsection UMBA_X_DETECT_ENVIRONMENT_USAGE_TARGET_AUTO_DETECTION Автоматическое определение целевой платформы
Все системные заголовки требуется подключать до использования библиотеки UMBA.
Под системными подразувеваются заголовки Windows, Qt, FreeRTOS и тп.
Если ничего не используется, то предполагается, что используется камень,
и производится подключение заголовков CMSIS для сответствующего МК.

При сборке под Windows определен макрос \b _WIN32, конфигурация автоматически определяется как
#UMBA_WIN32_USED и подключаются необходимые заголовочные файлы.

В зависимости от системы автоматически определяются макросы:
- #UMBA_WIN32_USED, 
- #UMBA_FREERTOS_USED,
- #UMBA_QT_USED.
- #UMBA_MCU_USED.

Их можно использовать в своем коде для проверки окружения.
Для Windows #UMBA_WIN32_USED определен всегда, и может быть определен одновременно с #UMBA_QT_USED или с #UMBA_FREERTOS_USED.

#UMBA_MCU_USED - определен, когда библиотека используется для сборки под железо

В зависимости от используемого компилятора автоматически определяются макросы:
- #UMBA_MSVC_COMPILER_USED,
- #UMBA_KEIL_ARMCC_COMPILER_USED,
- #UMBA_KEIL_CLANG_COMPILER_USED,
- #UMBA_IARCC_COMPILER_USED,
- #UMBA_GCC_COMPILER_USED.


*/

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
/*! 
    \ingroup UMBA_LIBS_TUNING_MACROS
    \brief Служит для отключения всех устаревших возможностей

    Никак не определяется в исходниках, при необходимости должен быть задан средствами среды разработки/сборочного инструмента.
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_DISABLE_DEPRECATED
#endif

//------------------------------
/*! 
    \ingroup UMBA_LIBS_TUNING_MACROS
    \brief Служит для отключения всех отладочных проверок.

    Никак не определяется в исходниках, при необходимости должен быть задан средствами среды разработки/сборочного инструмента.
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_NDEBUG
#endif

//------------------------------
/*! 
    \ingroup UMBA_LIBS_TUNING_MACROS
    \brief Служит для отключения виртуального деструктора у интерфейсов. Сэкономит 4 байта в каждой VTBL. 
    По умолчанию виртуальный деструктор объявлен у базового интерфейса IUnknown, чтобы разные дотошные компиляторы не генерировали варнинги.

    Никак не определяется в исходниках, при необходимости должен быть задан средствами среды разработки/сборочного инструмента.
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_INTERFACE_NO_VDTOR
#endif

//------------------------------
/*! 
    \ingroup UMBA_LIBS_TUNING_MACROS
    \brief По умолчанию странные виндовые макросы min и max андефайнятся, ибо они адское зло. Но если по странной прихоти их хочется оставить, то используйте данный макрос.

    Никак не определяется в исходниках, при необходимости должен быть задан средствами среды разработки/сборочного инструмента.
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_KEEP_MINMAX
#endif

//----------------------------------------------------------------------------







//----------------------------------------------------------------------------
/*! 
    \ingroup UMBA_LIBS_TUNING_MACROS
    \brief Указывает использовать версию кода для MCU

    Никак не определяется в исходниках, при необходимости должен быть задан средствами среды разработки/сборочного инструмента.
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_USE_MCU
#endif

//------------------------------
/*! 
    \ingroup UMBA_LIBS_TUNING_MACROS
    \brief Указывает использовать версию кода для Qt и подключает некоторые основные заголовочные файлы Qt

    Никак не определяется в исходниках, при необходимости должен быть задан средствами среды разработки/сборочного инструмента.
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_USE_QT
#endif

//------------------------------
/*! 
    \ingroup UMBA_LIBS_TUNING_MACROS
    \brief Указывает использовать версию кода для Win32/Win64. Обычно не требует явного указания.

    Никак не определяется в исходниках, при необходимости должен быть задан средствами среды разработки/сборочного инструмента.
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_USE_WIN32
#endif

//------------------------------
/*! 
    \ingroup UMBA_LIBS_TUNING_MACROS
    \brief Указывает использовать версию кода для FreeRTOS

    Никак не определяется в исходниках, при необходимости должен быть задан средствами среды разработки/сборочного инструмента.
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_USE_FREERTOS
#endif

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
/*! 
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Определён, если целевая платформа - Win32/Win64
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_WIN32_USED
#endif

//------------------------------
/*! 
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Определён, если целевая платформа - Win64
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_WIN64_USED
#endif

//------------------------------
/*! 
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Определён, если целевая платформа - Qt.

    В зависимости от целевой системы могут быть определены макросы #UMBA_WIN32_USED/#UMBA_WIN64_USED и другие
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_QT_USED
#endif

//------------------------------
/*! \def UMBA_FREERTOS_USED
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Определён, если целевая платформа - FreeRTOS. 
    
    Обычно определён при использовании FreeRTOS на MCU совместно #UMBA_MCU_USED. 
    Теоретически, FreeRTOS вроде может быть запущена и под Window. В этом случае будут определены макросы
    #UMBA_WIN32_USED/#UMBA_WIN64_USED.
    
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_FREERTOS_USED
#endif

//------------------------------
/*! 
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Определён, если целевая платформа - MCU
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_MCU_USED
#endif

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
/*! 
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Определён, если используется компилятор Microsoft Visual C++
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_MSVC_COMPILER_USED
#endif

//------------------------------
/*! 
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Определён, если используется компилятор ARMCC (Keil)
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_KEIL_ARMCC_COMPILER_USED
#endif

//------------------------------
/*! 
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Определён, если используется компилятор ARMCC
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_ARMCC_COMPILER_USED
#endif

//------------------------------
/*! 
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Определён, если используется компилятор CLang (Keil/ARMCC v6)
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_KEIL_CLANG_COMPILER_USED
#endif

//------------------------------
/*! 
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Определён, если используется компилятор ARMCC/CLang (Keil/ARMCC v6). Аналогично #UMBA_KEIL_CLANG_COMPILER_USED
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_ARMCC_CLANG_COMPILER_USED
#endif

//------------------------------
/*! 
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Определён, если используется компилятор CLang
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_CLANG_COMPILER_USED
#endif

//------------------------------
/*! 
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Определён, если используется компилятор IAR.

    У IAR в названиях макросов, которые определяют его компилятор, часто фигурирует строка "ICC".
    Но "ICC" - это известная аббревиатура для Intel C Compiler (который раньше был весьма крут в оптимизации под интеловские процы),
    У него даже экзешник так и называется. Чтобы не возникало путаницы, компилятор IAR обозван IARCC.
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_IARCC_COMPILER_USED
#endif

//------------------------------
/*! 
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Определён, если используется компилятор GCC (GNU C/C++ Compiler from GNU Compiler Collection)
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_GCC_COMPILER_USED
#endif

//------------------------------
/*! 
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Определён, если используется компилятор Borland или производный.
    
    Он также определён, если используется компилятор Inprise/CodeGear, 
    исходя из той логики, что тама люди соблюдали какую-то преемственность, а не переписывали всё заново при переименованиях конторы.

    Можно рассматривать абревиатуру BCC как Borland Compiler Collection, и если нет уточнений, то это есть изначальный Борман.
    См. #UMBA_CGC_COMPILER_USED, #UMBA_BCC_CGC_COMPILER_USED
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_BCC_COMPILER_USED
#endif

//------------------------------
/*! 
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Определён, если используется компилятор CodeGear (ранее бывшый Borland'ом).
    См. #UMBA_BCC_COMPILER_USED, #UMBA_BCC_CGC_COMPILER_USED
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_CGC_COMPILER_USED
#endif

//------------------------------
/*! 
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Определён, если используется компилятор CodeGear. CGC легко перепутать с GCC, поэтому был добавлен макрос, означающий
    Borland Compiler Collection - CodeGear Compiler.
    См. #UMBA_BCC_COMPILER_USED, #UMBA_CGC_COMPILER_USED
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_BCC_CGC_COMPILER_USED
#endif
//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
/*! 
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Определён, если на целевой архитектуре используется порядок хранения данных в памяти Little Endian - сначала младшие (младшие части по младшим адресам)
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_ARCH_LITTLE_ENDIAN
#endif

//------------------------------
/*! 
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Определён, если на целевой архитектуре используется порядок хранения данных в памяти Big Endian - сначала старшие (старшие части по младшим адресам)
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_ARCH_BIG_ENDIAN
#endif

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
/*! 
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Определён, если целевая архитектура - Intel x86 совместимый процессор (32ух или более разрядный, само собой)
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_ARCH_X86
#endif

//------------------------------
/*! 
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Определён, если целевая архитектура - проц, совместимый с расширениями x86 от AMD до 64ти-четырёх разрядов
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_ARCH_AMD64
#endif

//------------------------------
/*! 
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Устоявшийся алиас для #UMBA_ARCH_AMD64
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_ARCH_X64
#endif

//------------------------------
/*! 
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Определён, если целевая архитектура - ARM (32/64 разряда)
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_ARCH_ARM
#endif

//------------------------------
/*! 
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Определён, если целевая архитектура - ARM64. При этом определяется и макрос #UMBA_ARCH_ARM
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_ARCH_ARM64
#endif

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
/*! \def UMBA_CXX_HAS_STD03
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Определён, если компилятор поддерживает стандарт C++03 на уровне конструкций языка. 
    В основном все компиляторы поддерживают и стандартные библиотеки на уровне C++03.
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_CXX_HAS_STD03
#endif

//------------------------------

/*! \def UMBA_CXX_HAS_STD11
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Определён, если компилятор поддерживает стандарт C++11 на уровне конструкций языка. 
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_CXX_HAS_STD11
#endif

//------------------------------

/*! \def UMBA_CXX_HAS_STD14
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Определён, если компилятор поддерживает стандарт C++14 на уровне конструкций языка. 
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_CXX_HAS_STD14
#endif

//------------------------------

/*! \def UMBA_CXX_HAS_STD17
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Определён, если компилятор поддерживает стандарт C++17 на уровне конструкций языка. 
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_CXX_HAS_STD17
#endif

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
/*! \def UMBA_PP_HAS_COUNTER
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Определён, если есть поддержка макроса __COUNTER__. Довольное ненужное, да, чиста для унификации.
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_PP_HAS_COUNTER
#endif

//------------------------------
/*! \def UMBA_PP_NO_COUNTER
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Определён, если нет поддержки макроса __COUNTER__. Довольное ненужное, да, чиста для унификации.
 */
#ifdef DOXYGEN_ONLY
    #define UMBA_PP_NO_COUNTER
#endif

//----------------------------------------------------------------------------




#if 0
//----------------------------------------------------------------------------
/*! \def 
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Определён, если 
 */
#endif





















//----------------------------------------------------------------------------
//! Макрос-детектор для файла zz_detect_environment.h - если он определён, значит файл zz_detect_environment.h был подключен
#ifndef UMBA_X_DETECT_ENVIRONMENT_H
    #define UMBA_X_DETECT_ENVIRONMENT_H
#endif

#ifdef UMBA_STRICT
    #ifndef UMBA_DISABLE_DEPRECATED
        #define UMBA_DISABLE_DEPRECATED
    #endif
#endif

#include "zz_inc_windows.h"


#if defined(UMBA_USE_QT)
    #include <QObject>
#endif


#if defined(QT_VERSION) || defined(UMBA_USE_QT)
    #ifndef UMBA_QT_USED
        #define UMBA_QT_USED
    #endif
#endif


#if defined(_WIN32) || defined(WIN32)
    #define UMBA_WIN32_USED
#endif

#if defined(_WIN64) || defined(WIN64)
    #define UMBA_WIN64_USED
#endif

// Надеюсь, что данный макрос определён, когда линупс - целевая система, а не хост
#if defined(__linux__)
    #define UMBA_LINUX_USED
#endif

#if defined(UMBA_LINUX_USED) && ( defined(__amd64__) || defined(__x86_64__) )
    #define UMBA_LINUX64_USED
#endif



#ifdef UMBA_USE_FREERTOS
    #include <FreeRTOS.h>
    #define  UMBA_FREERTOS_USED
#else
    #ifdef INC_FREERTOS_H
        #define UMBA_FREERTOS_USED
    #endif
#endif


//! @cond Doxygen_Suppress_Not_Documented
#if !defined(UMBA_WIN32_USED) && !defined(UMBA_QT_USED) && !defined(UMBA_LINUX_USED)
    #define UMBA_MCU_USED
#endif
//! @endcond



// https://sourceforge.net/p/predef/wiki/Compilers/

#if defined(_MSC_VER)

    #define UMBA_MSVC_COMPILER_USED

    #if defined(_M_IX86)     /* This macro isn't defined for x64 or ARM compilation targets. */

        #define UMBA_ARCH_X86 /* Intel x86 */
        #define UMBA_ARCH_LITTLE_ENDIAN

    #elif defined(_M_AMD64)  /* Defined as the integer literal value 100 for compilations that target x64 processors. Otherwise, undefined. */

        #define UMBA_ARCH_X64
        #define UMBA_ARCH_AMD64
        #define UMBA_ARCH_LITTLE_ENDIAN

    #elif defined(_M_ARM64)  /* Defined as 1 for compilations that target 64-bit ARM processors. Otherwise, undefined. */

        #define UMBA_ARCH_ARM
        #define UMBA_ARCH_ARM64
        #define UMBA_ARCH_LITTLE_ENDIAN

    #elif defined(_M_ARM)    /* Defined as the integer literal value 7 for compilations that target ARM processors. Otherwise, undefined. */

        #define UMBA_ARCH_ARM
        #define UMBA_ARCH_LITTLE_ENDIAN

    #else

        #error "Windows Arch is unknown"

    #endif

    //! Версия MSVC (Для сравнения со значениями UMBA_MSVC_VER_*)
    #define UMBA_MSVC         _MSC_VER
    //! \copydoc UMBA_MSVC
    #define UMBA_MSVC_VER     UMBA_MSVC
    //! Версия компилятора
    #define UMBA_CCVER        UMBA_MSVC_VER


#elif defined(__CC_ARM)

    // Obsolette
    #define UMBA_KEIL_ARMCC_COMPILER_USED
    // New
    #define UMBA_ARMCC_COMPILER_USED

    #define UMBA_ARCH_ARM

    #if defined(__ARM_BIG_ENDIAN)
        #define UMBA_ARCH_BIG_ENDIAN
    #else
        #define UMBA_ARCH_LITTLE_ENDIAN
    #endif

    //! Версия ARMCC (Для сравнения со значениями UMBA_ARMCC_VER_*)
    /*! Формат - PVVbbbb, где: P is the major version; VV is the minor version; bbbb is the build number
     */
    #define UMBA_ARMCC        __ARMCC_VERSION
    //! \copydoc UMBA_ARMCC
    #define UMBA_ARMCC_VER    UMBA_ARMCC
    //! Версия компилятора
    #define UMBA_CCVER        UMBA_ARMCC_VER


#elif defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
//#elif defined (__ARMCC_VERSION) && defined(__clang__)
    /* Как правильнее? */

    // Obsolette
    #define UMBA_KEIL_CLANG_COMPILER_USED
    // New
    #define UMBA_CLANG_COMPILER_USED
    #define UMBA_ARMCC_CLANG_COMPILER_USED

    #if defined(__ARM_BIG_ENDIAN)
        #define UMBA_ARCH_BIG_ENDIAN
    #else
        #define UMBA_ARCH_LITTLE_ENDIAN
    #endif

    //! Версия ARMCC (Для сравнения со значениями UMBA_ARMCC_VER_*)
    /*! Формат - PVVbbbb, где: P is the major version; VV is the minor version; bbbb is the build number
     */
    #define UMBA_ARMCC        __ARMCC_VERSION
    //! \copydoc UMBA_ARMCC
    #define UMBA_ARMCC_VER    UMBA_ARMCC
    //! Версия компилятора
    #define UMBA_CCVER        UMBA_ARMCC_VER


#elif defined(__ICCARM__) || defined(__IAR_SYSTEMS_ICC__)

    // Obsolette
    //#define UMBA_IAR_ICC_COMPILER_USED
    // New
    //#define UMBA_ICC_COMPILER_USED

    #define UMBA_IARCC_COMPILER_USED

    #define UMBA_ARCH_LITTLE_ENDIAN


#elif defined(__GNUC__)

    #define UMBA_GCC_COMPILER_USED

    #define UMBA_ARCH_LITTLE_ENDIAN

#elif defined(__clang__)

    #define UMBA_CLANG_COMPILER_USED

    #define UMBA_ARCH_LITTLE_ENDIAN

#elif defined(__BORLANDC__)

    #define UMBA_BCC_COMPILER_USED

    #define UMBA_ARCH_LITTLE_ENDIAN

#elif defined(__CODEGEARC__)

    #define UMBA_BCC_COMPILER_USED
    #define UMBA_CGC_COMPILER_USED
    #define UMBA_BCC_CGC_COMPILER_USED
    // __CODEGEARC__

    #define UMBA_ARCH_LITTLE_ENDIAN

#else

    #error "Unknown (unsupported) compiler"

#endif





#if defined(UMBA_LINUX_USED)

    #if defined(__x86__)

        #if !defined(UMBA_ARCH_X86)

            #define UMBA_ARCH_X86

        #endif

        #if !defined(UMBA_ARCH_LITTLE_ENDIAN)

            #define UMBA_ARCH_LITTLE_ENDIAN

        #endif

    #elif defined(__amd64__) || defined(__x86_64__)

        #if !defined(UMBA_ARCH_X64)

            #define UMBA_ARCH_X64

        #endif

        #if !defined(UMBA_ARCH_AMD64)

            #define UMBA_ARCH_AMD64

        #endif

        #if !defined(UMBA_ARCH_LITTLE_ENDIAN)

            #define UMBA_ARCH_LITTLE_ENDIAN

        #endif

    #else

        #error "Linux Arch is unknown"

    #endif

#endif



// UMBA_PP_NO_*, UMBA_PP_HAS_* - preprocessor feature flags
// UMBA_CC_ - Plain С feature flags
// UMBA_CXX_ - С++ feature flags

//! @cond Doxygen_Suppress_Not_Documented
#if defined(__COUNTER__)
    #define UMBA_PP_HAS_COUNTER
#else
    #define UMBA_PP_NO_COUNTER
#endif
//! @endcond



//! Версия uMicrovision - uMicrovision 5 ARMCC 5.06 (build 750)
#define UMBA_ARMCC_VER_UVISION5_ARMCC        5060750
//! Версия uMicrovision/ARMCC - минимальная версия CLang-based ARMCC
#define UMBA_ARMCC_VER_UVISION5_CLANG_MIN    6010050
//! Версия uMicrovision/ARMCC - CLang-based ARMCC 6.09
#define UMBA_ARMCC_VER_UVISION5_CLANG        6090000



//----------------------------------------------------------------------------
// Предопределённые макросы MSVC - https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=vs-2019

//----------------------------------------------------------------------------
//! Версия MSVC - Visual Studio 6.0
#define UMBA_MSVC_VER_VS_6                                                1200

//----------------------------------------------------------------------------
//! Версия MSVC - Visual Studio .NET 2002 (7.0)
#define UMBA_MSVC_VER_70                                                  1300
//! \copybrief UMBA_MSVC_VER_70
#define UMBA_MSVC_VER_VS_2002        UMBA_MSVC_VER_70
//! \copybrief UMBA_MSVC_VER_70
#define UMBA_MSVC_VER_VS_NET         UMBA_MSVC_VER_2002

//----------------------------------------------------------------------------
//! Версия MSVC - Visual Studio .NET 2003 (7.1)
#define UMBA_MSVC_VER_71                                                  1310
//! \copybrief UMBA_MSVC_VER_71
#define UMBA_MSVC_VER_VS_2003        UMBA_MSVC_VER_71
//! \copybrief UMBA_MSVC_VER_71
#define UMBA_MSVC_VER_VS_NET_03      UMBA_MSVC_VER_2003
//! \copybrief UMBA_MSVC_VER_71
#define UMBA_MSVC_VER_VS_NET_2003    UMBA_MSVC_VER_2003

//----------------------------------------------------------------------------
//! Версия MSVC - Visual Studio 2005 (8.0)
#define UMBA_MSVC_VER_80                                                  1400
//! \copybrief UMBA_MSVC_VER_80
#define UMBA_MSVC_VER_VS_2005        UMBA_MSVC_VER_80

//----------------------------------------------------------------------------
//! Версия MSVC - Visual Studio 2008 (9.0)
#define UMBA_MSVC_VER_90                                                  1500
//! \copybrief UMBA_MSVC_VER_90
#define UMBA_MSVC_VER_VS_2008        UMBA_MSVC_VER_90

//----------------------------------------------------------------------------
//! Версия MSVC - Visual Studio 2010 (10.0)
#define UMBA_MSVC_VER_10                                                  1600
//! \copybrief UMBA_MSVC_VER_10
#define UMBA_MSVC_VER_VS_2010        UMBA_MSVC_VER_10

//----------------------------------------------------------------------------
//! Версия MSVC - Visual Studio 2012 (11.0)
#define UMBA_MSVC_VER_11                                                  1700
//! \copybrief UMBA_MSVC_VER_11
#define UMBA_MSVC_VER_VS_2012        UMBA_MSVC_VER_11

//----------------------------------------------------------------------------
//! Версия MSVC - Visual Studio 2013 (12.0)
#define UMBA_MSVC_VER_12                                                  1800
//! \copybrief UMBA_MSVC_VER_12
#define UMBA_MSVC_VER_VS_2013        UMBA_MSVC_VER_12

//----------------------------------------------------------------------------
//! Версия MSVC - Visual Studio 2015 (14.0)
#define UMBA_MSVC_VER_14                                                  1900
//! \copybrief UMBA_MSVC_VER_14
#define UMBA_MSVC_VER_VS_2015        UMBA_MSVC_VER_14

//----------------------------------------------------------------------------
//! Версия MSVC - Visual Studio 2017 RTW (15.0)
#define UMBA_MSVC_VER_150                                                 1910
//! \copybrief UMBA_MSVC_VER_150
#define UMBA_MSVC_VER_VS_2017        UMBA_MSVC_VER_150
//! \copybrief UMBA_MSVC_VER_150
#define UMBA_MSVC_VER_VS_2017_0      UMBA_MSVC_VER_150
//! \copybrief UMBA_MSVC_VER_150
#define UMBA_MSVC_VER_VS_2017_RTW    UMBA_MSVC_VER_150

//----------------------------------------------------------------------------
//! Версия MSVC - Visual Studio 2017 version 15.3
#define UMBA_MSVC_VER_153                                                 1911
//! \copybrief UMBA_MSVC_VER_153
#define UMBA_MSVC_VER_VS_2017_3      UMBA_MSVC_VER_153

//----------------------------------------------------------------------------
//! Версия MSVC - Visual Studio 2017 version 15.5
#define UMBA_MSVC_VER_155                                                 1912
//! \copybrief UMBA_MSVC_VER_155
#define UMBA_MSVC_VER_VS_2017_5      UMBA_MSVC_VER_155

//----------------------------------------------------------------------------
//! Версия MSVC - Visual Studio 2017 version 15.6
#define UMBA_MSVC_VER_156                                                 1913
//! \copybrief UMBA_MSVC_VER_156
#define UMBA_MSVC_VER_VS_2017_6      UMBA_MSVC_VER_156

//----------------------------------------------------------------------------
//! Версия MSVC - Visual Studio 2017 version 15.7
#define UMBA_MSVC_VER_157                                                 1914
//! \copybrief UMBA_MSVC_VER_157
#define UMBA_MSVC_VER_VS_2017_7      UMBA_MSVC_VER_157

//----------------------------------------------------------------------------
//! Версия MSVC - Visual Studio 2017 version 15.8
#define UMBA_MSVC_VER_158                                                 1915
//! \copybrief UMBA_MSVC_VER_158
#define UMBA_MSVC_VER_VS_2017_8      UMBA_MSVC_VER_158

//----------------------------------------------------------------------------
//! Версия MSVC - Visual Studio 2017 version 15.9
#define UMBA_MSVC_VER_159                                                 1916
//! \copybrief UMBA_MSVC_VER_159
#define UMBA_MSVC_VER_VS_2017_9      UMBA_MSVC_VER_159


//----------------------------------------------------------------------------
//! Версия MSVC - Visual Studio 2019 RTW (16.0)
#define UMBA_MSVC_VER_160                                                 1920
//! \copybrief UMBA_MSVC_VER_160
#define UMBA_MSVC_VER_VS_2019        UMBA_MSVC_VER_160
//! \copybrief UMBA_MSVC_VER_160
#define UMBA_MSVC_VER_VS_2019_0      UMBA_MSVC_VER_160
//! \copybrief UMBA_MSVC_VER_160
#define UMBA_MSVC_VER_VS_2019_RTW    UMBA_MSVC_VER_160

//----------------------------------------------------------------------------
//! Версия MSVC - Visual Studio 2019 version 16.1
#define UMBA_MSVC_VER_161                                                 1921
//! \copybrief UMBA_MSVC_VER_161
#define UMBA_MSVC_VER_VS_2019_1      UMBA_MSVC_VER_161

//----------------------------------------------------------------------------
//! Версия MSVC - Visual Studio 2019 version 16.2
#define UMBA_MSVC_VER_162                                                 1922
//! \copybrief UMBA_MSVC_VER_162
#define UMBA_MSVC_VER_VS_2019_2      UMBA_MSVC_VER_162

//----------------------------------------------------------------------------
//! Версия MSVC - Visual Studio 2019 version 16.3
#define UMBA_MSVC_VER_163                                                 1923
//! \copybrief UMBA_MSVC_VER_163
#define UMBA_MSVC_VER_VS_2019_3      UMBA_MSVC_VER_163

//----------------------------------------------------------------------------
//! Версия MSVC - Visual Studio 2019 version 16.4
#define UMBA_MSVC_VER_164                                                 1924
//! \copybrief UMBA_MSVC_VER_164
#define UMBA_MSVC_VER_VS_2019_4      UMBA_MSVC_VER_164

//----------------------------------------------------------------------------
//! Версия MSVC - Visual Studio 2019 version 16.5
#define UMBA_MSVC_VER_165                                                 1925
//! \copybrief UMBA_MSVC_VER_165
#define UMBA_MSVC_VER_VS_2019_5      UMBA_MSVC_VER_165

//----------------------------------------------------------------------------
//! Версия MSVC - Visual Studio 2019 version 16.6
#define UMBA_MSVC_VER_166                                                 1926
//! \copybrief UMBA_MSVC_VER_166
#define UMBA_MSVC_VER_VS_2019_6      UMBA_MSVC_VER_166

//----------------------------------------------------------------------------
//! Версия MSVC - Visual Studio 2019 version 16.7
#define UMBA_MSVC_VER_167                                                 1927
//! \copybrief UMBA_MSVC_VER_167
#define UMBA_MSVC_VER_VS_2019_7      UMBA_MSVC_VER_167






//! @cond Doxygen_Suppress_Not_Documented
#if defined(_MSC_VER)

    #if !defined(_MSVC_LANG)
        #define _MSVC_LANG __cplusplus
    #endif

    #if _MSVC_LANG >= 199711L
        #define UMBA_CXX_HAS_STD03
    #endif

    #if _MSVC_LANG >= 201103L
        #define UMBA_CXX_HAS_STD11
    #endif
    
    #if _MSVC_LANG >= 201402L
        #define UMBA_CXX_HAS_STD14
    #endif
    
    #if _MSVC_LANG >= 201703L
        #define UMBA_CXX_HAS_STD17
    #endif


#elif defined(__cplusplus)

    #if __cplusplus >= 201703L
        #define UMBA_CXX_HAS_STD17
    #endif
   
    #if __cplusplus >= 201402L
        #define UMBA_CXX_HAS_STD14
    #endif

    #if __cplusplus >= 201103L
        #define UMBA_CXX_HAS_STD11
    #endif

    #if __cplusplus >= 199711L
        #define UMBA_CXX_HAS_STD03
    #endif


#endif
//! @endcond



#if !defined(UMBA_ARCH_LITTLE_ENDIAN) && !defined(UMBA_ARCH_BIG_ENDIAN)

    #define UMBA_ARCH_LITTLE_ENDIAN

#endif



//! Определяется модулем UMBA для возможности автоматического определения факта использования данного модуля
#define MODULE_UMBA_USED



#if defined(UMBA_WIN32_USED)

    #include "zz_inc_windows.h"

#elif defined(UMBA_QT_USED)

    // Тут можно подключать Qt

#elif defined(UMBA_MCU_USED)

    #include "mcu.h"
    #include "stm32_drivers.h"

    
#endif




