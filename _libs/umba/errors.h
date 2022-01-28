#pragma once

//----------------------------------------------------------------------------

/*! \file
    \brief Ошибки, и связанные с ними определения
*/

//----------------------------------------------------------------------------

#include "umba/preprocessor.h"
#include "umba/stl.h"

#if defined(UMBA_WIN32_USED)
    #include <stdio.h>
#endif

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------

namespace umba{


typedef uint32_t  ErrorCode; //!< Сырой целочисленный тип кода ошибки



//----------------------------------------------------------------------------
namespace errors
{
    const ErrorCode errorFlag              = 0x80000000;  //!< Флаг - ошибка
    const ErrorCode invertFlag             = 0x40000000;  //!< Флаг инверсии

    const ErrorCode errorSubsystemMask     = 0x0F000000;  //!< Маска подсистемы

    const ErrorCode errorSubsystemUmba     = 0x00000000;  //!< Подсистема UMBA
    const ErrorCode errorSubsystemWindows  = 0x01000000;  //!< Подсистема Windows - проброс виндового кода ошибки
    const ErrorCode errorSubsystemPosix    = 0x02000000;  //!< Подсистема Posix - проброс Posix ошибки - Linux ошибка или (под Windows) - ошибка CRT
    const ErrorCode errorSubsystemQt       = 0x03000000;  //!< Подсистема Qt - ошибка фреймворка Qt


    //! Код ошибки из виндового кода
    inline ErrorCode makeFromWindows( ErrorCode errorCode )
    {
     return errorFlag | errorSubsystemWindows | (errorCode & ~(errorSubsystemMask | errorFlag));
    }

    //! Код ошибки из Posix кода
    inline ErrorCode makeFromPosix( ErrorCode errorCode )
    {
     return errorFlag | errorSubsystemPosix | (errorCode & ~(errorSubsystemMask | errorFlag));
    }

    //! Код ошибки из Qt кода
    inline ErrorCode makeFromQt( ErrorCode errorCode )
    {
     return errorFlag | errorSubsystemQt | (errorCode & ~(errorSubsystemMask | errorFlag));
    }

#ifndef DOXYGEN_SHOULD_SKIP_THIS

    #ifdef UMBA_DECLARE_ERROR_CODE
        #undef UMBA_DECLARE_ERROR_CODE
    #endif

    #ifdef UMBA_DECLARE_ERROR_CODE_ALIAS
        #undef UMBA_DECLARE_ERROR_CODE_ALIAS
    #endif

    /* info */
    #define UMBA_DECLARE_ERROR_CODE(name, code, info) \
                 const ErrorCode name = code; 
    #define UMBA_DECLARE_ERROR_CODE_ALIAS(name, code, info) \
                 const ErrorCode name = code; 

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

    #include "zz_error_codes.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS

    #undef UMBA_DECLARE_ERROR_CODE
    #undef UMBA_DECLARE_ERROR_CODE_ALIAS

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

} // namespace errors

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
template <typename TValue>
struct Result;

#if 0
    //! Конструктор по умолчанию
    //! Конструктор из объекта "ошибка"
    //! Конструктор из кода ошибки и результата (не все ошибки - фатальные)
    //! Конструктор копирования
    //! Конструктор перемещения
#endif



//! Плейс-холдер для кода ошибки
class Error
{

public:

    mutable ErrorCode    result; //!< Код ошибки

    //! Конструктор по умолчанию - создаёт "Ок - Нет ошибки"
    Error()               : result(errors::ok) {}
    //! Конструктор из кода ошибки
    Error(ErrorCode ec)   : result(ec) {}
    //! Конструктор копирования
    Error(const Error &e) : result(e.result) {}

    //! Оператор присваивания кода ошибки
    Error& operator=(const ErrorCode &e) { result = e;        return *this; }
    //! Оператор присваивания
    Error& operator=(const Error &e)     { result = e.result; return *this; }

    //! Преобразование в Result
    template <typename TValue>
    Error( const Result<TValue> &r );

    //! Проверка на сбой
    bool isFail() const
    {
        return (result & errors::errorFlag) ? true : false;
    }

    //! Проверка на успех
    bool isSuccess() const
    {
        return !isFail();
    }

    //! Проверка на полный успех
    bool isClean() const
    {
        return result == 0;
    }

    //! Использование в операторе if и сравнение с bool
    /*! Проверка на успешность вызова:

        if (auto a = someFunction()) { Ветка по успеху }
     */
    UMBA_EXPLICIT
    operator bool() const
    {
        if ((result&errors::invertFlag)==0) // invert flag not set
            return isSuccess();

        result &= ~ errors::invertFlag;
        return isFail();
    }

    //! Использование в операторе if с инверсией
    /*! Проверка на фейл вызова:

        if (auto a = !someFunction()) { Ветка по неудаче }
     */
    Error operator!() const
    {
        return Error(result | errors::invertFlag);
    }

