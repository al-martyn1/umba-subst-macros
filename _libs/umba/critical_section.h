/*! \file
    \brief Реализация критической секции.

    \section terms Терминология

    Критическая секция - это участок кода, который должен защищен от прерывания другой задачей, прерыванием и тп.

    Критическая секция - это также объект синхронизации, позволяющий предотвратить одновременное выполнение.

    Защита критической секции кода в МК с использованием механизма запрета прерываний не позволяет сделать 
    отдельный объект синхронизации, но для совместимости с Windows без него не обойтись.

    Обычно захват объекта критической секции называется Enter, освобождение - Leave, но
    для совместимости с классом AutoLock используются имена lock и unlock.

    \warning Использование методов lock и unlock напрямую запрещено, следует использовать 
    класс \ref umba::AutoLock или макрос \b UMBA_CRITICAL_SECTION / \b UMBA_CRITICAL_SECTION_EX.

    \section conf Настройка
    В любой системе класс критической секции называется CriticalSection, и используется соответствующая версия.
    Это справедливо и для случая использования FreeRTOS, за исключением того, что класс InterruptCriticalSection
    также доступен. Для выбора класса InterruptCriticalSection для использования по умолчанию требуется задать 
    макрос \b UMBA_FORCE_USE_INTERRUPT_CS

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
#include "umba/assert.h"
#include "umba/autolock.h"
#include "umba/stl.h"



#if defined(UMBA_MCU_USED) || defined(UMBA_FREERTOS_USED)
    #include "umba/x_mcu_low_level.h"
#endif


/*! 
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Указывает принудительно использовать объект InterruptCriticalSection вне зависимости от того, каким реальным типом является CriticalSection

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

 */
#ifdef DOXYGEN_ONLY
    #define UMBA_FORCE_USE_INTERRUPT_CS
#endif


/*! 
    \ingroup UMBA_LIBS_STATE_MACROS
    \brief Указывает не объявлять глобальный экземпляр критической секции

    По умолчанию предоставляется для использования готовый объект критической секции
    \b umba::globalCriticalSection, если не определен макрос \b UMBA_NO_GLOBAL_CS.

    Некоторые библиотеки расчитывают на существование глобальной критической секции. В этом случае, 
    если определен макрос \b UMBA_NO_GLOBAL_CS, следует определить такой объект самостоятельно.

 */
#ifdef DOXYGEN_ONLY
    #define UMBA_NO_GLOBAL_CS
#endif







//! @cond Doxygen_Suppress_Not_Documented
//! Реализация для UMBA_CRITICAL_SECTION_EX
#define UMBA_CRITICAL_SECTION_EX_IMPL2(criticalSectionType, criticalSectionObj, lineNo)  umba::AutoLock< criticalSectionType > umba_autoLock_##lineNo(criticalSectionObj)
//! Реализация для UMBA_CRITICAL_SECTION_EX
#define UMBA_CRITICAL_SECTION_EX2(criticalSectionType, criticalSectionObj, lineNo)       UMBA_CRITICAL_SECTION_EX_IMPL2(criticalSectionType, criticalSectionObj, lineNo)
//! @endcond

//! \def UMBA_CRITICAL_SECTION_EX(criticalSectionType, criticalSectionObj)
//! Макрос для захвата и автоматического освобождения критической секции
#if defined(UMBA_PP_HAS_COUNTER)
    #define UMBA_CRITICAL_SECTION_EX(criticalSectionType, criticalSectionObj)            UMBA_CRITICAL_SECTION_EX2(criticalSectionType, criticalSectionObj, __COUNTER__)
#else
    #define UMBA_CRITICAL_SECTION_EX(criticalSectionType, criticalSectionObj)            UMBA_CRITICAL_SECTION_EX2(criticalSectionType, criticalSectionObj, __LINE__)
#endif


#if defined(UMBA_CXX_HAS_STD11)
    //! Макрос для захвата и автоматического освобождения критической секции. Не требует указания типа критической секции. Определен при использовании C++11 (Keil - --cpp11)
    //#define UMBA_CRITICAL_SECTION(criticalSectionObj)                                        UMBA_CRITICAL_SECTION_EX( typename umba::details::remove_reference<decltype(criticalSectionObj)>::type, criticalSectionObj)
    #define UMBA_CRITICAL_SECTION(criticalSectionObj)                                    UMBA_CRITICAL_SECTION_EX( typename std::remove_reference<decltype(criticalSectionObj)>::type, criticalSectionObj)
#endif


// Для совместимости с кодом коллег в blocking_circular_buffer.h

// На самом деле лучше использовать umba/legacy.h

