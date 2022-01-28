#pragma once

//----------------------------------------------------------------------------

/*! \file
    \brief Различные колхозные алгоритмы
*/

//----------------------------------------------------------------------------

#include <string>
#include <vector>
#include <algorithm> 
#include <iterator>
#include <cctype>

#include "umba/utility.h"
#include "umba/string_plus.h"


// umba::algoritms::

namespace umba{
namespace algoritms{

/*
template<typename StringType, typename InputIteratorType, typename ContainerType> inline
ContainerType generate_strings( const StringType &fmt, InputIteratorType b, InputIteratorType e)
{
    // https://www.fluentcpp.com/2017/03/28/inserting-several-elements-into-an-stl-container/

    ContainerType res;

    umba::utility::optional_reserve(res, std::distance(b,e));

    std::transform( b, e, std::back_inserter(res)
                  , [fmt]( InputIteratorType iter )
                    {
                        return umba::string_plus::format_print(fmt, *iter)
                    }
                  );
    return res;
}
*/

#ifndef UMBA_MCU_USED

    #ifdef UMBA_CXX_HAS_STD11

        //! Хелпер для генерации строк по форматной строке и набору входных данных
        template<typename StringType, typename InputIteratorType, typename OutputIteratorType > inline
        void generate_strings_helper( const StringType &fmt, InputIteratorType b, InputIteratorType e, OutputIteratorType output )
        {
          #ifdef UMBA_CXX_HAS_STD17
            if constexpr( std::is_integral_v< InputIteratorType > )
          #else
            if ( std::is_integral< InputIteratorType >::value )
          #endif
            {
                for(; b!=e; ++b)
                {
                    *output = umba::string_plus::format_print(fmt, b);
                }
            }
            else
            {
                std::transform( b, e, output
                              , [fmt]( const InputIteratorType &iter )
                                {
                                    return umba::string_plus::format_print(fmt, iter);
                                }
                              );
            }
        }

        //! Генерация строк по форматной строке и набору входных данных
        template<typename FormatContainerType, typename InputIteratorType, typename ResultContainerType> inline
        ResultContainerType generate_strings( const FormatContainerType &fmtContainer, InputIteratorType b, InputIteratorType e)
        {
            // https://www.fluentcpp.com/2017/03/28/inserting-several-elements-into-an-stl-container/
        
            ResultContainerType res;
        
            auto distance = umba::utility::distance(b,e);
        
          #ifdef UMBA_CXX_HAS_STD17
            if constexpr( std::is_integral_v< typename FormatContainerType::value_type > )
          #else
            if ( std::is_integral< typename FormatContainerType::value_type >::value )
          #endif
            {
                umba::utility::optional_reserve(res, distance);
                generate_strings_helper( fmtContainer, b, e, std::back_inserter(res) );
            }
            else
            {
                umba::utility::optional_reserve(res, fmtContainer.size()*distance);
        
                for( const auto &fmt: fmtContainer)
                {
                    generate_strings_helper( fmt, b, e, std::back_inserter(res) );
                }
            }
        
            return res;
        }

    #endif

#endif



} // namespace algoritms
} // namespace umba

