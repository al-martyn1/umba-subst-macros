#pragma once



#include "enum_helpers.h"


#define UMBA_INTEGRAL_ARITHMETIC_STRONG_TYPEDEF( newTypeName, underlyingTypeName )             \
                                                                                               \
             enum class newTypeName : underlyingTypeName                                       \
             {                                                                                 \
                 dummy                                                                         \
             };                                                                                \
                                                                                               \
             UMBA_ENUM_CLASS_IMPLEMENT_ARITHMETIC_OPERATORS( newTypeName )                     \
             UMBA_ENUM_CLASS_IMPLEMENT_UNDERLYING_TYPE_ARITHMETIC_OPERATORS( newTypeName )     \
             UMBA_ENUM_CLASS_IMPLEMENT_RELATION_OPERATORS( newTypeName )                       \
             UMBA_ENUM_CLASS_IMPLEMENT_UNDERLYING_TYPE_RELATION_OPERATORS( newTypeName )       \
              /* UMBA_ENUM_CLASS_IMPLEMENT_BIT_OPERATORS( newTypeName ) */                     \
              /* UMBA_ENUM_CLASS_IMPLEMENT_UNDERLYING_TYPE_BIT_OPERATORS( newTypeName ) */     \
             UMBA_ENUM_CLASS_IMPLEMENT_UNDERLYING_TYPE_EQUAL_OPERATORS( newTypeName )          \
                                                                                               \
             template< typename StreamType >                                                   \
             StreamType& operator<<( StreamType &os, newTypeName val )                         \
             {                                                                                 \
                 os << umba::enum_helpers::toUnderlyingType(val);                              \
                 return os;                                                                    \
             }
             

// Also to read:
//   https://www.foonathan.net/2016/10/strong-typedefs/





