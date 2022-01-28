#pragma once

//----------------------------------------------------------------------------

/*! 
    \file
    \brief Типа пользовательские литералы
 */

/*
template<typename TargetType>
class UserDefinedLiteralCrutch
{
    TargetType  val;

public:

    constexpr
    UserDefinedLiteralCrutch( TargetType v ) : val(v) {}

    constexpr
    operator TargetType() const
    {
        return val;
    }

};


#define UMBA_DEFINE_USER_LITERAL_TYPE( TargetType, LiteralType ) typedef UserDefinedLiteralCrutch< TargetType >  LiteralType
*/
//! Объявляет пользовательский тип с оператором преобразования, в котором обычно просто масштабирование
#define UMBA_DEFINE_USER_LITERAL_TYPE_BEGIN( TargetType, LiteralType )\
class LiteralType                                                     \
{                                                                     \
    TargetType  value;                                                \
                                                                      \
public:                                                               \
                                                                      \
    constexpr                                                         \
    LiteralType( TargetType v ) : value(v) {}                         \
                                                                      \
    constexpr                                                         \
    operator TargetType() const                                       \
    {

//! Завершает объявление пользовательского типа
#define UMBA_DEFINE_USER_LITERAL_TYPE_END( )\
                                            \
        /*return val;*/                     \
    }                                       \
                                            \
};



#if 0

// Usage sample

UMBA_DEFINE_USER_LITERAL_TYPE_BEGIN( NanosecInterval, nanosec )
{
    return value;
}
UMBA_DEFINE_USER_LITERAL_TYPE_END( )

UMBA_DEFINE_USER_LITERAL_TYPE_BEGIN( NanosecInterval, microsec )
{
    return value*1000;
}
UMBA_DEFINE_USER_LITERAL_TYPE_END( )

UMBA_DEFINE_USER_LITERAL_TYPE_BEGIN( NanosecInterval, millisec )
{
    return value*1000*1000;
}
UMBA_DEFINE_USER_LITERAL_TYPE_END( )



// Usage:

NanosecInterval nsInterval = nanosec(100);
NanosecInterval nsInterval = microsec(50);
NanosecInterval nsInterval = millisec(50);


#endif

