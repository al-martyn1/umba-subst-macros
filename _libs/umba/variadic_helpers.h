#pragma once

//-----------------------------------------------------------------------------

/*! 
    \file
    \brief Хелперы для вариадик-шаблонов
 */

//-----------------------------------------------------------------------------

#include "umba.h"

namespace umba
{

// Variadic template test - https://ideone.com/fKViyk
//                          https://ideone.com/UyvVmZ

//-----------------------------------------------------------------------------
// Variadic tpl helpers

//! Variadic-хелпер для получения типа N-ного аргумента в списке аргументов. Cм. также #buildSomeTypeArray
template<std::size_t N, typename T, typename... types>
struct get_Nth_type
{
    using type = typename get_Nth_type<N - 1, types...>::type; //!< Типа тайпдеф
};

//! copybrief get_Nth_type
template<typename T, typename... types>
struct get_Nth_type<0, T, types...>
{
    using type = T; //!< Типа тайпдеф
};

//! copybrief get_Nth_type
template<std::size_t N, typename... Args>
using getVariadicArgsType = typename get_Nth_type<N, Args...>::type;
//-----------------------------------------------------------------------------






//-----------------------------------------------------------------------------
//! Шаблон преобразования variadic аргументов шаблона в простой массив - реализация - завершающая версия
template< size_t arrayBufSize, size_t itemNo, typename ArrayItemType >
inline
size_t buildSomeTypeArrayImpl(ArrayItemType *pArrayBuf)
{
    return itemNo;
}

//-----------------------------------------------------------------------------
//! Шаблон преобразования variadic аргументов шаблона в простой массив - реализация
template< size_t arrayBufSize, size_t itemNo, typename ArrayItemType, typename SomeType >
inline
size_t buildSomeTypeArrayImpl( ArrayItemType *pArrayBuf
                             , const SomeType &s)
{
    if (itemNo >= arrayBufSize)
    {
        static_assert(itemNo < arrayBufSize, "Error");
        return itemNo;
    }

    pArrayBuf[itemNo] = s;
    return itemNo + 1;
}

//-----------------------------------------------------------------------------
//! Шаблон преобразования variadic аргументов шаблона в простой массив - реализация
template< size_t arrayBufSize, size_t itemNo
        , typename ArrayItemType, typename SomeType
        , typename... SomeTypeListItems
        >
inline
size_t buildSomeTypeArrayImpl( ArrayItemType *pArrayBuf
                             , const SomeType &s
                             , SomeTypeListItems... structs)
{
    if (itemNo >= arrayBufSize)
    {
        static_assert(itemNo < arrayBufSize, "Error");
        return itemNo;
    }
    else
    {
        buildSomeTypeArrayImpl<arrayBufSize, itemNo >(pArrayBuf, s);
        return buildSomeTypeArrayImpl<arrayBufSize, itemNo + 1 >(pArrayBuf, structs...);
    }
}

//-----------------------------------------------------------------------------
//! Шаблон преобразования variadic аргументов шаблона в простой массив
/*! Использование

\code
// Function template with variadic args

template< typename... SomeTypeListItems >
inline
AdcInjected adcInitInject( ADC_TypeDef* ADCx, AdcSamplingSpeed samplingSpeed
                         , AdcInitHwOption doInitAdc // set to true to add injected channels over regular
                         , SomeTypeListItems... structs)
{
    getVariadicArgsType< 0, SomeTypeListItems... > buf[sizeof...(structs)];

    size_t sz = buildSomeTypeArray< sizeof...(structs) > (buf, structs...);

    return adcInitInject( ADCx, samplingSpeed, doInitAdc, buf, sz );
}

//...

// Calling that function
auto 
     adc1 = umba::periph::adcInitInject( ADC1, umba::periph::AdcSamplingSpeed::high
                                       , umba::periph::AdcInitHwOption::init
                                       , PA1, PA2, PA3 // here is the list
                                       );

\endcode

 */

template< size_t arrayBufSize, typename ArrayItemType, typename... SomeTypeListItems >
inline
size_t buildSomeTypeArray( ArrayItemType *pArrayBuf
                                   , SomeTypeListItems... structs)
{
    return buildSomeTypeArrayImpl< arrayBufSize, 0 >(pArrayBuf, structs...);
}


/*

!!! Usage    

template< typename... SomeTypeListItems >
inline
AdcInjected adcInitInject( ADC_TypeDef* ADCx, AdcSamplingSpeed samplingSpeed
                         , AdcInitHwOption doInitAdc // set to true to add injected channels over regular
                         , SomeTypeListItems... structs)
{
    getVariadicArgsType< 0, SomeTypeListItems... > buf[sizeof...(structs)];

    size_t sz = buildSomeTypeArray< sizeof...(structs) > (buf, structs...);

    return adcInitInject( ADCx, samplingSpeed, doInitAdc, buf, sz );
}


    auto 
         adc1 = umba::periph::adcInitInject( ADC1, umba::periph::AdcSamplingSpeed::high
                                           , umba::periph::AdcInitHwOption::init
                                           , PA1, PA2, PA3 // here is the list
                                           );


*/






} // namespace umba

