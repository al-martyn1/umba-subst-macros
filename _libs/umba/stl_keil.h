/*! \file
\brief Некоторые фичи C++11, которые не реализованы библиотеке компилятора Keil
*/
#pragma once

namespace std
{
    // Defined in header <cstddef>
    #if defined(UMBA_CXX_HAS_STD11)
	typedef decltype(nullptr) nullptr_t;
    #endif


    // Defined in header <algorithm>
    template< class T > struct remove_reference      {typedef T type;};
	template< class T > struct remove_reference<T&>  {typedef T type;};
    #if defined(UMBA_CXX_HAS_STD11)
	template< class T > struct remove_reference<T&&> {typedef T type;};
						
	template<class T> 
	typename remove_reference<T>::type&&
	move(T&& a) noexcept
	{
		typedef typename remove_reference<T>::type&& RvalRef;
		return static_cast<RvalRef>(a);
	} 

    // Defined in header <utility>
	template<class T> 
	T&&
	forward(typename remove_reference<T>::type& a) noexcept
	{
		return static_cast<T&&>(a);
	} 
    #endif

} // namespace std

#include "stl_keil_type_traits.h"
// #include "stl_keul_initializer_list.h"
#include "stl_keil_initializer_list.h"

