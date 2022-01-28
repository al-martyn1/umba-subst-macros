#pragma once

//----------------------------------------------------------------------------

/*! 
    \file
    \brief Разная всячина для строк в C++ стиле
 */

//----------------------------------------------------------------------------

#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <algorithm>

#include "umba/alloca.h"
#include "umba/exception.h"
//----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// ustrp::
// umba::string_plus::
// umba::cpp::
// namespace ustrp umba::cpp;

namespace umba
{

namespace string_plus
{

//-----------------------------------------------------------------------------
/*! \ingroup UMBA_CORE
 *  \addtogroup UMBA_STRING_PLUS
 *  @{
 */
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
//! Возвращает true для букв латиницы в нижнем регистре
inline bool is_alpha_lower( char ch )
{
    if (ch>='a' && ch<='z')
        return true;
    return false;
}

//! Возвращает true для букв латиницы в верхнем регистре
inline bool is_alpha_upper( char ch )
{
    if (ch>='A' && ch<='Z')
        return true;
    return false;
}

//! Возвращает true для цифровых символов ASCII
inline bool is_digit( char ch )
{
    if (ch>='0' && ch<='9')
        return true;
    return false;
}

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
//! Возвращает true для букв латиницы в нижнем регистре
inline bool is_alpha_lower( uint8_t ch ) { return is_alpha_lower( (char)ch ); }
//! Возвращает true для букв латиницы в верхнем регистре
inline bool is_alpha_upper( uint8_t ch ) { return is_alpha_upper( (char)ch ); }
//! Возвращает true для цифровых символов ASCII
inline bool is_digit      ( uint8_t ch ) { return is_digit      ( (char)ch ); }

//! Возвращает true для букв латиницы в нижнем регистре
inline bool is_alpha_lower( wchar_t ch ) { return is_alpha_lower( (char)ch ); }
//! Возвращает true для букв латиницы в верхнем регистре
inline bool is_alpha_upper( wchar_t ch ) { return is_alpha_upper( (char)ch ); }
//! Возвращает true для цифровых символов ASCII
inline bool is_digit      ( wchar_t ch ) { return is_digit      ( (char)ch ); }

//! Возвращает true для букв латиницы
inline bool is_alpha      ( char ch )    { return is_alpha_lower(ch) || is_alpha_upper(ch); }
//! Возвращает true для букв латиницы
inline bool is_alpha      ( uint8_t ch ) { return is_alpha_lower(ch) || is_alpha_upper(ch); }
//! Возвращает true для букв латиницы
inline bool is_alpha      ( wchar_t ch ) { return is_alpha_lower(ch) || is_alpha_upper(ch); }

//! Возвращает true для символа пробела (и только для него)
inline bool is_space      ( char ch )    { return ch==' '; }
//! Возвращает true для символа пробела (и только для него)
inline bool is_space      ( uint8_t ch ) { return ch==(uint8_t)' '; }
//! Возвращает true для символа пробела (и только для него)
inline bool is_space      ( wchar_t ch ) { return ch==L' '; }

//! Возвращает true для символа пробела или таба
inline bool is_space_ot_tab( char ch )    { return ch==' ' || ch=='\t'; }
//! Возвращает true для символа пробела или таба
inline bool is_space_ot_tab( uint8_t ch ) { return is_space_ot_tab((char)ch); }
//! Возвращает true для символа пробела или таба
inline bool is_space_ot_tab( wchar_t ch ) { return is_space_ot_tab((char)ch); }

//! Возвращает true для символа пробела, таба ли какого-либо из символов перевода строки
inline bool is_whitespace ( char ch )    { return ch==' ' || ch=='\r' || ch=='\n' || ch=='\t' || ch=='\v'; }
//! Возвращает true для символа пробела, таба ли какого-либо из символов перевода строки
inline bool is_whitespace ( uint8_t ch ) { return is_whitespace((char)ch); }
//! Возвращает true для символа пробела, таба ли какого-либо из символов перевода строки
inline bool is_whitespace ( wchar_t ch ) { return is_whitespace((char)ch); }

//! Возвращает true для какого-либо из символов перевода строки
inline bool is_linefeed ( char ch )    { return ch=='\r' || ch=='\n'; }
//! Возвращает true для какого-либо из символов перевода строки
inline bool is_linefeed ( uint8_t ch ) { return is_linefeed((char)ch); }
//! Возвращает true для какого-либо из символов перевода строки
inline bool is_linefeed ( wchar_t ch ) { return is_linefeed((char)ch); }

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
//! Возвращает отрицание функционального объекта-предиката
/*! 
    \tparam ActualPred Тип предиката
    \tparam ParamType  Тип аргумента предиката
    \return Отрицание предиката
 */

template<typename ActualPred, typename ParamType>
struct not_pred
{
    const ActualPred &actualPred; //!< Ссылка на предикат для отрицания его результата
    //! Конструктор предиката отрицания
    not_pred( const ActualPred &a /*!< предикат для отрицания */ ) : actualPred(a) {}
    //! Оператор функционального объекта
    bool operator()( ParamType ch ) const { return !actualPred(ch); }
};

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
//! Возвращает true, если аргумент - пробел
template<typename CharType>
struct space_pred
{
    //! Оператор функционального объекта
    bool operator()( CharType  ch ) const { return is_space(ch); }
};

//-----------------------------------------------------------------------------
//! Возвращает true, если аргумент - пробел или таб
template<typename CharType>
struct space_or_tab_pred
{
    //! Оператор функционального объекта
    bool operator()( CharType ch )  const { return is_space_ot_tab(ch); }
};

//-----------------------------------------------------------------------------
//! Возвращает true, если аргумент - пробел, таб, втаб, или перевод строки
template<typename CharType>
struct whitespace_pred
{
    //! Оператор функционального объекта
    bool operator()( CharType ch )  const { return is_whitespace(ch); }
};

//-----------------------------------------------------------------------------
//! Возвращает true, если аргумент - перевод строки
template<typename CharType>
struct linefeed_pred
{
    //! Оператор функционального объекта
    bool operator()( CharType ch )  const { return is_linefeed(ch); }
};

//-----------------------------------------------------------------------------
//! Возвращает true, если аргумент - ASCII-символ десятичной цифры
template<typename CharType>
struct dec_digit_pred
{
    //! Оператор функционального объекта
    bool operator()( CharType ch )  const { return is_digit(ch); }
};

//-----------------------------------------------------------------------------
//! Возвращает true, если аргумент входит в строку 
// template<typename CharType>
// struct is_one_of
// {
//     const CharType *m_pstr;
//  
//     is_one_of(const CharType *pstr) : m_pstr(pstr) {}
//  
//     //! Оператор функционального объекта
//     bool operator()( CharType ch )  const 
//     {
//         const CharType *pstr = m_pstr;
//         while(*pstr++) { if (*pstr==ch) return true; }
//         return false;
//     }
// };

//-----------------------------------------------------------------------------
//! Возвращает true, если аргумент входит в строку 
template<typename CharType>
struct is_one_of
{
    typedef std::basic_string<CharType> StringType;

    StringType m_str;

    is_one_of(const StringType &str ) : m_str(str) {}
    is_one_of(const CharType   *pstr) : m_str(pstr) {}

    //! Оператор функционального объекта
    bool operator()( typename StringType::value_type ch )  const 
    {
        return m_str.find(ch)!=m_str.npos;
    }
};

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
// Trims

//-----------------------------------------------------------------------------
//! Левый inplace trim с предикатом
/*! \tparam StringType Тип входной строки (std::basic_string)
    \tparam TrimPred   Предикат. Должен возвращать true для тех символов, которые необходимо обрезать
    \param s           Входная строка
    \param pred        Экземпляр предиката
 */
template <typename StringType, typename TrimPred> inline void ltrim( StringType &s, const TrimPred &pred )
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), not_pred<TrimPred, typename StringType::value_type>(pred) ));
}

//-----------------------------------------------------------------------------
//! Правый inplace trim с предикатом
/*! \copydetails ltrim */
template <typename StringType, typename TrimPred > inline void rtrim(StringType &s, const TrimPred &pred)
{
    s.erase( std::find_if(s.rbegin(), s.rend(), not_pred<TrimPred,typename StringType::value_type>(pred) ).base(), s.end());
}

//-----------------------------------------------------------------------------
//! Двусторонний inplace trim с предикатом
/*! \copydetails ltrim */
template <typename StringType, typename TrimPred> inline void trim(StringType &s, const TrimPred &pred)
{
    ltrim(s,pred); rtrim(s,pred);
}

//-----------------------------------------------------------------------------
//! Левый inplace trim. Обрезает пробелы и табы
/*! \tparam StringType Тип входной строки (std::basic_string)
    \param s           Входная строка
 */
template <typename StringType> inline void ltrim(StringType &s)
{
    ltrim(s, whitespace_pred<typename StringType::value_type>());
}

//-----------------------------------------------------------------------------
//! Правый inplace trim. Обрезает пробелы и табы
/*! \tparam StringType Тип входной строки (std::basic_string)
 */
template <typename StringType> inline void rtrim(StringType &s)
{
    rtrim(s, whitespace_pred<typename StringType::value_type>());
}

//-----------------------------------------------------------------------------
//! Двусторонний inplace trim. Обрезает пробелы и табы
/*! \tparam StringType Тип входной строки (std::basic_string)
 */