/*

#ifndef UMBA_DISABLE_LEGACY_CRITICAL_SECTION_MACROS

    #ifndef ENTER_CRITICAL_SECTION
    
        #define ENTER_CRITICAL_SECTION()    umba::globalCriticalSection.lock()
    
    #endif
    
    #ifndef LEAVE_CRITICAL_SECTION
    
        #define LEAVE_CRITICAL_SECTION()    umba::globalCriticalSection.unlock()
    
    #endif
    
    #ifndef EXIT_CRITICAL_SECTION
    
        #define EXIT_CRITICAL_SECTION()     LEAVE_CRITICAL_SECTION()
    
    #endif

#endif

*/
    

//---------------------------------------------------------

namespace umba{



//---------------------------------------------------------
//! Фейковая критическая секция, ничего не блокирует
class FakeCriticalSection
{
    public:

        FakeCriticalSection() {}

    private:
        template<typename LockObject> friend class AutoLock;
        void lock() {}
        void unlock() {}
        UMBA_NON_COPYABLE_CLASS(FakeCriticalSection)

}; // class FakeCriticalSection



#if defined(UMBA_WIN32_USED)

    //! Реализация критической секции для Win32
    class CriticalSection
    {
            template<typename LockObject> friend class AutoLock;

        public:

            CriticalSection();
            ~CriticalSection();
            UMBA_NON_COPYABLE_CLASS(CriticalSection)

        private:

            void lock();
            void unlock();

            CRITICAL_SECTION m_criticalSection;

    }; // class CriticalSection


#elif defined(UMBA_LINUX_USED)


    //UNDONE: need to make normal implementation
    typedef FakeCriticalSection  CriticalSection;


#elif defined(UMBA_FREERTOS_USED)

    class FreeRtosCriticalSection;

    template<> class AutoLock<FreeRtosCriticalSection>;

    //! Реализация критической секции для FreeRTOS
    class FreeRtosCriticalSection
    {
        public:

            FreeRtosCriticalSection() {}
            UMBA_NON_COPYABLE_CLASS(FreeRtosCriticalSection)

        private:

            friend class AutoLock<FreeRtosCriticalSection>;

            void lock()
            {
                #ifndef UMBA_NDEBUG
                    UMBA_ASSERT_FAIL();
                #endif
            }

            void unlock()
            {
                #ifndef UMBA_NDEBUG
                    UMBA_ASSERT_FAIL();
                #endif
            }

    }; // class FreeRtosCriticalSection

    //! Специализация AutoLock для критической секции FreeRTOS
    template<>
    class AutoLock<FreeRtosCriticalSection>
    {

        public:

            AutoLock(FreeRtosCriticalSection &)
            : m_savedInterruptStatus()
            {
                if (isInInterrupt())
                    m_savedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
                else
                    taskENTER_CRITICAL();
            }

            ~AutoLock()
            {
                if (isInInterrupt())
                    taskEXIT_CRITICAL_FROM_ISR(m_savedInterruptStatus);
                else
                    taskEXIT_CRITICAL();
            }

        private:

            UBaseType_t m_savedInterruptStatus;

    }; // class AutoLock<InterruptCriticalSection>


#endif



#if defined(UMBA_MCU_USED)

    class InterruptCriticalSection;

    template<> class AutoLock<InterruptCriticalSection>;

    //! Реализация критической секции для голого железа на запрете прерываний
    class InterruptCriticalSection
    {
        public:

            InterruptCriticalSection() {}
            UMBA_NON_COPYABLE_CLASS(InterruptCriticalSection)

        private:

            friend class AutoLock<InterruptCriticalSection>;

            void lock()
            {
                #ifndef UMBA_NDEBUG
                    UMBA_ASSERT_FAIL();
                #endif
            }

            void unlock()
            {
                #ifndef UMBA_NDEBUG
                    UMBA_ASSERT_FAIL();
                #endif
            }

            

    }; // class InterruptCriticalSection

    //! Специализация AutoLock для критической секции на запрете прерываний
    template<>
    class AutoLock<InterruptCriticalSection>
    {

        public:

            //! Сохраняет состояние маски прерываний и запрещает их
            AutoLock(InterruptCriticalSection &)
            : m_wasMasked(interruptLock())
            {
            }

            //! Восстаналивает маску прерываний
            ~AutoLock()
            {
             interruptUnlock(m_wasMasked);
            }

        private:

            bool m_wasMasked;

    }; // class AutoLock<InterruptCriticalSection>

#endif


#if defined(UMBA_MCU_USED)
    #if !defined(UMBA_FREERTOS_USED)
        //! Определяет универсальный тип критической секции, независимый от платформы
        typedef InterruptCriticalSection CriticalSection;       //!< Тип критической секции
    #else
        #if defined(UMBA_FORCE_USE_INTERRUPT_CS)
            typedef InterruptCriticalSection CriticalSection;   //!< Тип критической секции
        #else
            typedef FreeRtosCriticalSection CriticalSection;    //!< Тип критической секции
        #endif
    #endif
#endif


//! Глобальная критическая секция, доступная для использования, если не определен макрос \b UMBA_NO_GLOBAL_CS
extern CriticalSection globalCriticalSection;




} // namespace umba


