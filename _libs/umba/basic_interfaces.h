/*! \file
\brief Базовые интерфейсы
*/

#pragma once

#include "interface.h"
#include "umba/time_service.h"

//-----------------------------------------------------------------------------

/*!
    \ingroup UMBA_CORE
    \addtogroup UMBA_INTERFACES
 */

/*!
    \ingroup UMBA_INTERFACES
    \addtogroup UMBA_INTERFACES_BASE_INTERFACES
    @{
 */

//-----------------------------------------------------------------------------
namespace umba
{


//-----------------------------------------------------------------------------
//! Интерфейс полируемого
// UMBA_INTERFACE 
struct IPollCapable : UMBA_INHERITS umba::IUnknown
{

    UMBA_DECLARE_INTERFACE_ID(0xB54FCCA9);

    //! Метод для полировки/ Возвращает true, если были произведены какие-то действия. Зависит от конкретной реализации, и является простым способом что-то сообщить наверх. После отладки тупо игнорируется.
    virtual
    bool poll() = 0;

    //! Возвращает true, если объект готов к полировке
    virtual
    bool isReadyForPoll() = 0;

}; // UMBA_INTERFACE IPollCapable

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
//! Интерфейс ждуна
//UMBA_INTERFACE 
struct IIdleCapable : UMBA_INHERITS umba::IUnknown
{

    UMBA_DECLARE_INTERFACE_ID(0xD001476E);

    //! В некоторых случаях ждун может остановить ожидание, вернув false
    virtual
    bool onIdle() = 0;

}; // UMBA_INTERFACE IIdleCapable

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
//! "Something is complete" event handler UMBA_INTERFACE
//UMBA_INTERFACE 
struct ICompletionHandler : UMBA_INHERITS umba::IUnknown
{

    UMBA_DECLARE_INTERFACE_ID(0xA1A1E85B);

    //! Called when something is done.
    virtual
    void onComplete( unsigned customEventId //!< Some user defined code
                   ) = 0;

}; // UMBA_INTERFACE ICompletionHandler

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
//! Обработчик события таймера
/* Каждый обработчик может обрабатывать несколько таймеров
   различними ID.
   ID задается при создании таймера.
 */
//UMBA_INTERFACE 
struct ITimerHandler : UMBA_INHERITS umba::IUnknown
{

    UMBA_DECLARE_INTERFACE_ID(0xF10B5BD3);

    /*! Called when timer event generated
    */
    virtual
    void onTimer( unsigned eventId ) = 0;

}; // UMBA_INTERFACE ITimerHandler

//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
//! Обработчик запросов пинга
//UMBA_INTERFACE 
struct IPingRequestHandler : UMBA_INHERITS umba::IUnknown
{

    UMBA_DECLARE_INTERFACE_ID(0x800459C3);

    //! Можно ли пинговать, или чем-то занят?
    virtual
    bool isReadyForPing() = 0;

    //! Called when ping request generated
    virtual
    void onPing( ) = 0;

}; // UMBA_INTERFACE IPingRequestHandler

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
//! Интерфейс генератора запросов пинга
/*! Сам по себе интерфейс ничего не говорит о том, 
    как он генерирует события - по аппаратному таймеру,
    например, или это объект, который надо полировать.
    Также этот интерфейс ничего не говорит о том, кто и как обрабатывает
    запросы пингов
 */
//UMBA_INTERFACE 
struct IPingRequestGenerator : UMBA_INHERITS umba::IUnknown
{

    UMBA_DECLARE_INTERFACE_ID(0x6A8CA481);

    //! Задает период пингов, 0 - пинги не генерятся. Пинги обычно генерятся только если никакого другого обмена с устройством не производится
    virtual
    umba::time_service::TimeTick setPingPeriod( umba::time_service::TimeTick t ) = 0;

    //! Возвращает период пингов
    virtual
    umba::time_service::TimeTick getPingPeriod( ) = 0;

}; // UMBA_INTERFACE ICompletionHandler

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------

} // namespace umba


//-----------------------------------------------------------------------------

// End of UMBA_INTERFACES_DEF
/*! @} */

//-----------------------------------------------------------------------------