template <typename StringType> inline void trim(StringType &s)
{
    trim(s,whitespace_pred<typename StringType::value_type>());
}

//-----------------------------------------------------------------------------
//! Левый copy trim с предикатом
/*! \tparam StringType Тип входной и результирующей строки (std::basic_string)
    \tparam TrimPred   Предикат. Должен возвращать true для тех символов, которые необходимо обрезать
    \param s           Входная строка
    \param pred        Экземпляр предиката
    \returns           Обрезанную строку
 */
template <typename StringType, typename TrimPred> inline StringType ltrim_copy(StringType s, const TrimPred &pred)
{
    ltrim(s,pred); return s;
}

//-----------------------------------------------------------------------------
//! Правый copy trim с предикатом
/*! \copydetails ltrim */
template <typename StringType, typename TrimPred> inline StringType rtrim_copy(StringType s, const TrimPred &pred)
{
    rtrim(s,pred); return s;
}

//-----------------------------------------------------------------------------
//! Двусторонний copy trim с предикатом
/*! \copydetails ltrim */
template <typename StringType, typename TrimPred> inline StringType trim_copy(StringType s, const TrimPred &pred)
{
    trim(s,pred); return s;
}

//-----------------------------------------------------------------------------
//! Левый copy trim
/*! \tparam StringType Тип входной и результирующей строки (std::basic_string)
    \param s           Входная строка
    \returns           Обрезанную строку
 */
template <typename StringType> inline StringType ltrim_copy(StringType s)
{
    return ltrim_copy(s,whitespace_pred<typename StringType::value_type>());
}

//-----------------------------------------------------------------------------
//! Правый copy trim с предикатом
/*! \tparam StringType Тип входной и результирующей строки (std::basic_string)
    \param s           Входная строка
    \returns           Обрезанную строку
 */
template <typename StringType> inline StringType rtrim_copy(StringType s)
{
    return rtrim_copy(s,whitespace_pred<typename StringType::value_type>());
}

//-----------------------------------------------------------------------------
//! Двусторонний copy trim
/*! \tparam StringType Тип входной и результирующей строки (std::basic_string)
    \param s           Входная строка
    \returns           Обрезанную строку
 */
template <typename StringType> inline StringType trim_copy(StringType s)
{
    return trim_copy(s,whitespace_pred<typename StringType::value_type>());
}

//#endif // UMBA_CXX_HAS_STD11
//-----------------------------------------------------------------------------






//-----------------------------------------------------------------------------
template <typename IteratorType, typename TrimPred> inline void cont_iter_ltrim( IteratorType b, IteratorType e, const TrimPred &pred )
{
    for(; b!=e; ++b)
        ltrim(*b,pred);
}

template <typename IteratorType, typename TrimPred > inline void cont_iter_rtrim(IteratorType b, IteratorType e, const TrimPred &pred)
{
    for(; b!=e; ++b)
        rtrim(*b,pred);
}

template <typename IteratorType, typename TrimPred> inline void cont_iter_trim(IteratorType b, IteratorType e, const TrimPred &pred)
{
    for(; b!=e; ++b)
        trim(*b,pred);
}

template <typename IteratorType> inline void cont_iter_ltrim(IteratorType b, IteratorType e)
{
    typedef typename IteratorType::value_type                           StringType;
    typedef          whitespace_pred<typename StringType::value_type>   TrimPred;
    cont_iter_ltrim<IteratorType,TrimPred>(b,e,TrimPred());
}

template <typename IteratorType> inline void cont_iter_rtrim(IteratorType b, IteratorType e)
{
    typedef typename IteratorType::value_type                           StringType;
    typedef          whitespace_pred<typename StringType::value_type>   TrimPred;
    cont_iter_rtrim<IteratorType,TrimPred>(b,e,TrimPred());
}

template <typename IteratorType> inline void  cont_iter_trim(IteratorType b, IteratorType e)
{
    typedef typename IteratorType::value_type                           StringType;
    typedef          whitespace_pred<typename StringType::value_type>   TrimPred;
    cont_iter_trim <IteratorType,TrimPred>(b,e,TrimPred());
}

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
template <typename ContainerType, typename TrimPred> inline void cont_ltrim( ContainerType &c, const TrimPred &pred )
{
    cont_iter_ltrim(c.begin(), c.end(), pred);
}

template <typename ContainerType, typename TrimPred > inline void cont_rtrim(ContainerType &c, const TrimPred &pred)
{
    cont_iter_rtrim(c.begin(), c.end(), pred);
}

template <typename ContainerType, typename TrimPred> inline void cont_trim(ContainerType &c, const TrimPred &pred)
{
    cont_iter_trim(c.begin(), c.end(), pred);
}

template <typename ContainerType> inline void cont_ltrim(ContainerType &c)
{
    typedef typename ContainerType::value_type                          StringType;
    typedef          whitespace_pred<typename StringType::value_type>   TrimPred;
    cont_ltrim<ContainerType,TrimPred>(c,TrimPred());
}

template <typename ContainerType> inline void cont_rtrim(ContainerType &c)
{
    typedef typename ContainerType::value_type                          StringType;
    typedef          whitespace_pred<typename StringType::value_type>   TrimPred;
    cont_rtrim<ContainerType,TrimPred>(c,TrimPred());
}

template <typename ContainerType> inline void  cont_trim(ContainerType &c)
{
    typedef typename ContainerType::value_type                          StringType;
    typedef          whitespace_pred<typename StringType::value_type>   TrimPred;
    cont_trim<ContainerType,TrimPred>(c,TrimPred());
}

template <typename ContainerType, typename TrimPred> inline ContainerType cont_ltrim_copy(ContainerType c, const TrimPred &pred)
{
    ContainerType res = c; cont_ltrim( res, pred ); return res;
}

template <typename ContainerType, typename TrimPred> inline ContainerType cont_rtrim_copy(ContainerType c, const TrimPred &pred)
{
    ContainerType res = c; cont_rtrim( res, pred ); return res;
}

template <typename ContainerType, typename TrimPred> inline ContainerType  cont_trim_copy(ContainerType c, const TrimPred &pred)
{
    ContainerType res = c; cont_trim( res, pred ); return res;
}

template <typename ContainerType> inline ContainerType cont_ltrim_copy(ContainerType c)
{
    ContainerType res = c; cont_ltrim( res ); return res;
}

template <typename ContainerType> inline ContainerType cont_rtrim_copy(ContainerType c)
{
    ContainerType res = c; cont_rtrim( res ); return res;
}

template <typename ContainerType> inline ContainerType  cont_trim_copy(ContainerType c)
{
    ContainerType res = c; cont_trim( res ); return res;
}

//-----------------------------------------------------------------------------



                                                                   

//-----------------------------------------------------------------------------
// Starts/Ends

//-----------------------------------------------------------------------------
//! Возвращает true, если строка начинается с указанного префикса (StringType)
/*! \tparam StringType Тип входной и результирующей строки (std::basic_string)
    \param str         Входная строка
    \param prefix      Префикс, есть перегрузка для const StringType::value_type*
    \returns           true, если строка начинается с указанного префикса
 */
template <typename StringType> inline bool starts_with( const StringType &str, const StringType &prefix )
{
    if (str.size()<prefix.size())
        return false;

    return str.compare( 0, prefix.size(), prefix )==0;
}

//-----------------------------------------------------------------------------
//! Возвращает true, если строка начинается с указанного префикса (StringType::value_type*)
/*! \copydetails starts_with */
template <typename StringType> inline bool starts_with( const StringType &str, const typename StringType::value_type *prefix )
{
    return starts_with(str, StringType(prefix));
}

//-----------------------------------------------------------------------------
//! Возвращает true, если строка начинается с указанного префикса (StringType), удаляя префикс
/*! \copydetails starts_with */
template <typename StringType> inline bool starts_with_and_strip( StringType &str, const StringType &prefix )
{
    if (!starts_with( str, prefix ))
        return false;

    str.erase( 0, prefix.size() );

    return true;
}

//-----------------------------------------------------------------------------
//! Возвращает true, если строка начинается с указанного префикса (StringType::value_type*), удаляя префикс
/*! \copydetails starts_with */
template <typename StringType> inline bool starts_with_and_strip( StringType &str, const typename StringType::value_type *prefix )
{
    return starts_with_and_strip(str, StringType(prefix));
}

//-----------------------------------------------------------------------------
//! Возвращает true, если строка заканчивается на указанный постфикс (StringType)
/*! \tparam StringType Тип входной и результирующей строки (std::basic_string)
    \param str         Входная строка
    \param postfix     Постфикс (суффикс), есть перегрузка для const StringType::value_type*
    \returns           true, если строка начинается с указанного постфикса
 */
template <typename StringType> inline bool ends_with( const StringType &str, const StringType &postfix )
{
    if (str.size()<postfix.size())
        return false;

    return str.compare( str.size()-postfix.size(), postfix.size(), postfix )==0;
}

//-----------------------------------------------------------------------------
//! Возвращает true, если строка заканчивается на указанный постфикс (StringType::value_type*)
/*! \copydetails ends_with */
template <typename StringType> inline bool ends_with( const StringType &str, const typename StringType::value_type *postfix )
{
    return ends_with(str, StringType(postfix));
}

