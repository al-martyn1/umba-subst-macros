#pragma once

//-----------------------------------------------------------------------------

/*! \file
    \brief Базовые определения для интерфейсов, и связанные с ним определения

    Версия COM для нищих.

    Предназначена для использования в MCU.

    Предназначена для возможности получения дополнительных возможностей через указатель на базовый интерфейс.

    Никакого подсчета ссылок нет - объекты либо создаются статически, либо создаются однократно при (пере)инициализации
    устройства, и при повторных инициализациях предыдущая версия удаляется тупо через delete.
*/

//-----------------------------------------------------------------------------

/*!
    \ingroup UMBA_CORE
    \addtogroup UMBA_INTERFACES
 */

/*!
    \ingroup UMBA_INTERFACES
    \addtogroup UMBA_INTERFACES_DEF
    @{
 */

//-----------------------------------------------------------------------------

#if defined(UMBA_CXX_HAS_STD11)
    #include <stdint.h>
#endif

#include "errors.h"

/*! \def UMBA_INTERFACE
    Определяет UMBA-интерфейс
 */
#ifndef UMBA_INTERFACE
    #if (_MSC_VER >= 1100) && defined(__cplusplus)
        //#define UMBA_INTERFACE struct 
        #define UMBA_INTERFACE struct 
        //__declspec(novtable)
    #else
        #define UMBA_INTERFACE struct
    #endif
#endif

/*! \def UMBA_INHERITS
    Указывает наследование от интерфейса
 */
#ifndef UMBA_INHERITS
    //#define UMBA_INHERITS public
    #define UMBA_INHERITS public virtual
#endif

/*! \def UMBA_IMPLEMENTS
    Указывает реализацию интерфейса
 */
#ifndef UMBA_IMPLEMENTS
    //#define UMBA_IMPLEMENTS public
    #define UMBA_IMPLEMENTS public virtual
#endif



