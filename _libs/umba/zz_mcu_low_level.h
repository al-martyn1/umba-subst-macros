/*!
\file
\brief Microcontroller Unit Low Level definitions

Всяческие общие штуки, относящиеяся к МК.
В данный момент только для STM32
*/

#pragma once

#include "umba/zz_detect_environment.h"


//---------------------------------------------------------
//
// Барьеры памяти
//
//---------------------------------------------------------
/*! Instruction Synchronization Barrier сбрасывает конвеер инструкций процессора
    и заставляет его перечитать инструкции из мамяти (или кэша).
    Аппаратная оптимизация может переупорядочивать исполнение иструкций, и в случае, 
    когда необходимо гарантировать порядок их выполнения, следует вставлять данный барьер.
*/
#define UMBA_INSTRUCTION_BARRIER()               __ISB()
#ifndef UMBA_DISABLE_DEPRECATED
    //! Legacy версия UMBA_INSTRUCTION_BARRIER()
    #ifndef INSTRUCTION_SYNCHRONIZATION_BARRIER
        #define INSTRUCTION_SYNCHRONIZATION_BARRIER()    UMBA_INSTRUCTION_BARRIER()
    #endif
#endif


//---------------------------------------------------------
/*! Data Memory Barrier гарантирует, что все инструкции обращения к памяти, произведенные до барьера,
    будут выполнятся до инструкций обращения к памяти, заданных после барьера.
    Данный барьер не влияет на порядок и выполнение инструкций, не производящих доступ к памяти.
    Данный барьер не ожидает окончания выполнения инструкций.
    Тут неясно - в коде CMSIS указано, что без гарантий окончания, в даташите на Cortex вроде как с гарантиями.
*/
#define UMBA_DATA_MEMORY_BARRIER()               __DMB()
#ifndef UMBA_DISABLE_DEPRECATED
    //! Legacy версия UMBA_DATA_MEMORY_BARRIER
    #ifndef DATA_MEMORY_BARRIER
        #define DATA_MEMORY_BARRIER()                    UMBA_DATA_MEMORY_BARRIER()
    #endif
#endif


//---------------------------------------------------------
/*! Data Synchronization Barrier аналогичен UMBA_DATA_MEMORY_BARRIER(), но дает гарантии окончания выполнения иструкций
*/
#define UMBA_DATA_SYNCHRONIZATION_BARRIER()      __DSB()
#ifndef UMBA_DISABLE_DEPRECATED
    //! Legacy версия DATA_SYNCHRONIZATION_BARRIER
    #ifndef DATA_SYNCHRONIZATION_BARRIER
        #define DATA_SYNCHRONIZATION_BARRIER()           UMBA_DATA_SYNCHRONIZATION_BARRIER()
    #endif
#endif


//---------------------------------------------------------
/*! Memory Barrier гарантирует, что все инструкции обращения к памяти, произведенные до барьера,
    будут выполнятся до инструкций обращения к памяти, заданных после барьера.
    Данный барьер дает гарантии окончания выполнения иструкций.

    \note В общем случае следует использовать именно его, а не UMBA_DATA_MEMORY_BARRIER 
    или UMBA_DATA_SYNCHRONIZATION_BARRIER. Это несколько излишне аппаратнозависимые
    барьеры, и при их прямом использовании нужно каждый раз вспоминать, какой из них следует использовать.
*/
#define UMBA_MEMORY_BARRIER()                    UMBA_DATA_SYNCHRONIZATION_BARRIER()



//---------------------------------------------------------
//
// Прерывания
//
//---------------------------------------------------------
//! Проверка нахождения кода в обработчике прерывания
#define UMBA_IS_IN_ISR()                         (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk)
#ifndef UMBA_DISABLE_DEPRECATED
    //! Legacy версия UMBA_IS_IN_ISR
    #ifndef IS_IN_ISR
        #define IS_IN_ISR()                              UMBA_IS_IN_ISR()
    #endif
#endif


//---------------------------------------------------------
//! Проверка, разрешены прерывания или нет.
#define UMBA_INTERRUPTS_DISABLED()               __get_PRIMASK()
#ifndef UMBA_DISABLE_DEPRECATED
    //! Legacy определение
    #ifndef ARE_INTERRUPTS_DISABLED
        #define ARE_INTERRUPTS_DISABLED()                UMBA_ARE_INTERRUPTS_DISABLED()
    #endif
#endif


//---------------------------------------------------------
//! Запрет прерываний, в legacy API нет аналога
#define UMBA_DISABLE_IRQ()                       __disable_irq()


//---------------------------------------------------------
//! Разрешение прерываний, в legacy API нет аналога
#define UMBA_ENABLE_IRQ()                       __enable_irq()


//---------------------------------------------------------
//! Объявление функции-обработчика прерывания. Можно использовать как для собственно
//! объявления функции, так и для реализации.
#define UMBA_IRQ_HANDLER(procName)  extern "C" void procName()



namespace umba{

    //! Запрет прерываний
    /*! \return true, если прерывания были запрещены (замаскированы).
        В противном случае, внезапно, возвращается false
     */
    inline bool interruptLock()
    {
        uint32_t isMasked = UMBA_INTERRUPTS_DISABLED(); 
        UMBA_MEMORY_BARRIER();
        UMBA_DISABLE_IRQ();
        return isMasked ? true : false;
    }

    //! Разрешение прерываний
    inline void interruptUnlock( bool wasMasked = false //!< Признак предыдущей маскировки
                               )
    {
        if (!wasMasked)
           UMBA_ENABLE_IRQ();
    }

    //! Проверка, что выполняется, обработчик прерывания или основной код 
    inline bool isInInterrupt()
    {
        return UMBA_IS_IN_ISR() ? true : false;
    }

} // namespace umba



/*
#if defined UMBA_USE_FREERTOS

    // это псевдоним, который, вроде бы, лучше отражает суть
    // т.к. макросы FreeRTOS на самом деле запрещают только прерывания, у которых приоритет ниже 
    // configMAX_SYSCALL_INTERRUPT_PRIORITY ( это должны быть только прерывания от диспетчера и т.п.)
    
    // однако, если ваше прерывание имеет приоритет ниже - оно тоже будет запрещено!
    
    // помните, что меньший номер приоритета соответствует большей важности (0 - самое важное)
    
    // если диспетчер еще не запущен - совершенно логичным образом ничего не происходит
    
    #define ENTER_UNSCHEDULED_SECTION()   if( xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED ) { taskENTER_CRITICAL(); }
    #define LEAVE_UNSCHEDULED_SECTION()   if( xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED ) { taskEXIT_CRITICAL();  }
    
    #define EXIT_UNSCHEDULED_SECTION()    LEAVE_UNSCHEDULED_SECTION()

#endif



*/