//-----------------------------------------------------------------------------
//! Возвращает true, если строка заканчивается на указанный постфикс (StringType), удаляя постфикс
/*! \copydetails ends_with */
template <typename StringType>  inline bool ends_with_and_strip( StringType &str, const StringType &postfix )
{
    if (!ends_with( str, postfix ))
        return false;

    str.erase( str.size()-postfix.size(), postfix.size() );

    return true;
}

//-----------------------------------------------------------------------------
//! Возвращает true, если строка заканчивается на указанный постфикс (StringType::value_type*), удаляя постфикс
/*! \copydetails ends_with */
template <typename StringType> inline bool ends_with_and_strip( StringType &str, const typename StringType::value_type *postfix )
{
    return ends_with_and_strip(str, StringType(postfix));
}

//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
// Case conversion

//-----------------------------------------------------------------------------
//! Конвертирует строку StringType в нижний регистр (работает только для базового диапазона ASCII)
template <typename StringType> inline void tolower( StringType &str )
{
    for(typename StringType::iterator it = str.begin(); it != str.end(); ++it)
        *it = (typename StringType::value_type)std::tolower((char)*it);
}

//-----------------------------------------------------------------------------
//! Конвертирует строку StringType в верхний регистр (работает только для базового диапазона ASCII)
template <typename StringType> inline void toupper( StringType &str )
{
    for(typename StringType::iterator it = str.begin(); it != str.end(); ++it)
        *it = (typename StringType::value_type)std::toupper((char)*it);
}

//-----------------------------------------------------------------------------
//! Не меняет регистр символов
template <typename StringType> inline void tosamecase( StringType &str ) { }

//-----------------------------------------------------------------------------
//! Конвертирует строку StringType в нижний регистр (работает только для базового диапазона ASCII), возвращая модифицированную копию
template <typename StringType> inline StringType tolower_copy( const StringType &str )
{
    StringType res = str;
    tolower(res);
    return res;
}

//-----------------------------------------------------------------------------
//! Конвертирует строку StringType в верхний регистр (работает только для базового диапазона ASCII), возвращая модифицированную копию
template <typename StringType> inline StringType toupper_copy( const StringType &str )
{
    StringType res = str;
    toupper(res);
    return res;
}

//-----------------------------------------------------------------------------
//! Не меняет регистр символов, возвращая копию входного аргумента
template <typename StringType> inline StringType tosamecase_copy( const StringType &str )
{
    return str;
}

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
//! Создаёт строку StringType из std::wstring (работает только для базового диапазона ASCII).
template<typename StringType> inline StringType make_string( const std::wstring &str )
{
    StringType res;
    for( std::wstring::const_iterator it = str.begin(); it != str.end(); ++it)
        res.append(1, (typename StringType::value_type)*it );
    return res;
}

//-----------------------------------------------------------------------------
//! Создаёт строку StringType из std::string (работает только для базового диапазона ASCII).
template<typename StringType> inline StringType make_string( const std::string &str )
{
    StringType res;
    for( std::string::const_iterator it = str.begin(); it != str.end(); ++it)
        res.append(1, (typename StringType::value_type)*it );
    return res;
}

//-----------------------------------------------------------------------------
//! Создаёт строку StringType из const wchar_t* (работает только для базового диапазона ASCII).
template<typename StringType> inline StringType make_string( const wchar_t *str )
{
    StringType res;
    for(; *str; str++)
        res.append(1, (typename StringType::value_type)*str );
    return res;
}

//-----------------------------------------------------------------------------
//! Создаёт строку StringType из const char* (работает только для базового диапазона ASCII).
template<typename StringType> inline StringType make_string( const char *str )
{
    StringType res;
    for(; *str; str++)
        res.append(1, (typename StringType::value_type)*str );
    return res;
}

//-----------------------------------------------------------------------------
//! Создаёт строку StringType из sz wchar_t'ов (работает только для базового диапазона ASCII).
template<typename StringType> inline StringType make_string( wchar_t ch, size_t sz = 1 )
{
    StringType res;
    res.append(sz, (typename StringType::value_type)ch );
    return res;
}

//-----------------------------------------------------------------------------
//! Создаёт строку StringType из sz char'ов (работает только для базового диапазона ASCII).
template<typename StringType> inline StringType make_string( char ch, size_t sz = 1 )
{
    StringType res;
    res.append(sz, (typename StringType::value_type)ch );
    return res;
}

//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
//! "Сжатие" строки. Несколько последовательно повторяющихся символов склеиваются в один
template<typename StringType> inline
StringType textCompress( const StringType &text, const StringType &compressChars )
{
    StringType res;

    typename StringType::size_type i = 0, sz = text.size();

    typename StringType::value_type prevWs = 0;

    for(; i!=sz; ++i)
    {
        typename StringType::value_type ch = text[i];
        typename StringType::size_type chPos = compressChars.find(ch);
        //if (ch==' ' || ch=='\t')
        if (chPos!=compressChars.npos)
        {
            if (ch==prevWs)
            {
                // simple skip
            }
            else
            {
                prevWs = ch;
                res.append(1,ch);
            }
        }
        else
        {
            prevWs = 0;
            res.append(1,ch);
        }
    }

    return res;
}

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
// Split/Merge


//-----------------------------------------------------------------------------
//! Производит слияние строковых представлений объектов в одну строку
/*! 
    \tparam StringType  Тип результирующей строки (std::basic_string)
    \tparam Iter        Тип итератора по объектам
    \tparam ToStringObj Тип преобразователя в строку. operator( *Iter ) должен возвращать значение типа StringType 

    \param b            Итератор начала последовательности
    \param e            Итератор конца последовательности
    \param delim        Разделитель элементов в результирующей строке. Перегружен для std::string, std::wstring, const char*, const wchar_t*, char, wchar_t
    \param stringifier  Экземпляр объекта-преобразователя в строку

    \return Результат слияния строковых представлений объектов
 */
template<typename StringType, typename Iter, typename ToStringObj>
inline
StringType merge( Iter b, Iter e, const std::string &delim, const ToStringObj &stringifier )
{
    StringType _delim = make_string<StringType>(delim);
    StringType res;
    for( ; b!=e; ++b )
    {
        if (!res.empty()) res.append(_delim);
        res.append(stringifier(*b));
    }
    return res;
}

//------------------------------
//! Производит слияние строковых представлений объектов в одну строку
/*! 
    \tparam StringType  Тип результирующей строки (std::basic_string)
    \tparam Iter        Тип итератора по объектам
    \tparam ToStringObj Тип преобразователя в строку. operator( *Iter ) должен возвращать значение типа StringType 

    \param b            Итератор начала последовательности
    \param e            Итератор конца последовательности
    \param delim        Разделитель элементов в результирующей строке. Перегружен для std::string, std::wstring, const char*, const wchar_t*, char, wchar_t
    \param stringifier  Экземпляр объекта-преобразователя в строку

    \return Результат слияния строковых представлений объектов
 */
template<typename StringType, typename Iter, typename ToStringObj>
inline
StringType merge( Iter b , Iter e, const std::wstring &delim, const ToStringObj &stringifier )
{
    StringType _delim = make_string<StringType>(delim);
    StringType res;
    for( ; b!=e; ++b )
    {
        if (!res.empty()) res.append(_delim);
        res.append(stringifier(*b));
    }
    return res;
}

//------------------------------
//! Производит слияние строковых представлений объектов в одну строку
/*! 
    \tparam StringType  Тип результирующей строки (std::basic_string)
    \tparam Iter        Тип итератора по объектам
    \tparam ToStringObj Тип преобразователя в строку. operator( *Iter ) должен возвращать значение типа StringType 

    \param b            Итератор начала последовательности
    \param e            Итератор конца последовательности
    \param delim        Разделитель элементов в результирующей строке. Перегружен для std::string, std::wstring, const char*, const wchar_t*, char, wchar_t
    \param stringifier  Экземпляр объекта-преобразователя в строку

    \return Результат слияния строковых представлений объектов
 */
template<typename StringType, typename Iter, typename ToStringObj> inline
StringType merge( Iter b, Iter e, char delim, const ToStringObj &stringifier )
{
    return merge<StringType>( b, e, make_string<StringType>(delim,1), stringifier );
}

//------------------------------
//! Производит слияние строковых представлений объектов в одну строку
/*! 
    \tparam StringType  Тип результирующей строки (std::basic_string)
    \tparam Iter        Тип итератора по объектам
    \tparam ToStringObj Тип преобразователя в строку. operator( *Iter ) должен возвращать значение типа StringType 

    \param b            Итератор начала последовательности
    \param e            Итератор конца последовательности
    \param delim        Разделитель элементов в результирующей строке. Перегружен для std::string, std::wstring, const char*, const wchar_t*, char, wchar_t
    \param stringifier  Экземпляр объекта-преобразователя в строку

    \return Результат слияния строковых представлений объектов
 */
template<typename StringType, typename Iter, typename ToStringObj> inline
StringType merge( Iter b, Iter e, const char *delim, const ToStringObj &stringifier )
{
    return merge<StringType>( b, e, make_string<StringType>(delim), stringifier );
}