namespace umba
{

typedef uint32_t  InterfaceId; //!< Идентификатор (ID) интерфейса, CRC32 от обычного GUID



//! Прокидывает реализацию семейства методов queryInterface в класс BaseClass
#define UMBA_DELEGATE_QUERY_INTERFACE(BaseClass)               \
                                                               \
    template<typename TInterface>                              \
    umba::Result<TInterface*> queryInterface()                 \
    {                                                          \
        return BaseClass::queryInterface<TInterface>();        \
    }                                                          \
                                                               \
    template<typename TInterface>                              \
    umba::Error queryInterface( TInterface **p)                \
    {                                                          \
        return BaseClass::queryInterface<TInterface>(p);       \
    }                                                          \
                                                               \
    template<typename TInterface>                              \
    umba::Error queryInterface( TInterface* &p)                \
    {                                                          \
        return queryInterface<TInterface>(&p);                 \
    }


//! Реализация семейства методов queryInterface
// #define UMBA_IMPLEMENT_QUERY_INTERFACE_TEMPLATE_VERSION()
#define UMBA_IMPLEMENT_QUERY_INTERFACE()                                      \
    template<typename TInterface>                                             \
    umba::Result<TInterface*> queryInterface()                                \
    {                                                                         \
        if (auto res = queryInterfaceImpl(TInterface::interfaceId))           \
        {                                                                     \
            TInterface *pInterface = static_cast<TInterface*>(res.value);     \
            return pInterface;                                                \
        }                                                                     \
        else                                                                  \
        {                                                                     \
            return umba::Error(res);                                          \
        }                                                                     \
    }                                                                         \
                                                                              \
    template<typename TInterface>                                             \
    umba::Error queryInterface( TInterface **p)                               \
    {                                                                         \
        if (!p)                                                               \
            return errors::invalid_param;                                     \
                                                                              \
        if (auto res = queryInterface<TInterface>())                          \
        {                                                                     \
            *p = static_cast<TInterface*>(res.value);                         \
            return umba::errors::ok;                                          \
        }                                                                     \
        else                                                                  \
        {                                                                     \
            return umba::Error(res);                                          \
        }                                                                     \
    }                                                                         \
                                                                              \
    template<typename TInterface>                                             \
    umba::Error queryInterface( TInterface* &p)                               \
    {                                                                         \
        if (auto res = queryInterface<TInterface>())                          \
        {                                                                     \
            p = static_cast<TInterface*>(res.value);                          \
            return umba::errors::ok;                                          \
        }                                                                     \
        else                                                                  \
        {                                                                     \
            return umba::Error(res);                                          \
        }                                                                     \
    }


//! Реализация шаблонного семейства методов queryInterface
#define UMBA_IMPLEMENT_QUERY_INTERFACE_TEMPLATE_VERSION()  UMBA_IMPLEMENT_QUERY_INTERFACE()


//! Задаёт идентификатор интерфейса
#define UMBA_DECLARE_INTERFACE_ID(id)  static const umba::InterfaceId interfaceId = id


//! Начинает реализацию таблицы интерфейсов, поддерживаемых классом
#define UMBA_BEGIN_INTERFACE_MAP()                                             \
                   virtual                                                     \
                   umba::Result<void*> queryInterfaceImpl( umba::InterfaceId id ) override  \
                   {                                                           \
                       switch(id)                                              \
                          {                                                    \
                              case umba::IUnknown::interfaceId : return (void*)static_cast<umba::IUnknown*>(this);

//! Начинает реализацию таблицы интерфейсов, поддерживаемых реализацией
/*! Расширенная таблица интерфейсов для случаев сложного и неоднозначного наследования.
    Тип mainInterfaceType будет использоваться для промежуточного кастинга для разрешения неоднозначностей.
 */
#define UMBA_BEGIN_INTERFACE_MAP_EX(mainInterfaceType)                         \
                   virtual                                                     \
                   umba::Result<void*> queryInterfaceImpl( umba::InterfaceId id ) override \
                   {                                                           \
                       switch(id)                                              \
                          {                                                    \
                              case umba::IUnknown::interfaceId      : return (void*)static_cast<umba::IUnknown*>(static_cast<mainInterfaceType*>(this));  \
                              case mainInterfaceType :: interfaceId : return (void*)static_cast< mainInterfaceType *>(this);

//! Элемент таблицы интерфейсов
#define UMBA_IMPLEMENT_INTERFACE(IterfaceType)    case IterfaceType :: interfaceId : return (void*)static_cast< IterfaceType * >(this);

//! Элемент расширенной таблицы интерфейсов. Тип TypeThrough будет использоваться для промежуточного кастинга для разрешения неоднозначностей.
#define UMBA_IMPLEMENT_INTERFACE_EX(IterfaceType, TypeThrough) case IterfaceType :: interfaceId : return (void*)static_cast< IterfaceType * >(static_cast< TypeThrough * >(this));

//! Завершает реализацию таблицы интерфейсов, поддерживаемых классом
#define UMBA_END_INTERFACE_MAP()                                                      \
                        default: return umba::Error(umba::errors::unknown_interface); \
                       }                                                              \
                   }
                   

//! Возвращает идентификатор интерфейса по типу интерфейса
template<typename InterfaceType>
InterfaceId interfaceIdOf(InterfaceType *pInterface)
{
    return InterfaceType::interfaceId;
}

//! Возвращает идентификатор интерфейса по типу интерфейса
template<typename InterfaceType>
InterfaceId interfaceIdOf(InterfaceType **ppInterface)
{
    return InterfaceType::interfaceId;
}







//! Базовый интерфейс IUnknown
/*! 
    Версия COM для нищих.

    Предназначена для использования в MCU.

    Предназначена для возможности получения дополнительных возможностей через указатель на базовый интерфейс.

    Никакого подсчета ссылок нет - объекты либо создаются статически, либо создаются однократно при (пере)инициализации
    устройства, и при повторных инициализациях предыдущая версия удаляется тупо через delete.

 */
// UMBA_INTERFACE 
struct IUnknown
{
    UMBA_DECLARE_INTERFACE_ID(0x00000000);

    //! Объявление реализации queryInterface
    virtual
    Result<void*> queryInterfaceImpl( InterfaceId ) = 0;

    UMBA_IMPLEMENT_QUERY_INTERFACE_TEMPLATE_VERSION()

    #ifndef UMBA_INTERFACE_NO_VDTOR
    //! Виртуальный деструктор.
    /*! Так как у нас для MCU в основном статика, то виртуальность деструктора не особо актуальна,
        но некоторые ругаются. Но в релизе можно сэкономить пару байтов в VTBL.
     */
    virtual
    ~IUnknown()
    {
    }
    #endif

/*
    template<typename TInterface>
    Result<TInterface*> queryInterface()
    {
        if (auto res = queryInterfaceImpl(TInterface::interfaceId))
        {
            TInterface *pInterface = static_cast<TInterface*>(res.value);
            return pInterface;
        }
        else 
        {
            return Error(res);
        }
    }

    template<typename TInterface>
    Error queryInterface( TInterface **p)
    {
        if (!p)
            return errors::invalid_param;

        if (auto res = queryInterface<TInterface>())
        {
            *p = static_cast<TInterface*>(res.value);
            return errors::ok;
        }
        else 
        {
            return Error(res);
        }
    }

    template<typename TInterface>
    Error queryInterface( TInterface* &p)
    {
        if (auto res = queryInterface<TInterface>())
        {
            p = static_cast<TInterface*>(res.value);
            return errors::ok;
        }
        else 
        {
            return Error(res);
        }
    }
*/

}; // UMBA_INTERFACE IUnknown


}; // namespace umba

//-----------------------------------------------------------------------------

// End of UMBA_INTERFACES_DEF
/*! @} */

//-----------------------------------------------------------------------------