    #ifdef UMBA_MCU_USED

      #ifndef DOXYGEN_SHOULD_SKIP_THIS

        // errors:: 
        #define UMBA_DECLARE_ERROR_CODE(name, code, info) \
                     case name : return UMBA_STRINGIFY(name);
        #define UMBA_DECLARE_ERROR_CODE_ALIAS(name, code, info)

      #endif /* DOXYGEN_SHOULD_SKIP_THIS */


        //! Возвращает строку с сообщением об ошибке
        const char* what() const
        {
            switch(result & ~errors::invertFlag)
            {
                using namespace errors;
                #include "zz_error_codes.h"
                #undef UMBA_DECLARE_ERROR_CODE
                #undef UMBA_DECLARE_ERROR_CODE_ALIAS
                default:
                {
                     return "Unknown error";
                }
            };
        }

        //! Возвращает Unicode строку сообщением об ошибке
        const wchar_t* whatWide() const
        {
            return L"Some error";
        }

    #else

      #ifndef DOXYGEN_SHOULD_SKIP_THIS

        // errors:: 
        #define UMBA_DECLARE_ERROR_CODE(name, code, info) \
                     case name : return info;
        #define UMBA_DECLARE_ERROR_CODE_ALIAS(name, code, info)

      #endif /* DOXYGEN_SHOULD_SKIP_THIS */

        //! Возвращает строку с сообщением об ошибке
        const char* what() const
        {
            switch(result & ~errors::invertFlag)
            {
                using namespace errors;
                #include "zz_error_codes.h"
                #undef UMBA_DECLARE_ERROR_CODE
                #undef UMBA_DECLARE_ERROR_CODE_ALIAS
                default:
                {
                    #if defined(_WIN32) || defined(WIN32)
                        ErrorCode cleanCode = result & ~(errorSubsystemMask|errorFlag|invertFlag);
                        const char* fmt = "Unknown error %d (0x%08X)";
                        switch(result&errorSubsystemMask)
                        {
                            case errorSubsystemUmba:     fmt = "Umba error %d (0x%08X)";    break;
                            case errorSubsystemWindows:  fmt = "Windows error %d (0x%08X)"; break;
                            case errorSubsystemPosix:    fmt = "Posix error %d (0x%08X)";   break;
                            case errorSubsystemQt:       fmt = "Qt error %d (0x%08X)";      break;
                            //default:                     return "Unknown error";
                        };

                        #if defined(UMBA_CXX_HAS_STD11)
                            thread_local 
                        #else
                            #if defined(_MSC_VER)
                                __declspec( thread )
                            #endif
                        #endif
                        static char buf[64];

                        sprintf(buf, fmt, cleanCode, result);
                        return buf;
                    #else
                        switch(result&errorSubsystemMask)
                        {
                            case errorSubsystemUmba:     return "Umba error";
                            case errorSubsystemWindows:  return "Windows error";
                            case errorSubsystemPosix:    return "Posix error";
                            case errorSubsystemQt:       return "Qt error";
                            default:                     return "Unknown error";
                        };
                    #endif
                }
            };
        }

      #ifndef DOXYGEN_SHOULD_SKIP_THIS

        //errors::
        #define UMBA_DECLARE_ERROR_CODE(name, code, info) \
                     case  name : return L##info;
        #define UMBA_DECLARE_ERROR_CODE_ALIAS(name, code, info)

      #endif /* DOXYGEN_SHOULD_SKIP_THIS */

        //! Возвращает Unicode строку сообщением об ошибке
        const wchar_t* whatWide() const
        {
            switch(result & ~errors::invertFlag)
            {
                using namespace errors;
                #include "zz_error_codes.h"
                #undef UMBA_DECLARE_ERROR_CODE
                #undef UMBA_DECLARE_ERROR_CODE_ALIAS
                default:
                {
                    #if defined(_WIN32) || defined(WIN32)
                        ErrorCode cleanCode = result & ~(errorSubsystemMask|errorFlag|invertFlag);
                        const wchar_t* fmt = L"Unknown error %d (0x%08X)";
                        switch(result&errorSubsystemMask)
                        {
                            case errorSubsystemUmba:     fmt = L"Umba error %d (0x%08X)";    break;
                            case errorSubsystemWindows:  fmt = L"Windows error %d (0x%08X)"; break;
                            case errorSubsystemPosix:    fmt = L"Posix error %d (0x%08X)";   break;
                            case errorSubsystemQt:       fmt = L"Qt error %d (0x%08X)";      break;
                            //default:                     return "Unknown error";
                        };
                        #if defined(UMBA_CXX_HAS_STD11)
                            thread_local 
                        #else
                            #if defined(_MSC_VER)
                                __declspec( thread )
                            #endif
                        #endif
                        static wchar_t buf[64];

                        swprintf(buf, fmt, cleanCode, result);
                        return buf;
                    #else
                        switch(result&errorSubsystemMask)
                        {
                            case errorSubsystemUmba:     return L"Umba error";
                            case errorSubsystemWindows:  return L"Windows error";
                            case errorSubsystemPosix:    return L"Posix error";
                            case errorSubsystemQt:       return L"Qt error";
                            default:                     return L"Unknown error";
                        };
                    #endif
                }
            };
        }