//-----------------------------------------------------------------------------
//! Производит слияние строковых представлений объектов в одну строку
/*! 
    \tparam StringType  Тип результирующей строки (std::basic_string)
    \tparam Iter        Тип итератора по объектам
    \tparam ToStringObj Тип преобразователя в строку. operator( *Iter ) должен возвращать значение типа StringType 

    \param b            Итератор начала последовательности
    \param e            Итератор конца последовательности
    \param delim        Разделитель элементов в результирующей строке. Перегружен для std::string, std::wstring, const char*, const wchar_t*, char, wchar_t
    \param stringifier  Экземпляр объекта-преобразователя в строку

    \return Результат слияния строковых представлений объектов
 */
template<typename StringType, typename Iter, typename ToStringObj> inline
StringType merge( Iter b, Iter e, wchar_t delim, const ToStringObj &stringifier )
{
    return merge<StringType>( b, e, make_string<StringType>(delim,1), stringifier );
}

//------------------------------
//! Производит слияние строковых представлений объектов в одну строку
/*! 
    \tparam StringType  Тип результирующей строки (std::basic_string)
    \tparam Iter        Тип итератора по объектам
    \tparam ToStringObj Тип преобразователя в строку. operator( *Iter ) должен возвращать значение типа StringType 

    \param b            Итератор начала последовательности
    \param e            Итератор конца последовательности
    \param delim        Разделитель элементов в результирующей строке. Перегружен для std::string, std::wstring, const char*, const wchar_t*, char, wchar_t
    \param stringifier  Экземпляр объекта-преобразователя в строку

    \return Результат слияния строковых представлений объектов
 */
template<typename StringType, typename Iter, typename ToStringObj> inline
StringType merge( Iter b, Iter e, const wchar_t *delim, const ToStringObj &stringifier )
{
    return merge<StringType>( b, e, make_string<StringType>(delim), stringifier );
}

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
//! Производит слияние строковых представлений объектов в одну строку
/*! 
    \tparam StringType  Тип результирующей строки (std::basic_string)
    \tparam ItemType    Тип элемента/объекта
    \tparam ToStringObj Тип преобразователя в строку. operator( *Iter ) должен возвращать значение типа StringType 

    \param tokens       Вектор элементов для склеивания в строку
    \param delim        Разделитель элементов в результирующей строке. Перегружен для std::string, std::wstring, const char*, const wchar_t*, char, wchar_t
    \param stringifier  Экземпляр объекта-преобразователя в строку

    \return Результат слияния строковых представлений объектов
 */
template<typename StringType, typename ItemType, typename ToStringObj> inline
std::string merge( const std::vector<ItemType> &tokens, char delim, const ToStringObj &stringifier )
{
    return merge<StringType>( tokens.begin(), tokens.end(), delim, stringifier );
}

//------------------------------
//! Производит слияние строковых представлений объектов в одну строку
/*! 
    \tparam StringType  Тип результирующей строки (std::basic_string)
    \tparam ItemType    Тип элемента/объекта
    \tparam ToStringObj Тип преобразователя в строку. operator( *Iter ) должен возвращать значение типа StringType 

    \param tokens       Вектор элементов для склеивания в строку
    \param delim        Разделитель элементов в результирующей строке. Перегружен для std::string, std::wstring, const char*, const wchar_t*, char, wchar_t
    \param stringifier  Экземпляр объекта-преобразователя в строку

    \return Результат слияния строковых представлений объектов
 */
template<typename StringType, typename ItemType, typename ToStringObj> inline
std::string merge( const std::vector<ItemType> &tokens, const char *delim, const ToStringObj &stringifier )
{
    return merge<StringType>( tokens.begin(), tokens.end(), delim, stringifier );
}

//------------------------------
//! Производит слияние строковых представлений объектов в одну строку
/*! 
    \tparam StringType  Тип результирующей строки (std::basic_string)
    \tparam ItemType    Тип элемента/объекта
    \tparam ToStringObj Тип преобразователя в строку. operator( *Iter ) должен возвращать значение типа StringType 

    \param tokens       Вектор элементов для склеивания в строку
    \param delim        Разделитель элементов в результирующей строке. Перегружен для std::string, std::wstring, const char*, const wchar_t*, char, wchar_t
    \param stringifier  Экземпляр объекта-преобразователя в строку

    \return Результат слияния строковых представлений объектов
 */
template<typename StringType, typename ItemType, typename ToStringObj> inline
std::string merge( const std::vector<ItemType> &tokens, const std::string &delim, const ToStringObj &stringifier )
{
    return merge<StringType>( tokens.begin(), tokens.end(), delim, stringifier );
}

//------------------------------
//! Производит слияние строковых представлений объектов в одну строку
/*! 
    \tparam StringType  Тип результирующей строки (std::basic_string)
    \tparam ItemType    Тип элемента/объекта
    \tparam ToStringObj Тип преобразователя в строку. operator( *Iter ) должен возвращать значение типа StringType 

    \param tokens       Вектор элементов для склеивания в строку
    \param delim        Разделитель элементов в результирующей строке. Перегружен для std::string, std::wstring, const char*, const wchar_t*, char, wchar_t
    \param stringifier  Экземпляр объекта-преобразователя в строку

    \return Результат слияния строковых представлений объектов
 */
template<typename StringType, typename ItemType, typename ToStringObj> inline
std::string merge( const std::vector<ItemType> &tokens, wchar_t delim, const ToStringObj &stringifier )
{
    return merge<StringType>( tokens.begin(), tokens.end(), delim, stringifier );
}

//------------------------------
//! Производит слияние строковых представлений объектов в одну строку
/*! 
    \tparam StringType  Тип результирующей строки (std::basic_string)
    \tparam ItemType    Тип элемента/объекта
    \tparam ToStringObj Тип преобразователя в строку. operator( *Iter ) должен возвращать значение типа StringType 

    \param tokens       Вектор элементов для склеивания в строку
    \param delim        Разделитель элементов в результирующей строке. Перегружен для std::string, std::wstring, const char*, const wchar_t*, char, wchar_t
    \param stringifier  Экземпляр объекта-преобразователя в строку

    \return Результат слияния строковых представлений объектов
 */
template<typename StringType, typename ItemType, typename ToStringObj> inline
std::string merge( const std::vector<ItemType> &tokens, const wchar_t *delim, const ToStringObj &stringifier )
{
    return merge<StringType>( tokens.begin(), tokens.end(), delim, stringifier );
}

//------------------------------
//! Производит слияние строковых представлений объектов в одну строку
/*! 
    \tparam StringType  Тип результирующей строки (std::basic_string)
    \tparam ItemType    Тип элемента/объекта
    \tparam ToStringObj Тип преобразователя в строку. operator( *Iter ) должен возвращать значение типа StringType 

    \param tokens       Вектор элементов для склеивания в строку
    \param delim        Разделитель элементов в результирующей строке. Перегружен для std::string, std::wstring, const char*, const wchar_t*, char, wchar_t
    \param stringifier  Экземпляр объекта-преобразователя в строку

    \return Результат слияния строковых представлений объектов
 */
