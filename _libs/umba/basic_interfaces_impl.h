/*! \file
    \brief Реализация некоторых базовых интерфейсов
*/

//-----------------------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------------------

#include "basic_interfaces.h"

#include <utility>

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
namespace umba
{




//-----------------------------------------------------------------------------
//! Базовая реализация IPingRequestGenerator
struct PingRequestGeneratorImplBase : UMBA_IMPLEMENTS IPingRequestGenerator
{

    //! Задает период пингов, возвращает предыдущее значение
    virtual
    umba::time_service::TimeTick setPingPeriod( umba::time_service::TimeTick t ) override
    {
        std::swap( m_pingPeriod, t );
        return t;
    }

    //! Возвращает период пингов
    virtual
    umba::time_service::TimeTick getPingPeriod( ) override
    {
        return m_pingPeriod;
    }


protected:

    umba::time_service::TimeTick  m_pingPeriod = 0;   //!< Период пинга 

}; // struct PingRequestGeneratorImplBase

//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
//! Базовая реализация IPollCapable
struct SimplePollCapableImplBase : public IPollCapable
{
    UMBA_BEGIN_INTERFACE_MAP( )
         UMBA_IMPLEMENT_INTERFACE( IPollCapable )
    UMBA_END_INTERFACE_MAP()

}; // struct SimpleIdleCapableImplBase

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
//! Реализация IPollCapable
/*! Адаптирует функциональный объект к интерфейсу IPollCapable
 */
template< typename THandler >
struct SimplePollCapable : public SimplePollCapableImplBase
{
    //! Конструктор, принимает обработчик события
    SimplePollCapable( const THandler &h ) : m_handler(h) {}

    //! Полировка
    virtual
    void poll() override
    {
        m_handler();
    }

    //! Готовность к полировке
    virtual
    bool isReadyForPoll() override
    {
         //#if defined(UMBA_RTKOS_OS)
         //return UMBA_RTKOS_OS->isStarted();
         //#else
         return true;
         //#endif
    }

protected:

    THandler m_handler;  //!< Сохранённый обработчик

}; // struct SimpleIPollCapable

//-----------------------------------------------------------------------------
//! Создаёт экземпляр адаптера IPollCapable для функционального объекта
template< typename THandler >
inline
SimplePollCapable<THandler> makeSimplePollCapable( const THandler &handler )
{
    return SimplePollCapable<THandler>(handler);
}

//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
//! Базовая реализация ждуна IIdleCapable
struct SimpleIdleCapableImplBase : public IIdleCapable
{
    UMBA_BEGIN_INTERFACE_MAP( )
         UMBA_IMPLEMENT_INTERFACE( IIdleCapable )
    UMBA_END_INTERFACE_MAP()

}; // struct SimpleIdleCapableImplBase

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
//! Реализация ждуна IIdleCapable
/*! Адаптирует функциональный объект к интерфейсу IIdleCapable
 */
template< typename THandler >
struct SimpleIdleCapable : public SimpleIdleCapableImplBase
{
    //! Конструктор, принимает обработчик события
    SimpleIdleCapable( const THandler &h ) : m_handler(h) {}

    //! Событие onЖду
    virtual
    bool onIdle() override
    {
        return m_handler();
    }

protected:

    THandler m_handler; //!< Сохранённый обработчик

}; // struct SimpleIdleCapable

//-----------------------------------------------------------------------------
//! Создаёт экземпляр адаптера IIdleCapable для функционального объекта
template< typename THandler >
inline
SimpleIdleCapable<THandler> makeSimpleIdleCapable( const THandler &handler )
{
    return SimpleIdleCapable<THandler>(handler);
}

//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
//! Базовая реализация обработчика событий таймера ITimerHandler
struct SimpleTimerHandlerImplBase : public ITimerHandler
{
    UMBA_BEGIN_INTERFACE_MAP( )
         UMBA_IMPLEMENT_INTERFACE( ITimerHandler )
    UMBA_END_INTERFACE_MAP()

}; // struct SimpleTimerHandlerImplBase

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
//! Реализация обработчика событий таймера ITimerHandler
/*! Адаптирует функциональный объект к интерфейсу ITimerHandler
 */
template< typename THandler >
struct SimpleTimerHandler : public SimpleTimerHandlerImplBase
{
    //! Конструктор, принимает обработчик события
    SimpleTimerHandler( const THandler &h ) : m_handler(h) {}

    //! Событие таймера
    virtual
    void onTimer( unsigned eventId ) override
    {
        m_handler( static_cast<ITimerHandler*>(this), eventId );
    }

protected:

    THandler m_handler; //!< Сохранённый обработчик

}; // struct SimpleTimerHandlerImplBase

//-----------------------------------------------------------------------------
//! Создаёт экземпляр адаптера ITimerHandler для функционального объекта
template< typename THandler >
inline
SimpleTimerHandler<THandler> makeSimpleTimerHandler( const THandler &handler )
{
    return SimpleTimerHandler<THandler>(handler);
}

//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------

} // namespace umba


//-----------------------------------------------------------------------------



