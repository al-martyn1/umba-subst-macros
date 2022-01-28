#pragma once

//----------------------------------------------------------------------------

/*! \file
    \brief Работа с переменными окружения
*/

//----------------------------------------------------------------------------

#include "umba/umba.h"
#include "umba/string_plus.h"

#if !defined(_INC_STDLIB) && !defined(_STDLIB_H_) && !defined(_STDLIB_H)
    #include <stdlib.h>
#endif


namespace umba
{

//! Окружение и работа с его переменными
namespace env
{

//! Получение переменной окружения, возвращает true, если значение получено
inline
bool getVar(const ::std::string &varName, ::std::string &val)
   {
    if (varName.empty()) return false;
    char * pval = ::getenv(varName.c_str());
    if (!pval) return false;
    val = pval;
    return true;
   }

#if defined(_MSC_VER) && _MSC_VER>=1300
    #pragma warning( push )
    #pragma warning( disable : 4996 ) // Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct.
#endif

//! Установка переменной окружения
inline
bool putVar(const ::std::string &varName, const ::std::string &val)
   {
    ::std::string tmp = varName + ::std::string("=") + val;
    // Могут быть утечки памяти, но зато везде безопасно.
    // Некоторые системы не делают копию передаваемого значения, 
    // а запоминают непосредственно переданное значение, поэтому передача
    // автоматических переменных вызовет крах.
    char *ptmp = (char*)malloc(tmp.size()+1);
    if (!ptmp)
        return false;
    tmp.copy(ptmp, tmp.size());
    ptmp[tmp.size()] = 0;
    #ifdef _MSC_VER
    if (::_putenv(ptmp)<0) return false;
    #else
    if (::putenv(ptmp)<0) return false;
    #endif
    return true;
   }
#if defined(_MSC_VER) && _MSC_VER>=1300
    #pragma warning( pop )
#endif


#if defined(WIN32) || defined(_WIN32)

//! Получение переменной окружения, возвращает true, если значение получено
inline
bool getVar(const ::std::wstring &varName, ::std::wstring &val)
   {
    if (varName.empty()) return false;
    wchar_t * pval = ::_wgetenv(varName.c_str());
    if (!pval) return false;
    val = pval;
    return true;
   }    

//! Получение переменной окружения, возвращает true, если значение получено
inline
bool getVar(const ::std::wstring &varName, ::std::string &val)
   {
    if (varName.empty()) return false;
    wchar_t * pval = ::_wgetenv(varName.c_str());
    if (!pval) return false;
    val = string_plus::make_string<std::string>(pval);
    return true;
   }    

//! Получение переменной окружения, возвращает true, если значение получено
inline
bool getVar(const ::std::string &varName, ::std::wstring &val)
   {
    if (varName.empty()) return false;
    char * pval = ::getenv(varName.c_str());
    if (!pval) return false;
    val = string_plus::make_string<std::wstring>(pval);
    return true;
   }

//------------------------------
#if defined(_MSC_VER) && _MSC_VER>=1300
    #pragma warning( push )
    #pragma warning( disable : 4996 ) // Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct.
#endif

inline
bool putVar(const ::std::wstring &varName, const ::std::wstring &val)
   {
    ::std::wstring tmp = varName + ::std::wstring(L"=") + val;
    wchar_t *ptmp = (wchar_t*)malloc((tmp.size()+1)*sizeof(wchar_t));
    if (!ptmp)
        return false;
    tmp.copy(ptmp, tmp.size());
    ptmp[tmp.size()] = 0;
    if (::_wputenv(ptmp)<0) return false;
    return true;
   }

#if defined(_MSC_VER) && _MSC_VER>=1300
    #pragma warning( pop )
#endif


#endif


}; // namespace env
}; // namespace umba



