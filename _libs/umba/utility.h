#pragma once

//----------------------------------------------------------------------------

/*! \file
    \brief Утилитки в C++ стиле
*/

//----------------------------------------------------------------------------

#include <string>
#include <vector>
#include <algorithm> 
#include <iterator>
#include <cctype>

#include "umba/stl.h"


// umba::utility::

namespace umba{

//! Утилитки в C++ стиле
namespace utility
{

/*! \ingroup UMBA_CORE
 *  \addtogroup UMBA_CPP
 *  @{
 */


//----------------------------------------------------------------------------
// https://stackoverflow.com/questions/19392361/adding-element-to-back-of-stl-container

//  Usage: optional_reserve( c, N );
//----------------------------------------------------------------------------
//! Базовый false-тип для детекта наличия метода reserve у объекта
template< typename C, typename = void >
struct has_reserve
  : std::false_type
{};

//------------------------------
//! Специализация, тестирующая наличие метода reserve у объекта
template< typename C >
struct has_reserve< C, std::enable_if_t<
                         std::is_same<
                           decltype( std::declval<C>().reserve( std::declval<typename C::size_type>() ) ),
                           void
                         >::value
                       > >
  : std::true_type
{};

//------------------------------
//! Функция резервирует дополнительное (по отношению к текущему занимаемому объему) место, если у объекта есть метод reserve
/*! 
     \tparam C Тип контейнера
 */
template< typename C >
std::enable_if_t< has_reserve< C >::value > inline
optional_reserve( C& c, std::size_t n )
{
  c.reserve( c.size() + n );
}

//------------------------------
//! Версия функции резервирования места optional_reserve для контейнеров, не имеющих метода reserve. Не делает ничего.
/*! 
     \tparam C Тип контейнера
 */
template< typename C >
std::enable_if_t< !has_reserve< C >::value > inline
optional_reserve( C&, std::size_t ) {}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
//  Usage: optional_shrink_to_fit( c );
//----------------------------------------------------------------------------
//! Базовый false-тип для детекта наличия метода shrink_to_fit у объекта
template< typename C, typename = void >
struct has_shrink_to_fit
  : std::false_type
{};

//------------------------------
//! Специализация, тестирующая наличие метода shrink_to_fit у объекта
template< typename C >
struct has_shrink_to_fit< C, std::enable_if_t<
                         std::is_same<
                           decltype( std::declval<C>().shrink_to_fit( ) ),
                           void
                         >::value
                       > >
  : std::true_type
{};

//------------------------------
//! Функция пытается освободить лишнюю занятую, но не используемую контейнером (объектом) память, если у объекта есть метод shrink_to_fit
/*! 
     \tparam C Тип контейнера
 */
template< typename C >
std::enable_if_t< has_shrink_to_fit< C >::value > inline
optional_shrink_to_fit( C& c )
{
  c.shrink_to_fit();
}

//------------------------------
//! Версия функции резервирования места optional_shrink_to_fit для контейнеров, не имеющих метода shrink_to_fit. Не делает ничего.
/*! 
     \tparam C Тип контейнера
 */
template< typename C >
std::enable_if_t< !has_shrink_to_fit< C >::value > inline
optional_shrink_to_fit( C&, std::size_t ) {}

//----------------------------------------------------------------------------





//----------------------------------------------------------------------------
#ifndef UMBA_MCU_USED

//! Вычисляет дистанцию между индексами, по аналогии с итераторами
/*! 
     \return Возвращает вычисленную дистанцию
 */
template< typename IterType >
std::enable_if_t< std::is_integral< IterType >::value > inline
distance(IterType b, IterType e)
{
    return e-b;
}


//! Вычисляет дистанцию между чем-то, что видимо является итераторами
/*! 
     \return Возвращает вычисленную дистанцию
 */
template< typename IterType >
std::enable_if_t< !std::is_integral< IterType >::value >
distance(IterType b, IterType e)
{
    return std::distance(b,e);
}
#endif


/*
template< typename IterType > inline
auto distance(IterType b, IterType e) -> decltype(a*b)
{
    if constexpr( std::is_integral_v< IterType > )
        return e-b;
    else
        return std::distance(b,e);
}
*/

//----------------------------------------------------------------------------


// End of UMBA_CPP
/*! @}*/


} // namespace utility
} // namespace umba


