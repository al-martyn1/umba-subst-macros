#pragma once

/*!
\file
\brief Константные строки - обёртка для строковых литералов
 */

#include "umba.h"

#if defined(UMBA_KEIL_ARMCC_COMPILER_USED) || defined(UMBA_KEIL_CLANG_COMPILER_USED) || defined(UMBA_MSVC_COMPILER_USED) || defined(UMBA_GCC_COMPILER_USED)

    #include <string.h>

#else
    #error "Other compilers not currently supported"
    // ,
    // UMBA_IAR_ICC_COMPILER_USED
    // UMBA_GCC_COMPILER_USED
#endif


// umba::platform_support

namespace umba{

//! Platform support - всякие функции, которые сцуко везде по разному именуются и тп, хотя делают одно и то же
namespace platform_support{

// Not shure about GCC

#if defined(UMBA_KEIL_ARMCC_COMPILER_USED) || defined(UMBA_KEIL_CLANG_COMPILER_USED) || defined(UMBA_GCC_COMPILER_USED)

    //! Сравнение строк. \returns -1, 0, 1
    inline
    int str_compare( const char *s1, const char *s2 )
    {
        return std::strcmp( s1, s2 );
    }
    
    //! Сравнение первых cnt символов строк. \returns -1, 0, 1
    inline
    int strn_compare( const char *s1, const char *s2, size_t cnt )
    {
        return std::strncmp( s1, s2, cnt );
    }
    
    //! Сравнение первых cnt символов строк без учёта регистра символов. \returns -1, 0, 1
    inline
    int str_icase_compare( const char *s1, const char *s2 )
    {
        return std::strcasecmp( s1, s2 );
    }
    
    //! Сравнение первых cnt символов строк без учёта регистра символов. \returns -1, 0, 1
    inline
    int strn_icase_compare( const char *s1, const char *s2, size_t cnt )
    {
        return std::strncasecmp( s1, s2, cnt );
    }
    
#elif defined(UMBA_MSVC_COMPILER_USED)

    //! Сравнение строк. \returns -1, 0, 1
    inline
    int str_compare( const char *s1, const char *s2 )
    {
        return strcmp( s1, s2 );
    }
    
    //! Сравнение первых cnt символов строк. \returns -1, 0, 1
    inline
    int strn_compare( const char *s1, const char *s2, size_t cnt )
    {
        return strncmp( s1, s2, cnt );
    }
    
    //! Сравнение первых cnt символов строк без учёта регистра символов. \returns -1, 0, 1
    inline
    int str_icase_compare( const char *s1, const char *s2 )
    {
        return _stricmp( s1, s2 );
    }
    
    //! Сравнение первых cnt символов строк без учёта регистра символов. \returns -1, 0, 1
    inline
    int strn_icase_compare( const char *s1, const char *s2, size_t cnt )
    {
        return _strnicmp( s1, s2, cnt );
    }

#else

    //! Сравнение строк. \returns -1, 0, 1
    inline
    int str_compare( const char *s1, const char *s2 )
    {
        UMBA_ASSERT(0 && "Not implemented for this compiler"); return 0;
    }
    
    //! Сравнение первых cnt символов строк. \returns -1, 0, 1
    inline
    int strn_compare( const char *s1, const char *s2, size_t cnt )
    {
        UMBA_ASSERT(0 && "Not implemented for this compiler"); return 0;
    }
    
    //! Сравнение первых cnt символов строк без учёта регистра символов. \returns -1, 0, 1
    inline
    int str_icase_compare( const char *s1, const char *s2 )
    {
        UMBA_ASSERT(0 && "Not implemented for this compiler"); return 0;
    }
    
    //! Сравнение первых cnt символов строк без учёта регистра символов. \returns -1, 0, 1
    inline
    int strn_icase_compare( const char *s1, const char *s2, size_t cnt )
    {
        UMBA_ASSERT(0 && "Not implemented for this compiler"); return 0;
    }
    
#endif

} //namespace platform_support


constexpr bool case_ignore    = true;   //!< Константа, задающая игнорировать регистр символов
constexpr bool case_sensitive = false;  //!< Константа, задающая учитывать регистр символов


//! Константная строка, мимикрирует под std::basic_string
/*!
    Используется для:
    -# Использования строковых констант при помощи удобного и стандартного интерфейса std::basic_string без использования
       собственно std::basic_string - полезно для MCU
    -# Для оптимизации использования - размер строки вычисляется на этапе создания объекта (побочный эффект)

    \todo
    //TODO: Надо бы параметризовать типом Char'ов, сделать BasicConstString, а ConstString - уже конкретная специализация для char.
 */
template< bool CaseIgnore = case_sensitive >
class ConstString
{

public:

    typedef char    value_type; //!< Тип содержимого
    typedef char    char_type;  //!< Тип символа
    typedef size_t  size_type;  //!< Тип размера

    static const bool case_ignore = CaseIgnore; //!< Опция игнорирования регистра строки

    //! Конструктор из сырого массива
    ConstString( const char* str )
    : m_str( str )
    , m_strSize(0)
    {
        UMBA_ASSERT(m_str!=0);
        m_strSize = std::strlen(m_str);
    }

    //! Конструктор копирования
    ConstString( const ConstString &) = default;
    //! Конструктор перемещения
    ConstString( ConstString &&)      = default;

    //! Оператор присвоения
    ConstString& operator=( const ConstString & )       = default;

    //! Возвращает указатель на Zero-terminated строку
    const char* c_str() const
    {
        return m_str;
    }

    //! Возвращает размер строки
    size_t size() const
    {
        return m_strSize;
    }

    //! Оператор индексирования, возвращает ссылку на символ. Только константная версия
    const char& operator[]( size_t idx ) const
    {
        return m_str[idx];
    }

    //! Сравнение с сырой строкой
    int compare( const char *str ) const
    {
        if (CaseIgnore)
            return platform_support::str_icase_compare( m_str, str );
        else
            return platform_support::str_compare( m_str, str );
    }

    //! Сравнение с таким же объектом
    int compare( const ConstString &other ) const
    {
        if (CaseIgnore)
            return platform_support::str_icase_compare( m_str, other.c_str() );
        else
            return platform_support::str_compare( m_str, other.c_str() );
    }

    //! Сравнение с сырой строкой только части строки длиной N
    int compare( const char *str, size_t n ) const
    {
        if (CaseIgnore)
            return platform_support::strn_icase_compare( m_str, str, n );
        else
            return platform_support::strn_compare( m_str, str, n );
    }

    //! Сравнение с таким же объектом только части строки длиной N
    int compare( const ConstString &other, size_t n ) const
    {
        if (CaseIgnore)
            return platform_support::strn_icase_compare( m_str, other.c_str(), n );
        else
            return platform_support::strn_compare( m_str, other.c_str(), n );
    }

protected:

