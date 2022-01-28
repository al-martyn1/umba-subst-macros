#pragma once

/*!
\file
\brief Подсистема time_service
*/

#include "umba/zz_detect_environment.h"
#include "umba/preprocessor.h"
#include <stdint.h>


#if defined(UMBA_WIN32_USED)

    // Ничего дополнительно подключать не требуется

#elif defined(UMBA_LINUX_USED)

    // Ничего дополнительно подключать не требуется

#elif defined(UMBA_QT_USED)

    // Ничего дополнительно подключать не требуется

#elif defined(UMBA_FREERTOS_USED)

    // Ничего дополнительно подключать не требуется

#else
#endif


// umba::timeservice::
namespace umba{
namespace time_service{


    #ifdef DOXYGEN_ONLY

        //! Platform specific type for timer ticks representation, Usually it's defined as uint32_t, DWORD etc
        typedef PlatformSpecific   TimeTick;
        //! Platform specific type for hi-resolution timer ticks representation, Usually it's defined as uint32_t, DWORD etc
        typedef uint64_t           HiresTimeTick;

    #else

        typedef uint64_t           HiresTimeTick; //!< Тики высокого разрешения

        #if defined(UMBA_WIN32_USED)
            typedef DWORD      TimeTick;          //!< Тики в милисекундах
        #elif defined(UMBA_FREERTOS_USED)
            typedef TickType_t TimeTick;          //!< Тики в милисекундах
        #else
            typedef uint32_t   TimeTick;          //!< Тики в милисекундах
        #endif

    #endif



    //! Инициализация сервиса времени (и его запуск). 
    /*! Требуется только на голой системе. Для FreeRTOS инициализация происходит при старте шедулера */
    void init();

    //! Запуск сервиса времени. Реализовано только на голом железе
    void start();

    //! Остановка сервиса времени. Реализовано только на голом железе
    void stop();
    
    //! Перевод системных тиков в милисекунды
    TimeTick ticksToMs(TimeTick ticks);
    //! Перевод милисекунд в системных тики
    TimeTick msToTicks(TimeTick ms);

    //! Текущее время (тик), приведенное к милисекундам
    TimeTick getCurTimeMs();

    //! Текущее время (тик) в микросекундах
    HiresTimeTick getCurTimeHires();

    //! Возвращает сырые Hi-Res тики 
    void getCurTimeHiresRaw( TimeTick* pHigh, TimeTick *pLow, TimeTick *pModule);

    //! Что-то считает
    HiresTimeTick calcCurTimeHires( TimeTick hPart, TimeTick lPart, TimeTick module );

    //! Текущее время (тик) приведенный к милисекундам - для совместимости со старым кодом
    UMBA_DECLARE_DEPRECATED_FN( TimeTick getCurTime() );

    //! Производит задержку выполнения
    /*! Не совместима с прерываниями и не должна вызываться в них */
    void  delayMs(TimeTick deltaMs);

    //! Производит задержку выполнения (устаревшее имя), см также delayMs
    UMBA_DECLARE_DEPRECATED_FN( void delay_ms(TimeTick deltaMs) );


} // namespace time_service
} // namespace umba
// umba::timeservice::

