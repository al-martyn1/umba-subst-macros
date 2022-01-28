/*!
\file
\brief Автоматический захват и освобождение ресурса.
Объект-ресурс должен предоставлять методы lock и unlock.
*/

#pragma once

#include "umba/zz_detect_environment.h"

#include "umba/stl.h"


namespace umba{

/*
namespace details
{
  template<typename _Tp>
    struct remove_reference
    { typedef _Tp   type; };

  template<typename _Tp>
    struct remove_reference<_Tp&>
    { typedef _Tp   type; };

  #if defined(UMBA_CXX_HAS_STD11)
  template<typename _Tp>
    struct remove_reference<_Tp&&>
    { typedef _Tp   type; };
  #endif

}
*/


//! Класс-обертка для автоматического захвата объектов блокировки.
/*! Конструктор захватывает передаваемый ему объект и сохраняет ссылку на него.
    Деструктор освобождает захваченный ранее объект.
 */
template<typename LockObject>
class AutoLock
{

    public:
        
        //! Сохраняет ссылку на объект блокировки и производит его захват
        AutoLock( LockObject &lockObject ) 
            : m_lockObject(lockObject)
        {
            m_lockObject.lock();
        }

        //! Освобождает объект блокировки
        ~AutoLock()
        {
            m_lockObject.unlock();
        }
      
    private:
      
        LockObject &m_lockObject;

}; // class AutoLock


} // namespace umba