    const char* m_str;     //!< Указатель на сырую строку
    size_t      m_strSize; //!< Размер строки

}; // class ConstString
//-----------------------------------------------------------------------------



typedef ConstString<case_ignore>      ConstStringCaseIgnore;    //!< Регистронезависимая константная строка
typedef ConstString<case_sensitive>   ConstStringCaseSensitive; //!< Регистрочувствительная константная строка






//-----------------------------------------------------------------------------
//! Сравнение строк
template< bool CaseIgnore >
inline
int 
stringsCompare( const ConstString<CaseIgnore> &s1, const char *s2 )
{
    return s1.compare( s2 );
}

//! Сравнение строк
template< bool CaseIgnore >
inline
int 
stringsCompare( const char *s1, const ConstString<CaseIgnore> &s2 )
{
    return - s2.compare( s1 );
}

//! Сравнение строк
template< bool CaseIgnore >
inline
int 
stringsCompare( const ConstString<CaseIgnore> &s1, const ConstString<CaseIgnore> &s2 )
{
    return s1.compare( s2 );
}

//-----------------------------------------------------------------------------
//! Сравнение строк
template< bool CaseIgnore >
inline
int 
stringsCompare( const ConstString<CaseIgnore> &s1, const char *s2, size_t n )
{
    return s1.compare( s2, n );
}

//! Сравнение строк
template< bool CaseIgnore >
inline
int 
stringsCompare( const char *s1, const ConstString<CaseIgnore> &s2, size_t n )
{
    return - s2.compare( s1, n );
}

//! Сравнение строк
template< bool CaseIgnore >
inline
int 
stringsCompare( const ConstString<CaseIgnore> &s1, const ConstString<CaseIgnore> &s2, size_t n )
{
    return s1.compare( s2, n );
}

//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
//! Оператор сравнения
template< bool CaseIgnore >
inline
bool operator==( const ConstString<CaseIgnore> &s1, const char *s2 )
{
    return stringsCompare( s1, s2)==0;
}

//! Оператор сравнения
template< bool CaseIgnore >
inline
bool operator!=( const ConstString<CaseIgnore> &s1, const char *s2 )
{
    return stringsCompare( s1, s2)==0;
}

//! Оператор сравнения
template< bool CaseIgnore >
inline
bool operator<( const ConstString<CaseIgnore> &s1, const char *s2 )
{
    return stringsCompare( s1, s2)<0;
}

//! Оператор сравнения
template< bool CaseIgnore >
inline
bool operator<=( const ConstString<CaseIgnore> &s1, const char *s2 )
{
    return stringsCompare( s1, s2)<=0;
}

//! Оператор сравнения
template< bool CaseIgnore >
inline
bool operator>( const ConstString<CaseIgnore> &s1, const char *s2 )
{
    return stringsCompare( s1, s2)>0;
}

//! Оператор сравнения
template< bool CaseIgnore >
inline
bool operator>=( const ConstString<CaseIgnore> &s1, const char *s2 )
{
    return stringsCompare( s1, s2)>=0;
}
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
//! Оператор сравнения
template< bool CaseIgnore >
inline
bool operator==( const char *s1, const ConstString<CaseIgnore> &s2 )
{
    return stringsCompare( s1, s2)==0;
}

//! Оператор сравнения
template< bool CaseIgnore >
inline
bool operator!=( const char *s1, const ConstString<CaseIgnore> &s2 )
{
    return stringsCompare( s1, s2)==0;
}

//! Оператор сравнения
template< bool CaseIgnore >
inline
bool operator<( const char *s1, const ConstString<CaseIgnore> &s2 )
{
    return stringsCompare( s1, s2)<0;
}

//! Оператор сравнения
template< bool CaseIgnore >
inline
bool operator<=( const char *s1, const ConstString<CaseIgnore> &s2 )
{
    return stringsCompare( s1, s2)<=0;
}

//! Оператор сравнения
template< bool CaseIgnore >
inline
bool operator>( const char *s1, const ConstString<CaseIgnore> &s2 )
{
    return stringsCompare( s1, s2)>0;
}

//! Оператор сравнения
template< bool CaseIgnore >
inline
bool operator>=( const char *s1, const ConstString<CaseIgnore> &s2 )
{
    return stringsCompare( s1, s2)>=0;
}
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
//! Оператор сравнения
template< bool CaseIgnore >
inline
bool operator==( const ConstString<CaseIgnore> &s1, const ConstString<CaseIgnore> &s2 )
{
    return stringsCompare( s1, s2)==0;
}

//! Оператор сравнения
template< bool CaseIgnore >
inline
bool operator!=( const ConstString<CaseIgnore> &s1, const ConstString<CaseIgnore> &s2 )
{
    return stringsCompare( s1, s2)==0;
}

//! Оператор сравнения
template< bool CaseIgnore >
inline
bool operator<( const ConstString<CaseIgnore> &s1, const ConstString<CaseIgnore> &s2 )
{
    return stringsCompare( s1, s2)<0;
}

//! Оператор сравнения
template< bool CaseIgnore >
inline
bool operator<=( const ConstString<CaseIgnore> &s1, const ConstString<CaseIgnore> &s2 )
{
    return stringsCompare( s1, s2)<=0;
}

//! Оператор сравнения
template< bool CaseIgnore >
inline
bool operator>( const ConstString<CaseIgnore> &s1, const ConstString<CaseIgnore> &s2 )
{
    return stringsCompare( s1, s2)>0;
}

//! Оператор сравнения
template< bool CaseIgnore >
inline
bool operator>=( const ConstString<CaseIgnore> &s1, const ConstString<CaseIgnore> &s2 )
{
    return stringsCompare( s1, s2)>=0;
}



//! Функция проверки строки на префикс
template< bool CaseIgnore >
inline
bool startsWith( const char * s, const ConstString<CaseIgnore> &prefix )
{
    return stringsCompare( prefix, s, prefix.size() )==0;
}

//! Функция проверки строки на префикс
template< bool CaseIgnore >
inline
bool startsWith( const ConstString<CaseIgnore> & s, const char * prefix )
{
    ConstString<CaseIgnore> p = prefix;
    return stringsCompare( p, s, p.size() )==0;
}

//! Функция проверки строки на префикс
template< bool CaseIgnore >
inline
bool startsWith( const ConstString<CaseIgnore> &s, const ConstString<CaseIgnore> &prefix )
{
    return stringsCompare( prefix, s, prefix.size() )==0;
}

//! Функция проверки строки на префикс
inline
bool startsWith( const char * s, const char * prefix )
{
    ConstString<false> p = prefix;
    return stringsCompare( p, s, p.size() )==0;
}






} // namespace umba

