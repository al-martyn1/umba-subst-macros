/*! \file
\brief Совместимое со всеми компиляторами подключение стандартных заголовков
*/

#pragma once
#include "umba/umba.h"

#if defined(UMBA_CXX_HAS_STD11)
    #include <stdint.h>
#else

    #include <limits.h>
    
    namespace std{

        template<class _Ty,
        _Ty _Val>
        struct integral_constant
        {	// convenient template for integral constant types
        static 
        //constexpr 
        _Ty value = _Val;
        
        typedef _Ty value_type;
        typedef integral_constant<_Ty, _Val> type;
        
        //constexpr 
        operator value_type() const //_NOEXCEPT 
        	{	// return stored value
        	return (value);
        	}
        
        //constexpr 
        value_type operator()() const //_NOEXCEPT
        	{	// return stored value
        	return (value);
        	}
        };
        
        typedef integral_constant<bool, true> true_type;
        typedef integral_constant<bool, false> false_type;
           
   
        typedef unsigned __int8     uint8_t ; 
        typedef signed   __int8     int8_t  ;

        typedef unsigned __int16    uint16_t;
        typedef signed   __int16    int16_t ;

        typedef unsigned __int32    uint32_t;
        typedef signed   __int32    int3_t  ;

        typedef unsigned __int64    uint64_t;
        typedef signed   __int64    int64_t ;

        typedef unsigned __int64    uintmax_t;
        typedef signed   __int64    intmax_t ;
        
    } // namespace std

    typedef std::uint8_t      uint8_t ;
    typedef std::int8_t       int8_t  ;
                                      
    typedef std::uint16_t     uint16_t;
    typedef std::int16_t      int16_t ;
                                      
    typedef std::uint32_t     uint32_t;
    typedef std::int3_t       int3_t  ;
                                      
    typedef std::uint64_t     uint64_t;
    typedef std::int64_t      int64_t ;

    typedef std::uintmax_t    uintmax_t;
    typedef std::intmax_t     intmax_t ;


    #define INT8_MIN      SCHAR_MIN
    #define INT16_MIN     SHRT_MIN
    #define INT32_MIN     INT_MIN
    #define INT64_MIN     LLONG_MIN

    #define INT8_MAX      SCHAR_MAX
    #define INT16_MAX     SHRT_MAX
    #define INT32_MAX     INT_MAX
    #define INT64_MAX     LLONG_MAX

    #define UINT8_MIN     0
    #define UINT16_MIN    0
    #define UINT32_MIN    0
    #define UINT64_MIN    0

    #define UINT8_MAX     UCHAR_MAX
    #define UINT16_MAX    USHRT_MAX
    #define UINT32_MAX    UINT_MAX
    #define UINT64_MAX    ULLONG_MAX

#endif

#include <utility>
#include <algorithm>
#include <cstddef>

#if defined(UMBA_KEIL_ARMCC_COMPILER_USED)
    #include "stl_keil.h"
#else
    #if defined(UMBA_CXX_HAS_STD11)
        #include <type_traits>
        #include <initializer_list>
    #else
        #include "stl_keil.h"
    #endif
#endif



#if !defined(UMBA_CXX_HAS_STD14)

    namespace std{

        template< bool B, class T = void >
        using enable_if_t = typename enable_if<B,T>::type;

    } // namespace std

#endif


