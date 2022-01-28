#pragma once

/*! 
    \file
    \brief Разная всячина в C++ стиле
 */

// umba extentions looks like an std

#include "umba.h"

#include <algorithm>

// umba::cpp::

namespace umba
{

//! Разная всячина в C++ стиле
namespace cpp
{

/*! \ingroup UMBA_CORE
 *  \addtogroup UMBA_CPP
 *  @{
 */


//----------------------------------------------------------------------------
//! back_insert_iterator итератор для сырого массива
/*! 
    \tparam ItemType Тип итерируемых элементов
 */

//! back_insert_iterator итератор для сырого массива
template<typename ItemType>
struct array_back_insert_iterator
{
    typedef ItemType        value_type;  //!< Тип итерируемых элементов
    typedef std::size_t     size_type;   //!< Тип размера

    //! Конструктор из сырого указателя и количества элементов
    array_back_insert_iterator( ItemType *pArray   //!< Указатель на массив/буфер для заполнения 
                              , size_type numItems //!< Количество доступных для заполнения элементов
                              )
    : m_pArray(pArray)
    , m_numItems(numItems)
    , m_curPos(0)
    {
    }

    //! Конструктор копирования
    array_back_insert_iterator( const array_back_insert_iterator &it //!< Копируемый итератор
                              )
    : m_pArray  (it.m_pArray  )
    , m_numItems(it.m_numItems)
    , m_curPos  (it.m_curPos  )
    {
    }

    //! Оператор разыменования. Возвращает ссылку на текущий элемент, или, если индекс выходит за пределы массива - ссылку на последний элемент массива
    ItemType &operator *()
    {
        if (m_curPos>=m_numItems)
        {
            //++m_curPos;
            return m_pArray[m_numItems-1]; // return last item as dummy item (we don't use dummy in an our class to make it more lightweight)
                                            // In case of the overflow it is not so serious is the last item valid or not
        }

        //return m_pArray[m_curPos++];
        return m_pArray[m_curPos];
    }

    //! Префиксный инкремент
    array_back_insert_iterator<ItemType> operator++()
    {
        m_curPos++;
        return *this;
    }

    //! Постфиксный инкремент
    array_back_insert_iterator<ItemType> operator++( int /*! Маркер постфиксного оператора */ )
    {
        array_back_insert_iterator<ItemType> res = *this;
        m_curPos++;
        return res;
    }

    //! Возвращает максимальное количество хороших элементов. Последний элемент - перезаписываемый, и не содержит достоверных данных
    size_type max_good() const
    {
        return m_numItems-1;
    }

    //! Возвращает true, если было переполнение
    bool overflow() const
    {
        return m_curPos >= m_numItems;
    }

    //! Возвращает число валидных элементов
    size_type valid_size() const
    {
        if (!overflow())
            return m_curPos;

        return max_good();
    }

    //! Возвращает размер переполнения
    size_type overflow_count() const
    {
        if (!overflow())
            return 0;

        return m_curPos - max_good();;
    }

protected:

    ItemType   *m_pArray;    //!< Указатель на буфер
    size_type   m_numItems;  //!< Количество доступных в буфере элементов
    size_type   m_curPos;    //!< Текущая позиция в буфере

};




//template< typename T, int N > inline
//array_back_insert_iterator<T> array_back_inserter( T (&arr)[N] )  { return array_back_insert_iterator( &arr[0], (std::size_t)N ); }

//! Хелпер для создания array_back_insert_iterator
template< typename T, std::size_t N > inline
array_back_insert_iterator<T> array_back_inserter( T (&arr)[N] )  
   { return array_back_insert_iterator< T >( &arr[0], (std::size_t)N ); }


//template< typename T, int N, typename SomeInt > inline
//array_back_insert_iterator<T> array_back_inserter( T (&arr)[N], SomeInt useOnlySize )  { return array_back_insert_iterator( &arr[0], std::min((std::size_t)N, (std::size_t)useOnlySize) ); }

//! Хелпер для создания array_back_insert_iterator
template< typename T, std::size_t N > inline
array_back_insert_iterator<T> array_back_inserter( T (&arr)[N], std::size_t M )  
   { return array_back_insert_iterator< T >( &arr[0], std::min((std::size_t)N, (std::size_t)M) ); }

//! Хелпер для создания array_back_insert_iterator
template< typename T > inline
array_back_insert_iterator<T> array_back_inserter( const T *pT, std::size_t N )  
   { return array_back_insert_iterator< T >( pT, N ); }

// End of UMBA_CPP
/*! @}*/


} // namespace cpp
} // namespace umba



