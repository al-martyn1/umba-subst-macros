#pragma once

//----------------------------------------------------------------------------

/*! \file
    \brief Универсальная реализация UMBA_ASSERT
*/

//----------------------------------------------------------------------------

#include "umba/zz_detect_environment.h"
#include "umba/preprocessor.h"



// Подключаем заголовки, зависимые от платформы

#if defined(UMBA_QT_USED)

    //#include <>

#elif defined(UMBA_WIN32_USED) /* Win32 без Qt */

    #include <crtdbg.h>

#elif defined(UMBA_MCU_USED)

    //#include <>

#else

    #error "None of UMBA_*_USED macro defined, target not detected"

#endif




//---------------------------------------------------------
/*! \def UMBA_ASSERT(expr)
    Проверка условия и аварийный выход (или сообщение с последующим продолжением работы).

    При компиляции под Qt или Win32 будут вызваны соответствующие функции Qt/Windows, отображающие диалог с сообщением об ошибке.

    При компиляции для железа и работе под отладчиком последний должен показать место, где произошел assert.
    \note Не факт что это всегда работает.
*/

#if defined(UMBA_QT_USED)

    #define UMBA_ASSERT( statement )         Q_ASSERT(statement)

#elif defined(UMBA_WIN32_USED) /* Win32 без Qt */

    #define UMBA_ASSERT( statement )         _ASSERTE(statement)

#else /* Подразумевается UMBA_MCU_USED */

    #define UMBA_ASSERT( statement )         do { if(! (statement) ) { __disable_irq(); while(1){ __BKPT(0xAB); if(0) break;} }  } while(0)

#endif

//---------------------------------------------------------
//! UMBA_ASSERT_FAIL срабатывает всегда, и ставится туда, куда по идее, никогда попадать не должны
#define UMBA_ASSERT_FAIL()           UMBA_ASSERT( 0 )



//---------------------------------------------------------
//! Реализация assert'а времени компиляции
#define UMBA_STATIC_ASSERT_MSG(condition, msg)   typedef char umba_static_assertion_##msg[(condition)?1:-1]
//! @cond Doxygen_Suppress_Not_Documented
#define UMBA_STATIC_ASSERT3(X, L)                UMBA_STATIC_ASSERT_MSG(X, at_line_##L)
#define UMBA_STATIC_ASSERT2(X, L)                UMBA_STATIC_ASSERT3(X, L)
//! @endcond

//---------------------------------------------------------
//! UMBA_STATIC_ASSERT - assert времени компиляции
#define UMBA_STATIC_ASSERT(X) UMBA_STATIC_ASSERT2(X, __LINE__)






