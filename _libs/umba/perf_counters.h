/*!
\file
\brief Подсистема perf_counters - счетчики производительности

    \section perf_counters_basics Общие сведения

    Тип счетчика производительности (PerfTick) зависит от системы.
    Для STM32 это 32х битный тип, для Win32 - 64х битный тип

    Максимальные величины, которые можно получать на 32х битном типе на STM32:

    0x7FFFFFFF*2/ 72000000 = 59.65 сек
    0x7FFFFFFF*2/168000000 = 25.56 сек

    STM32F7 - 216 МГц
    STM32H7 - 400 МГц

    0x7FFFFFFF*2/400000000 = 10 сек


    bool countersAvailable() - счетчики доступны

    PerfTick queryCounter()  - значение счетчика

    Для подсчета 


    https://www.compel.ru/lib/ne/2018/1/3-stm32-semeystva-h7-s-novyimi-vozmozhnostyami-k-novyim-vershinam
    https://pikabu.ru/tag/Stm32/hot
    https://spb.terraelectronica.ru/news/5552


    Алгоритм скользящего среднего (Simple Moving Average) - https://habr.com/ru/post/134375/
    https://ru.wikipedia.org/wiki/%D0%A1%D0%BA%D0%BE%D0%BB%D1%8C%D0%B7%D1%8F%D1%89%D0%B0%D1%8F_%D1%81%D1%80%D0%B5%D0%B4%D0%BD%D1%8F%D1%8F

    Точное вычисление средних и ковариаций методом Уэлфорда - https://habr.com/ru/post/333426/

    Алгоритм Кэхэна - https://ru.wikipedia.org/wiki/%D0%90%D0%BB%D0%B3%D0%BE%D1%80%D0%B8%D1%82%D0%BC_%D0%9A%D1%8D%D1%85%D1%8D%D0%BD%D0%B0

    https://ru.wikipedia.org/wiki/%D0%A1%D1%80%D0%B5%D0%B4%D0%BD%D0%B5%D0%B5_%D0%B0%D1%80%D0%B8%D1%84%D0%BC%D0%B5%D1%82%D0%B8%D1%87%D0%B5%D1%81%D0%BA%D0%BE%D0%B5




    Критическая секция - это участок кода, который должен защищен от прерывания другой задачей, прерыванием и тп.

    Критическая секция - это также объект синхронизации, позволяющий предотвратить одновременное выполнение.

    Защита критической секции кода в МК с использованием механизма запрета прерываний не позволяет сделать 
    отдельный объект синхронизации, но для совместимости с Windows без него не обойтись.

    Обычно захват объекта критической секции называется Enter, освобождение - Leave, но
    для совместимости с классом AutoLock используются имена lock и unlock.

    \warning Использование методов lock и unlock напрямую запрещено, следует использовать 
    класс \ref umba::AutoLock или макрос #UMBA_CRITICAL_SECTION / #UMBA_CRITICAL_SECTION_EX.

    \section perf_counters_conf Настройка
    В любой системе класс критической секции называется CriticalSection, и используется соответствующая версия.
    Это справедливо и для случая использования FreeRTOS, за исключением того, что класс InterruptCriticalSection
    также доступен. Для выбора класса InterruptCriticalSection для использования по умолчанию требуется задать 
    макрос #UMBA_FORCE_USE_INTERRUPT_CS

    \note
    Реализация критической секции для FreeRTOS использует syscall'ы 
    \b taskENTER_CRITICAL_FROM_ISR, \b taskENTER_CRITICAL, \b taskEXIT_CRITICAL_FROM_ISR и \b taskEXIT_CRITICAL
    и предоставляет гарантии этой ОС. Если ваши прерывания настроены на высокий приоритет, то 
    следует использовать InterruptCriticalSection, но следует учитывать, что в таких прерываниях запрещено
    использование syscall'ов.

    По умолчанию предоставляется для использования готовый объект критической секции
    \b umba::globalCriticalSection, если не определен макрос \b UMBA_NO_GLOBAL_CS.

    Некоторые библиотеки расчитывают на существование глобальной критической секции. В этом случае, 
    если определен макрос \b UMBA_NO_GLOBAL_CS, следует определить такой объект самостоятельно.




*/

#pragma once

#include "umba/zz_detect_environment.h"
#include "umba/preprocessor.h"
#include <stdint.h>


#if defined(UMBA_WIN32_USED)

    // Ничего дополнительно подключать не требуется
#elif defined(UMBA_QT_USED)
    // Ничего дополнительно подключать не требуется
#elif defined(UMBA_FREERTOS_USED)
    // Ничего дополнительно подключать не требуется
#else
#endif


/*

BOOL WINAPI QueryPerformanceCounter(
  _Out_ LARGE_INTEGER *lpPerformanceCount
);


LARGE_INTEGER liFrequency = {0};

// Get the Frequency
if(QueryPerformanceFrequency(&liFrequency))
{

    // Start Timing
    LARGE_INTEGER liStart = {0};
    if(QueryPerformanceCounter(&liStart))           
    {
        // Do Stuff

        // Get Time spent...
        LARGE_INTEGER liStop = {0};
        if(QueryPerformanceCounter(&liStop))    
        {               
            LONGLONG llMilliseconds = (LONGLONG)((liStop.QuadPart - liStart.QuadPart) * 1000.0 / liFrequency.QuadPart);
            printf("time ellapsed ms:%lld\n", llMilliseconds);
        }
    }       
}
*/

namespace umba{

//!
namespace perf_counters{


} // namespace perf_counters
} // namespace umba

