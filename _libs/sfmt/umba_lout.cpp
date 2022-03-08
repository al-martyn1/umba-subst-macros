/*! \file 
    \brief Реализация глобального объекта потока LOG вывода umba::lout

    USE_UMBA_LOUT_UART - Если хотим срать в COM-порт, тот нужно задать тут его имя/номер (uart1)

    USE_UMBA_LOUT_SWV  - Если хотим срать в отладочный output Keil'а - меню View/Serial Windows/Debug (printf viewer)

    USE_UMBA_LOUT_COUT - если хотим срать в консольный std::cout (Win/Linux)
    USE_UMBA_LOUT_CERR - если хотим срать в консольный std::cout (Win/Linux)

 */



#include "umba/umba.h"
#include "umba/stl.h"
#include "umba/simple_formatter.h"

#if defined(USE_UMBA_LOUT)

    #if defined(UMBA_MCU_USED)
        #include "uart/uart_handle.h"
    #endif

    #include "umba/i_char_writer.h"
    
#endif


namespace umba{

#if defined(USE_UMBA_LOUT_UART)

    umba::LegacyUartCharWriter<2048>   charWritter = umba::LegacyUartCharWriter<2048>( uart :: USE_UMBA_LOUT_UART )

#elif defined(USE_UMBA_LOUT_SWV)

    umba::SwvCharWritter               charWritter;

#elif defined(USE_UMBA_LOUT_COUT)

        umba::StdStreamCharWriter    charWritter(std::cout);

#elif defined(USE_UMBA_LOUT_CERR)

        umba::StdStreamCharWriter    charWritter(std::cerr);

#endif


#if defined(UMBA_LOUT_USED)

    umba::SimpleFormatter          lout(&charWritter);

#endif

} // namespace umba