    #endif

}; // struct Error

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
//! Проверка двух Error между собой на равенство
inline
bool operator==(Error e1, Error e2)
{
    return e1.result == e2.result;
}

//------------------------------
//! Проверка двух Error между собой на неравенство
inline
bool operator!=(Error e1, Error e2)
{
    return e1.result != e2.result;
}

//------------------------------
//! Проверка Error и ErrorCode на равенство
inline
bool operator==(Error e1, ErrorCode e2)
{
    return e1.result == e2;
}

//------------------------------
//! Проверка Error и ErrorCode на неравенство
inline
bool operator!=(Error e1, ErrorCode e2)
{
    return e1.result != e2;
}

//------------------------------
//! Проверка ErrorCode и Error на равенство
inline
bool operator==(ErrorCode e1, Error e2)
{
    return e1 == e2.result;
}

//------------------------------
//! Проверка ErrorCode и Error на неравенство
inline
bool operator!=(ErrorCode e1, Error e2)
{
    return e1 != e2.result;
}

//------------------------------
//! Проверка Error и bool на равенство - Error равен true, когда он успешен
inline
bool operator==(Error e1, bool e2)
{
    return e1.isSuccess() == e2;
}

//------------------------------
//! Проверка Error и bool на неравенство - Error равен true, когда он успешен
inline
bool operator!=(Error e1, bool e2)
{
    return e1.isSuccess() != e2;
}

//------------------------------
//! Проверка bool и Error на равенство - Error равен true, когда он успешен
inline
bool operator==(bool e1, Error e2)
{
    return e1 == e2.isSuccess();
}

//------------------------------
//! Проверка bool и Error на неравенство - Error равен true, когда он успешен
inline
bool operator!=(bool e1, Error e2)
{
    return e1 != e2.isSuccess();
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
//! Обобщенный результат
/*! Данный класс позволяет возвращать код результата (ошибки)
    одновременно с полезной нагрузкой. Когда полезная нагрузка не нужна,
    следует использовать класс Error
 */
template <typename TValue>
struct Result : public Error
{

public:

    TValue       value;  //!< Значение результата

    //! Конструктор по умолчанию
    Result( )
    : Error()
    , value()
    {}

    //! Конструктор из объекта "ошибка"
    Result( const Error &e )
    : Error(e.result)
    , value()
    {}

    //! Конструктор из кода ошибки и результата (не все ошибки - фатальные)
    Result( ErrorCode ec, const TValue &tv )
    : Error(ec)
    , value(tv)
    {}

    //! Конструктор копирования
    Result( const Result &r )
    : Error(r.result)
    , value(r.value)
    {}

    #if defined(UMBA_CXX_HAS_STD11)
    //! Конструктор перемещения
    Result(Result &&r)
    : Error(r.result)
    , value()
    {
        value = std::move(r.value);
    }
    #endif

    //! Конструктор из значения результата
    Result( const TValue &tv )
    : Error(0)
    , value(tv)
    {}

/*
    explicit operator Error() const
    {
        return Error(result);
    }
*/
    //! Оператор присваивания
    Result& operator=(const Result &r)
    {
        if (&r==this) return *this;
        result = r.result;
        value  = r.value;
        return *this;
    }

    #if defined(UMBA_CXX_HAS_STD11)
    //! Присваивающий оператор присваивания
    Result& operator=(Result &&r)
    {
        if (&r==this) return *this;
        result = std::move(r.result);
        value  = std::move(r.value);
        return *this;
    }
    #endif


    //! Использование в операторе if и сравнение с bool
    /*! Проверка на успешность вызова:

        if (auto a = someFunction()) { Ветка по успеху }

        if (auto a = !someFunction()) { Ветка по неудаче }

     */
    UMBA_EXPLICIT
    operator bool() const
    {
        if ((result&errors::invertFlag)==0) // invert flag not set
            return isSuccess();

        result &= ~ errors::invertFlag;
        return isFail();
    }

    //! Использование в операторе if с инверсией
    /*! Проверка на фейл вызова:

        if (auto a = !someFunction()) { Ветка по неудаче }
     */
    Result operator!() const
    {
        return Result( result | errors::invertFlag, value );
    }

}; // struct Result




template <typename TValue>
inline
Error::Error( const Result<TValue> &r ) : result(r.result)
{
}




} // namespace umba