template<typename StringType, typename ItemType, typename ToStringObj> inline
std::string merge( const std::vector<ItemType> &tokens, const std::wstring &delim, const ToStringObj &stringifier )
{
    return merge<StringType>( tokens.begin(), tokens.end(), delim, stringifier );
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
//! Просто возвращает свой аргумент. Типа вместо лямбды
template <typename T>
struct lambda_return_arg
{
    //! Возвращает свой аргумент
    T operator()( const T &t ) const {  return t; }
};

//------------------------------
//! Возвращает true, если объект empty(). Типа вместо лямбды
template <typename T>
struct lambda_is_empty
{
    //! Возвращает результат вызова empty() для своего аргумента
    bool operator()( const T &t ) const  { return t.empty(); }
};

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
//! Производит слияние строк в одну строку
/*! 
    \tparam StringType  Тип результирующей строки (std::basic_string)
    \tparam Iter        Тип итератора по объектам

    \param b            Итератор начала последовательности строк
    \param e            Итератор конца последовательности строк
    \param delim        Разделитель элементов в результирующей строке. Перегружен для std::string, std::wstring, const char*, const wchar_t*, char, wchar_t

    \return Результат слияния строк
 */
template<typename StringType, typename Iter> inline
StringType merge( Iter b, Iter e, char delim )
{
    return merge<StringType>( b, e, delim, lambda_return_arg<StringType>() );
}

//------------------------------
//! Производит слияние строк в одну строку
/*! 
    \tparam StringType  Тип результирующей строки (std::basic_string)
    \tparam Iter        Тип итератора по объектам

    \param b            Итератор начала последовательности строк
    \param e            Итератор конца последовательности строк
    \param delim        Разделитель элементов в результирующей строке. Перегружен для std::string, std::wstring, const char*, const wchar_t*, char, wchar_t

    \return Результат слияния строк
 */
template<typename StringType, typename Iter> inline
StringType merge( Iter b, Iter e, const char *delim )
{
    return merge<StringType>( b, e, delim, lambda_return_arg<StringType>() );
}

//------------------------------
//! Производит слияние строк в одну строку
/*! 
    \tparam StringType  Тип результирующей строки (std::basic_string)
    \tparam Iter        Тип итератора по объектам

    \param b            Итератор начала последовательности строк
    \param e            Итератор конца последовательности строк
    \param delim        Разделитель элементов в результирующей строке. Перегружен для std::string, std::wstring, const char*, const wchar_t*, char, wchar_t

    \return Результат слияния строк
 */
template<typename StringType, typename Iter> inline
StringType merge( Iter b, Iter e, const std::string &delim )
{
    return merge<StringType>( b, e, delim, lambda_return_arg<StringType>() );
}

//----------------------------------------------------------------------------
//! Производит слияние строк в одну строку
/*! 
    \tparam StringType  Тип результирующей строки (std::basic_string)
    \tparam Iter        Тип итератора по объектам

    \param b            Итератор начала последовательности строк
    \param e            Итератор конца последовательности строк
    \param delim        Разделитель элементов в результирующей строке. Перегружен для std::string, std::wstring, const char*, const wchar_t*, char, wchar_t

    \return Результат слияния строк
 */
template<typename StringType, typename Iter> inline
StringType merge( Iter b, Iter e, wchar_t delim )
{
    return merge<StringType>( b, e, delim, lambda_return_arg<StringType>() );
}

//------------------------------
//! Производит слияние строк в одну строку
/*! 
    \tparam StringType  Тип результирующей строки (std::basic_string)
    \tparam Iter        Тип итератора по объектам

    \param b            Итератор начала последовательности строк
    \param e            Итератор конца последовательности строк
    \param delim        Разделитель элементов в результирующей строке. Перегружен для std::string, std::wstring, const char*, const wchar_t*, char, wchar_t

    \return Результат слияния строк
 */
template<typename StringType, typename Iter> inline
StringType merge( Iter b, Iter e, const wchar_t *delim )
{
    return merge<StringType>( b, e, delim, lambda_return_arg<StringType>() );
}

//------------------------------
//! Производит слияние строк в одну строку
/*! 
    \tparam StringType  Тип результирующей строки (std::basic_string)
    \tparam Iter        Тип итератора по объектам

    \param b            Итератор начала последовательности строк
    \param e            Итератор конца последовательности строк
    \param delim        Разделитель элементов в результирующей строке. Перегружен для std::string, std::wstring, const char*, const wchar_t*, char, wchar_t

    \return Результат слияния строк
 */
template<typename StringType, typename Iter> inline
StringType merge( Iter b, Iter e, const std::wstring &delim )
{
    return merge<StringType>( b, e, delim, lambda_return_arg<StringType>() );
}

//----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
//! Производит слияние строк в одну строку
/*! 
    \tparam StringType  Тип результирующей строки (std::basic_string)

    \param tokens       Вектор элементов для склеивания в строку
    \param delim        Разделитель элементов в результирующей строке. Перегружен для std::string, std::wstring, const char*, const wchar_t*, char, wchar_t

    \return Результат слияния строк
 */
template<typename StringType> inline
StringType merge( const std::vector<StringType> &tokens, char delim )
{
    return merge<StringType>( tokens.begin(), tokens.end(), delim, lambda_return_arg<StringType>() );
}

//------------------------------
//! Производит слияние строк в одну строку
/*! 
    \tparam StringType  Тип результирующей строки (std::basic_string)

    \param tokens       Вектор элементов для склеивания в строку
    \param delim        Разделитель элементов в результирующей строке. Перегружен для std::string, std::wstring, const char*, const wchar_t*, char, wchar_t

    \return Результат слияния строк
 */
template<typename StringType> inline
StringType merge( const std::vector<std::string> &tokens, const char *delim )
{
    return merge<StringType>( tokens.begin(), tokens.end(), delim, lambda_return_arg<StringType>() );
}

//------------------------------
//! Производит слияние строк в одну строку
/*! 
    \tparam StringType  Тип результирующей строки (std::basic_string)

    \param tokens       Вектор элементов для склеивания в строку
    \param delim        Разделитель элементов в результирующей строке. Перегружен для std::string, std::wstring, const char*, const wchar_t*, char, wchar_t

    \return Результат слияния строк
 */
template<typename StringType> inline
StringType merge( const std::vector<std::string> &tokens, const std::string &delim )
{
    return merge<StringType>( tokens.begin(), tokens.end(), delim, lambda_return_arg<StringType>() );
}

//----------------------------------------------------------------------------
//! Производит слияние строк в одну строку
/*! 
    \tparam StringType  Тип результирующей строки (std::basic_string)

    \param tokens       Вектор элементов для склеивания в строку
    \param delim        Разделитель элементов в результирующей строке. Перегружен для std::string, std::wstring, const char*, const wchar_t*, char, wchar_t

    \return Результат слияния строк
 */
template<typename StringType> inline
StringType merge( const std::vector<StringType> &tokens, wchar_t delim )
{
    return merge<StringType>( tokens.begin(), tokens.end(), delim, lambda_return_arg<StringType>() );
}

//------------------------------
//! Производит слияние строк в одну строку
/*! 
    \tparam StringType  Тип результирующей строки (std::basic_string)

    \param tokens       Вектор элементов для склеивания в строку
    \param delim        Разделитель элементов в результирующей строке. Перегружен для std::string, std::wstring, const char*, const wchar_t*, char, wchar_t

    \return Результат слияния строк
 */
template<typename StringType> inline
StringType merge( const std::vector<std::string> &tokens, const wchar_t *delim )
{
    return merge<StringType>( tokens.begin(), tokens.end(), delim, lambda_return_arg<StringType>() );
}

//------------------------------
//! Производит слияние строк в одну строку
/*! 
    \tparam StringType  Тип результирующей строки (std::basic_string)

    \param tokens       Вектор элементов для склеивания в строку
    \param delim        Разделитель элементов в результирующей строке. Перегружен для std::string, std::wstring, const char*, const wchar_t*, char, wchar_t

    \return Результат слияния строк
 */
template<typename StringType> inline
StringType merge( const std::vector<std::string> &tokens, const std::wstring &delim )
{
    return merge<StringType>( tokens.begin(), tokens.end(), delim, lambda_return_arg<StringType>() );
}

//----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
// Misc string algorithms 


//-----------------------------------------------------------------------------
//! Расширение строки до нужной длины
template<typename StringType> inline
void expand( StringType &str                                                            //!< Расширяемая строка
           , typename StringType::size_type sz                                          //!< Требуемый размер
           , typename StringType::value_type ch = (typename StringType::value_type)' '  //!< Символ заполнения
           )
{
    if (str.size()>=sz)
        return;

    typename StringType::size_type numCharsToAdd = sz - str.size();
    str.append( numCharsToAdd, ch );
}

//------------------------------
//! Расширение копии строки до нужной длины. \returns Расширеную строку.
template<typename StringType> inline
StringType expand_copy( const StringType &str                                                      //!< Расширяемая строка
                      , typename StringType::size_type sz                                          //!< Требуемый размер
                      , typename StringType::value_type ch = (typename StringType::value_type)' '  //!< Символ заполнения
                      )
{
    std::string res = str;
    expand(res, sz, ch);
    return res;
}

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
// Brace utils


//-----------------------------------------------------------------------------
//! Хелперы для разного скобочного
namespace ascii_brace
{


//------------------------------
//! Возвращает парный символ для своего аргумента (только из диапазона ASCII)
inline char get_pair( char brCh )
{
    switch(brCh)
    {
        case '<' : return '>' ;
        case '>' : return '<' ;

        case '[' : return ']' ;
        case ']' : return '[' ;

        case '{' : return '}' ;
        case '}' : return '{' ;

        case '(' : return ')' ;
        case ')' : return '(' ;

        case '\'': return '\'';
        case '\"': return '\"';

        case '%': return '%';
        case '$': return '$';

        case '@': return '@';
        case '#': return '#';

        case '_': return '_';

        default  : return 0;
    };
}

//------------------------------
//! Возвращает парный символ для своего аргумента (только из диапазона ASCII)
inline wchar_t get_pair( wchar_t brCh )  { return get_pair((char)brCh); }

//------------------------------
//! Возвращает true, если входной символ - скобка (только из диапазона ASCII)
template<typename CharType> inline CharType is_brace( CharType brCh )
{
    CharType pr = get_pair(brCh);
    return pr != 0;
}

//------------------------------
//! Сравнивает два символа. Возвращает -1, 0, 1. Вообще-то предполагалось для скобок, но работает для любых символов. Если результат меньше нуля, то br1 - открывающая скобка
template<typename CharType> inline int compare( CharType br1, CharType br2 )
{
    if (br1==br2) return 0;
    else return br1<br2 ? -1 : 1;
}

//------------------------------
//! Возвращает true, если переданный символ - парный (один из видов скобки) или апостроф, кавычка, или %, $, @, # - они тоже считаются парными
template<typename CharType> inline bool is_paired    ( CharType brCh ) { return compare ( brCh  , get_pair(brCh) ) == 0; }
//! Возвращает true, если символ - открывающий - левый
template<typename CharType> inline bool is_left      ( CharType brCh ) { return compare ( brCh  , get_pair(brCh) )  < 0; }
//! Возвращает true, если символ - закрывающий - правый
template<typename CharType> inline bool is_right     ( CharType brCh ) { return compare ( brCh  , get_pair(brCh) )  > 0; }
//! Возвращает парный левый символ
template<typename CharType> inline CharType get_left ( CharType brCh ) { return is_left ( brCh) ? brCh : get_pair(brCh); }
//! Возвращает парный левый символ
template<typename CharType> inline CharType get_right( CharType brCh ) { return is_right( brCh) ? brCh : get_pair(brCh);}

//------------------------------




//------------------------------
//! Создаёт наборы левых и правых скобок для входного набора, в котором можно задавать как левые, так и правые варианты. На входе должны быть только символы, имеющие пару. Типа нормализация
template<typename StringType> inline
void build_pairs( const typename StringType::value_type *braces, StringType *pLefts = 0, StringType *pRights = 0)
{
    typedef typename StringType::value_type CharType;
    //lefts .clear(); lefts .reserve(8);
    //rights.clear(); rights.reserve(8);
    if (pLefts==0 && pRights==0)
        return;

    StringType lefts, rights;
    lefts .reserve(8);
    rights.reserve(8);

    while(*braces)
    {
        CharType b = *braces;

        if (is_left(b)==is_right(b))
            throw std::runtime_error("umba::cpp::ascii_brace::build_pairs (1): paired brace can't be left and right at the same time");

        if (!is_paired(b))
            throw std::runtime_error("umba::cpp::ascii_brace::build_pairs (2): can't build pair for non-paired brace");

        bool isLeft = is_left(b);

        CharType lb =  isLeft ? b : get_pair(b); // if left we keep, else we got the pair
        CharType rb = !isLeft ? b : get_pair(b); // if not left we got the pair, else - keep

        lefts .append(1,lb);
        rights.append(1,rb);

        ++braces;
    }

    if (pLefts ) *pLefts  = lefts ;
    if (pRights) *pRights = rights;
}

//------------------------------
//! Создаёт наборы левых и правых скобок для входного набора, в котором можно задавать как левые, так и правые варианты. На входе должны быть только символы, имеющие пару. Типа нормализация
template<typename StringType> inline
void build_pairs( const StringType &str, StringType *pLefts = 0, StringType *pRights = 0)
{
    build_pairs(str.c_str(), pLefts, pRights);
}

//------------------------------
//! Создаёт наборы левых и правых скобок для входного набора, в котором можно задавать как левые, так и правые варианты. На входе должны быть только символы, имеющие пару. Типа нормализация
template<typename StringType> inline
void build_pairs( const StringType &braces, StringType &lefts, StringType &rights )
{
    build_pairs( braces.c_str(), &lefts, &rights );
}

//------------------------------
//! Утилита. Пробегаем последовательно по вектору счётчиков, сравниваем с параметром, возвращаем true, только если все значения равны заданному параметру.
/*! 
    \tparam CounterType Тип счётчика

    \param cntVec       Вектор счётчиков
    \param val          Величина для сравнения

    \return             Возвращает true, только если все значения равны заданному параметру
 */
template<typename CounterType>
inline bool util_check_all_exact( const std::vector<CounterType> &cntVec, CounterType val )
{
    typename std::vector<CounterType>::const_iterator cit = cntVec.begin();
    for(; cit != cntVec.end();  ++cit)
    {
        if (*cit!=val)
            return false;
    }
    return true;
}

//------------------------------
//! Удаление внешних скобок с учетом вложенности внутренних
/*! \returns Разбитый по скобкам набор строк */
template<typename StringType> inline
std::vector<StringType>
simple_unbrace( const StringType &str                          //!< Строка со скобками
              , const StringType &braces                       //!< Список допустимых/скобок
              , bool              keepStartingBrace = false    //!< Оставить ли открывающую скобку (как маркер)
              )
{
    typedef typename StringType::value_type CharType;

    std::vector<StringType> res;

    StringType leftBraces, rightBraces;
    build_pairs( braces, leftBraces, rightBraces );

    std::vector< CharType > bracesStack;
    //bracesStack.reserve(8);

    std::vector<int> nestingCounts( leftBraces.size(), (int)0 );

    typename StringType::size_type startPos = 0, pos = 0, size = str.size();

    while(pos!=size) // ; ++pos
    {
        CharType ch = str[pos];
        typename StringType::size_type lIdx = leftBraces .find( ch );
        typename StringType::size_type rIdx = rightBraces.find( ch );

        if (lIdx==StringType::npos && rIdx==StringType::npos)
        {
            ++pos;
            continue; // not an opening or closing brace
        }

        // Found opening or closing brace
        bool nonPairedBraceOpen = true;
        if (lIdx!=StringType::npos && rIdx!=StringType::npos)
        {
            if (!bracesStack.empty() && bracesStack.back()==ch)
                nonPairedBraceOpen = false;
        }

        if (lIdx!=StringType::npos && nonPairedBraceOpen)
        {
            // Opening brace
            bool pushedVal = false;
            if (startPos!=pos /*&& util_check_all_exact( nestingCounts, 0 )*/ && bracesStack.empty())
            {
                res.push_back( std::string(str, startPos, pos-startPos) );
                pushedVal = true;
            }

            nestingCounts[lIdx]++;
            ++pos;
            if (pushedVal)
                startPos = pos;

            bracesStack.push_back(ch);

            continue;
        }

        // Only closing brace here

        ++pos;
        nestingCounts[rIdx]--;

        if (!bracesStack.empty())
            bracesStack.pop_back();

        //if (util_check_all_exact( nestingCounts, 0 ))
        if (bracesStack.empty())
        {
            if (keepStartingBrace)
                res.push_back( StringType(str, startPos-1, pos-startPos) );
            else
                res.push_back( StringType(str, startPos, pos-startPos-1) );
            startPos = pos;
        }
    }

    if (startPos!=pos)
        res.push_back( StringType(str, startPos, pos-startPos) );

    return res;
}

//------------------------------
//! Удаление внешних скобок с учетом вложенности внутренних
/*! \returns Разбитый по скобкам набор строк */
template<typename StringType> inline
std::vector<StringType>
simple_unbrace( const StringType &str                          //!< Строка со скобками
              , const typename StringType::value_type *braces  //!< Список допустимых/скобок
              , bool              keepStartingBrace = false    //!< Оставить ли открывающую скобку (как маркер)
              )
{
    return simple_unbrace(str, StringType(braces), keepStartingBrace );
}


//------------------------------
//! Утилита для разбора строк по скобкам
template<typename StringType> inline
void split_against_braces_helper( std::vector< std::pair< typename StringType::size_type, typename StringType::size_type> > &nonBraceRanges //!< [out] Не заскобыченные диапазоны
                                       , const StringType        &str                                                                       //!< Входная строка
                                       , const StringType        &braces                                                                    //!< Обрабатываемые скобки
                                       , const std::string       &fileName = make_string<StringType>("")                                    //!< Имя файла?
                                       , size_t lineNumber = (size_t)-1                                                                     //!< Номер строки
                                       , size_t linePos = (size_t)-1                                                                        //!< Позиция в строке
                                       , bool strictOrder = false                                                                           //!< Строгий порядок?
                                       )
{
    typedef typename StringType::value_type CharType;

    StringType leftBraces, rightBraces;
    build_pairs( braces, &leftBraces, &rightBraces );

    std::vector< CharType > bracesStack;

    std::vector<int> nestingCounts( leftBraces.size(), (int)0 );

    typename StringType::size_type startPos = 0, pos = 0, size = str.size();

    while(pos!=size) // ; ++pos
    {
        // Not in braces
        CharType ch = str[pos];
        typename StringType::size_type lIdx = leftBraces .find( ch );
        typename StringType::size_type rIdx = rightBraces.find( ch );

        if (lIdx==StringType::npos && rIdx==StringType::npos)
        {
            ++pos;
            continue; // not an opening or closing brace
        }

        // Found opening or closing brace
        bool nonPairedBraceOpen = true;
        if (lIdx!=StringType::npos && rIdx!=StringType::npos)
        {
            if (strictOrder)
            {
                if (!bracesStack.empty() && bracesStack.back()==ch)
                    nonPairedBraceOpen = false;
                //bracesStack.push_back(ch);
            }
            else
            {
                if (nestingCounts[lIdx]>0)
                    nonPairedBraceOpen = false;
            }
        }


        if (lIdx!=StringType::npos && nonPairedBraceOpen)
        {
            // Opening brace
        
            if (strictOrder)
            {
                bracesStack.push_back(ch);
            }
        
            if (startPos!=pos && util_check_all_exact( nestingCounts, 0 ))
                nonBraceRanges.push_back( std::make_pair(startPos,pos) );
            
            nestingCounts[lIdx]++;

            ++pos;
            continue;
        }

        // Only closing brace here
        if (strictOrder)
        {
            //bracesStack.push_back(ch);
            if (bracesStack.empty())
            {
                throw umba::FileParsingException("Found closing brace, but there is no opening brace found before", fileName, lineNumber, linePos);
            }
        
            CharType openingBrace = bracesStack.back();
            bracesStack.pop_back();
        
            if (get_pair(openingBrace)!=ch)
            {
                throw umba::FileParsingException("Opening brace doesn't match closing brace", fileName, lineNumber, linePos);
            }
        }

        ++pos;
        nestingCounts[rIdx]--;

        if (util_check_all_exact( nestingCounts, 0 ))
        {
            // Not not in braces
            startPos = pos;
        }
    }

    if (startPos!=pos)
        nonBraceRanges.push_back( std::make_pair(startPos,pos) );
}

//------------------------------
//! Возвращает размер входного разделителя
inline size_t util_get_sep_size( const char &sep )         { return 1; }
//! Возвращает размер входного разделителя
inline size_t util_get_sep_size( const wchar_t &sep )      { return 1; }
//! Возвращает размер входного разделителя
inline size_t util_get_sep_size( const char *sep )         { return std::strlen(sep); }
//! Возвращает размер входного разделителя
inline size_t util_get_sep_size( const wchar_t *sep )      { return std::wcslen(sep); }
//! Возвращает размер входного разделителя
inline size_t util_get_sep_size( const std::string &sep )  { return sep.size(); }
//! Возвращает размер входного разделителя
inline size_t util_get_sep_size( const std::wstring &sep ) { return sep.size(); }

//------------------------------
//! Какой-то хелпер
template<typename StringType, typename SepType> inline
std::vector< typename StringType::size_type >
split_against_braces_get_sep_positions( const StringType   &str    //!< Входная строка
                               , const StringType         &braces  //!< Обрабатываемые скобки
                               , const SepType            &sep     //!< Разделитель
                               , const std::string& fileName = ""  //!< Имя файла?
                               , size_t lineNumber = (size_t)-1    //!< Номер строки
                               , size_t linePos = (size_t)-1       //!< Позиция в строке
                               , bool strictOrder = false          //!< Строгий порядок?
                               )
{
    std::vector< std::pair< typename StringType::size_type, typename StringType::size_type> > nonBraceRanges;
    split_against_braces_helper( nonBraceRanges, str, braces, fileName, lineNumber, linePos, strictOrder );

    typename StringType::size_type sepSize = util_get_sep_size(sep);

    std::vector< typename StringType::size_type > sepPositions;

    typename std::vector< std::pair< typename StringType::size_type, typename StringType::size_type> >::const_iterator it = nonBraceRanges.begin();
    for(; it != nonBraceRanges.end(); ++it)
    {
        typename StringType::size_type beginPos = it->first;
        typename StringType::size_type endPos   = it->second;

        typename StringType::size_type sepPos   = str.find( sep, beginPos );
        while(sepPos!=StringType::npos && sepPos<endPos)
        {
            sepPositions.push_back(sepPos);
            beginPos = sepPos + sepSize;
            if (beginPos>=endPos)
                break;
            sepPos   = str.find( sep, beginPos );
        }
    }

    return sepPositions;
}

//------------------------------
//! Какая-то реализация
template<typename StringType, typename SepType> inline
void split_against_braces_impl( std::vector<StringType> &splits   //!< [out] Какой-то выхлоп
                              , const StringType        &str      //!< Входная строка
                              , const StringType        &braces   //!< Обрабатываемые скобки
                              , const SepType            &sep     //!< Разделитель
                              , const std::string& fileName = ""  //!< Имя файла?
                              , size_t lineNumber = (size_t)-1    //!< Номер строки
                              , size_t linePos = (size_t)-1       //!< Позиция в строке
                              , bool strictOrder = false          //!< Строгий порядок?
                              )
{
    typename StringType::size_type sepSize = util_get_sep_size(sep);

    std::vector< typename StringType::size_type > sepPositions = 
        split_against_braces_get_sep_positions( str, braces, sep, fileName, lineNumber, linePos, strictOrder );

    if (sepPositions.empty())
    {
        splits.push_back( str );
        return;
    }

    typename StringType::size_type startPos = 0, sepPos = 0, size = str.size();
    typename std::vector< typename StringType::size_type >::const_iterator sepPosIt = sepPositions.begin();
    for(; sepPosIt!=sepPositions.end() && startPos<size; ++sepPosIt)
    {
        sepPos = *sepPosIt;
        splits.push_back( StringType( str, startPos, sepPos-startPos) );
        startPos = sepPos + sepSize;
    }

    //if ((sepPos))
    //if (startPos<size )
    {
        splits.push_back( StringType( str, startPos, StringType::npos ) );
    }
}

//------------------------------
//! Сплит строки по разделителю с учетом скобок
template<typename StringType> inline
void split_against_braces( std::vector<StringType> &splits      //!< [out] Выхлоп
                         , const StringType        &str         //!< Входная строка
                         , const StringType        &braces      //!< Обрабатываемые скобки
                         , typename StringType::value_type sep  //!< Разделитель
                         , const std::string& fileName = ""     //!< Имя файла?
                         , size_t lineNumber = (size_t)-1       //!< Номер строки
                         , size_t linePos = (size_t)-1          //!< Позиция в строке
                         , bool strictOrder = false             //!< Строгий порядок?
                         )
{
    split_against_braces_impl<StringType, typename StringType::value_type>( splits, str, braces, sep, fileName, lineNumber, linePos, strictOrder );
}

//------------------------------
//! Сплит строки по разделителю с учетом скобок
template<typename StringType> inline
void split_against_braces( std::vector<StringType>                &splits  //!< [out] Выхлоп         
                         , const StringType                       &str     //!< Входная строка       
                         , const typename StringType::value_type  *braces  //!< Обрабатываемые скобки
                         , typename StringType::value_type         sep     //!< Разделитель          
                         , const std::string& fileName = ""                //!< Имя файла?           
                         , size_t lineNumber = (size_t)-1                  //!< Номер строки         
                         , size_t linePos = (size_t)-1                     //!< Позиция в строке     
                         , bool strictOrder = false                        //!< Строгий порядок?     
                         )
{
    split_against_braces_impl<StringType, typename StringType::value_type>( splits, str, braces, sep, fileName, lineNumber, linePos, strictOrder );
}

//------------------------------
//! Сплит строки по разделителю с учетом скобок
template<typename StringType> inline
void split_against_braces( std::vector<StringType> &splits   //!< [out] Выхлоп         
                         , const StringType        &str      //!< Входная строка       
                         , const StringType        &braces   //!< Обрабатываемые скобки
                         , const StringType        &sep      //!< Разделитель          
                         , const std::string& fileName = ""  //!< Имя файла?           
                         , size_t lineNumber = (size_t)-1    //!< Номер строки         
                         , size_t linePos = (size_t)-1       //!< Позиция в строке     
                         , bool strictOrder = false          //!< Строгий порядок?     
                         )
{
    split_against_braces_impl<StringType, StringType>( splits, str, braces, sep, fileName, lineNumber, linePos, strictOrder );
}

//------------------------------
//! Сплит строки по разделителю с учетом скобок
template<typename StringType> inline
void split_against_braces( std::vector<StringType>               &splits  //!< [out] Выхлоп         
                         , const StringType                      &str     //!< Входная строка       
                         , const typename StringType::value_type *braces  //!< Обрабатываемые скобки
                         , const StringType                       &sep    //!< Разделитель          
                         , const std::string& fileName = ""               //!< Имя файла?           
                         , size_t lineNumber = (size_t)-1                 //!< Номер строки         
                         , size_t linePos = (size_t)-1                    //!< Позиция в строке     
                         , bool strictOrder = false                       //!< Строгий порядок?     
                         )
{
    split_against_braces_impl<StringType, StringType>( splits, str, braces, sep, fileName, lineNumber, linePos, strictOrder );
}


//------------------------------

} // ascii_brace

//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
//! Удаление пустых элементов
template<typename StringType> inline
void eraseEmptiesHelper( std::vector<StringType> v /*!< Вектор элементов */)
{
    
    // Чукич, стр 62
    #ifdef UMBA_CXX_HAS_STD11 
    
    // В 2005 студии оно не работает
    v.erase( std::remove_if( v.begin(), v.end(), []( const StringType &s) { return s.empty(); } ) );

    #else

    std::vector<StringType>::iterator rmIt = std::remove_if( v.begin(), v.end(), lambda_is_empty<StringType>() );
    if (rmIt!=)
        v.erase( rmIt );

    #endif
}

//------------------------------
//! Тупой сплит строки, используется версия сплита по скобкам с редуцированным набором скобок
template<typename StringType> inline
std::vector<StringType> split( const StringType &str                  //!< Входная строка
                             , typename StringType::value_type delim  //!< Разделитель
                             , bool skipEmpty = true                  //!< Пропускать пустые элементы?
                             )
{
    std::vector<StringType> splits;
    ascii_brace::split_against_braces( splits, str
                                     , StringType() // no braces at all
                                     , delim
                                     );

    if (skipEmpty)
    {
        // Чукич, стр 62
        #if 0 // В 2005 студии оно не работает
        eraseEmptiesHelper( splits );
        #endif
    }

    return splits;
}

//------------------------------
//! Тупой сплит строки, используется версия сплита по скобкам с редуцированным набором скобок
template<typename StringType> inline
std::vector<StringType> split( const StringType &str    //!< Входная строка             
                             , const StringType &delim  //!< Разделитель                
                             , bool skipEmpty = true    //!< Пропускать пустые элементы?
                             )
{
    std::vector<StringType> splits;
    ascii_brace::split_against_braces( splits, str
                                     , StringType() // no braces at all
                                     , delim
                                     );

    if (skipEmpty)
    {
        // Чукич, стр 62
        #if 0 // В 2005 студии оно не работает
        splits.erase( std::remove_if( splits.begin()
                                    , splits.end()
                                    #ifdef UMBA_CXX_HAS_STD11
                                    , []( const StringType &s) { return s.empty(); }
                                    #else
                                    , lambda_is_empty<StringType>()
                                    #endif
                                    )
                    );
        #endif
    }

    return splits;
}

//-----------------------------------------------------------------------------
//! Сплит строки по классу символа
/*! Хз, что, где-то было нужно
    \note https://stackoverflow.com/questions/257288/templated-check-for-the-existence-of-a-class-member-function
 */
template<typename StringType, typename CharClassBoolPred, typename CharClassType> inline
std::vector<StringType> split_by_class( const StringType &str          //!< Входная строка
                                      , const CharClassBoolPred &pred  //!< Предикат класса символа
                                      )
{
    std::vector<StringType> res;

    if (str.empty())
        return res;
    
    res.push_back( StringType() );
    CharClassType prevCharClass = pred(str[0]);

    typename StringType::size_type i = 0, size = str.size();
    for(; i!=size; ++i)
    {
        typename StringType::value_type ch = str[i];
        CharClassType curCharClass = pred(ch);
        if (prevCharClass!=curCharClass)
            res.push_back( std::string() );
        prevCharClass = curCharClass;
        res.back().push_back(ch);
    }

    return res;

}

//-----------------------------------------------------------------------------
//! Делит строку на две подстроки по сепаратору, возвращает true, если split реально произошел
template<typename StringType> inline
bool split_to_pair( const StringType str, StringType &first, StringType &second, const StringType &sep, typename StringType::size_type pos = 0 )
{
    auto fpos = str.find( sep, pos );

    if (fpos==str.npos)
    {
        second.clear();
        first = str;
        return false;
    }

    first  = str.substr( 0              , fpos     );
    second = str.substr( fpos+sep.size(), str.npos );

    return true;

}

//-----------------------------------------------------------------------------
//! Делит строку на две подстроки по сепаратору, возвращает true, если split реально произошел
template<typename StringType> inline
bool split_to_pair( const StringType str, StringType &first, StringType &second, const typename StringType::value_type &sep, typename StringType::size_type pos = 0 )
{
    auto fpos = str.find( sep, pos );

    if (fpos==str.npos)
    {
        second.clear();
        first = str;
        return false;
    }

    first  = str.substr( 0     , fpos     );
    second = str.substr( fpos+1, str.npos );

    return true;

}


// bool splitToPair( std::string str, std::string &first, std::string &second, char ch, std::string::size_type pos = 0 )

// bool hasEq = splitToPair(cmdLineArg, opt, optArg, '=');


//-----------------------------------------------------------------------------
//! Возвращает true, если строка закавычена. 
/*! 
    Если quotEnd не равен нулю, то кавычки используются как задано, иначе - автоопределение.
 */
template<typename StringType> inline
bool is_quoted( const StringType &s                        //!< Строка для проверки
              , typename StringType::value_type quotStart    //!< Открывающая кавычка
              , typename StringType::value_type quotEnd = 0  //!< Закрывающая кавычка, если 0 - то срабатывает автоопределение обоих кавычек (используются парные символы по открывающией кавычке)
              )
{
    if (s.size()<2)
        return false;

    if (quotEnd==0)
    {
        quotEnd   = ascii_brace::get_right(quotStart);
        quotStart = ascii_brace::get_left(quotStart);
    }

    if (s[0]==quotStart && s[s.size()-1]==quotEnd)
        return true;

    return false;
}

//-----------------------------------------------------------------------------
//! Возвращает true, если строка была закавычена, и раскавычивает её. 
/*! 
    Если quotEnd не равен нулю, то кавычки используются как задано, иначе - автоопределение.
 */
template<typename StringType> inline
bool unquote( StringType &s                             //!< Строка для раскавычивания
            , typename StringType::value_type quotStart   //!< Открывающая кавычка
            , typename StringType::value_type quotEnd = 0 //!< Закрывающая кавычка, если 0 - то срабатывает автоопределение обоих кавычек (используются парные символы по открывающией кавычке)
            )
{
    if (!is_quoted(s, quotStart, quotEnd))
        return false;
    
    s.erase( s.size()-1 );
    s.erase( 0, 1 );
    trim(s);
    return true;
    
}

//-----------------------------------------------------------------------------
//! Возвращает true, если строка была закавычена, и раскавычивает её. 
/*! 
    Используются первые символы параметров strStart и strEnd. Если quotEnd не равен нулю, то кавычки используются как задано, иначе - автоопределение.
 */
template<typename StringType> inline
bool unquote( StringType &s                           //!< Строка для раскавычивания
            , const StringType &strStart              //!< Открывающая кавычка
            , const StringType &strEnd = StringType() //!< Закрывающая кавычка, если пустая строка - то срабатывает автоопределение обоих кавычек (используются парные символы по открывающией кавычке)
            )
{
    if (strStart.empty())
        return false;

    if (strEnd.empty())
       return unquote( s, strStart[0], 0 );

    return unquote( s, strStart[0], strEnd[0] );
}


//-----------------------------------------------------------------------------
// Same behavior as in unquote
//! \copydoc unquote
template<typename StringType> inline
bool unquote_if_quoted( StringType &s, typename StringType::value_type quotStart, typename StringType::value_type quotEnd = 0 )
{
    return unquote( s, quotStart, quotEnd );
}

//-----------------------------------------------------------------------------
//! \copydoc unquote
template<typename StringType> inline
bool unquote_if_quoted( StringType &s, const StringType &quotStart, const StringType &quotEnd = StringType() )
{
    return unquote( s, quotStart, quotEnd );
}

//-----------------------------------------------------------------------------
//! Закавычивает строку
template<typename StringType> inline
StringType quote( const typename StringType::value_type *str                                         //!< Строка для закавычивания
                , typename StringType::value_type quotStart = (typename StringType::value_type)'\''  //!< Открывающая кавычка
                , typename StringType::value_type quotEnd = 0                                        //!< Закрывающая кавычка, если 0 - то срабатывает автоопределение обоих кавычек (используются парные символы по открывающией кавычке) 
                )
{
    if (quotEnd==0)
    {
        quotEnd   = ascii_brace::get_right(quotStart);
        quotStart = ascii_brace::get_left(quotStart);
    }

    StringType res = StringType( 1, quotStart );
    res.append(str);
    res.append( 1, quotEnd );
    return res;
}

//-----------------------------------------------------------------------------
//! \copydoc quote
template<typename StringType> inline
StringType quote( const StringType &str
                , typename StringType::value_type quotStart = (typename StringType::value_type)'\''
                , typename StringType::value_type quotEnd = 0
                )
{
    quotStart = ascii_brace::get_left(quotStart);
    quotEnd   = quotEnd==0 ? ascii_brace::get_right(quotStart) : ascii_brace::get_right(quotEnd);

    StringType res = StringType( 1, quotStart );
    res.append(str);
    res.append( 1, quotEnd );
    return res;
}

//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
//! Хелпер-обертка для sprintf и аналогичных платформо-зависимых аналогов.
/*! 
    Используется vsnprintf.
    Теоретически, vsnprintf есть везде, но имя обычно чуть-чуть, но не такое как у других.
    Второе - не надо следить за буфером, его выделением, переполнением, и тп.
    Выбирается некоторый разумный размер буфера, лишнее - обрезается.
    Не следует задавать форматные строки огромной длины.
    Функция предназначена в основном для внутреннего использования, для printf-like быстрого форматирования интегральных величин,
    но теоретически можно использовать и форматные строки, задаваемые извне.

    Базовый шаблон вызовет ассерт, нужна специализация для конкретного типа

    \param fmt Форматная строка
 */
template <typename StringType> inline
StringType format_print( const StringType fmt, ... )
{
    UMBA_ASSERT_FAIL();
}

//-----------------------------------------------------------------------------
//! Хелпер для вычисления размера буфера для функции format_print
inline std::size_t calc_buf_char_size_for_format_print( std::size_t fmtStringSize )
{
    UMBA_ASSERT(fmtStringSize<256);

    std::size_t bufSize = fmtStringSize*16;
    if (bufSize<128)
        bufSize = 128;
    if (bufSize>4096)
        bufSize = 4096;

    return bufSize;

}
//-----------------------------------------------------------------------------
//! \copydoc format_print
template <> inline
std::string format_print<std::string>( const std::string fmt, ... )
{
    std::size_t bufSize = calc_buf_char_size_for_format_print (fmt.size());

    char *pBuf = 0;
    try
    {
        pBuf = (char*)alloca(bufSize*sizeof(*pBuf));
        if (!pBuf) return fmt;
    }
    catch(...)
    {
        return fmt;
    }

    va_list args;
    va_start (args, fmt);
    int numCharsPrinted = vsnprintf(pBuf,bufSize-1, fmt.c_str(), args );
    va_end (args);

    return std::string(pBuf);
}

//-----------------------------------------------------------------------------

#ifdef UMBA_MSVC_COMPILER_USED
//! \copydoc format_print
template <> inline
std::wstring format_print<std::wstring>( const std::wstring fmt, ... )
{
    std::size_t bufSize = calc_buf_char_size_for_format_print (fmt.size());

    wchar_t *pBuf = 0;
    try
    {
        (wchar_t*)alloca(bufSize*sizeof(*pBuf));
        if (!pBuf) return fmt;
    }
    catch(...)
    {
        return fmt;
    }

    va_list args;
    va_start (args, fmt);
    int numCharsPrinted = _vsnwprintf(pBuf, bufSize-1, fmt.c_str(), args );
    va_end (args);

    return std::wstring(pBuf);
}
#endif // UMBA_MSVC_COMPILER_USED

//-----------------------------------------------------------------------------

// End of UMBA_STRING_PLUS
/*! @}*/

//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
} // namespace string_plus

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
//! Алиас для совместимости совместимости со старым кодом
// namespace cpp = string_plus;

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------

} // namespace umba




