#pragma once

/*! \file
    \brief Манипуляции с битами 
*/

#include <limits.h>
#include <stdint.h>
#include "umba/i_char_writer.h"
#include "umba/stl.h"

#include <cstring>
#include <algorithm>



// umba::bits::

//-----------------------------------------------------------------------------
namespace umba
{




class SimpleFormatter;

//-----------------------------------------------------------------------------
//! Работа с битами
namespace bits
{



//-----------------------------------------------------------------------------
//! Делает IntTypeT из набора байт в порядке Little Endian
/*! \tparam IntTypeT Тип результирующего целого \param pBytes Указатель на сырые байты \param numBytes Число байт для обработки. 
    \note Нет проверки, сходится ли указанное число байт с размером типа-результата
 */
template< typename IntTypeT > inline
IntTypeT makeIntFromBytesLE( const uint8_t *pBytes, size_t numBytes )
{
    UMBA_ASSERT(pBytes);

    IntTypeT res = 0;
    for(size_t i = 0; i!=numBytes; ++i)
    {
        res <<= 8;
        res |= (IntTypeT)pBytes[i];
    }

    return res;
}

//-----------------------------------------------------------------------------
//! Делает IntTypeT из набора байт в порядке Little Endian
/*! Конвертирует число байт, соответствующее размеру IntTypeT целого.
    \tparam IntTypeT Тип результирующего целого \param pBytes Указатель на сырые байты.
 */
template< typename IntTypeT > inline
IntTypeT makeIntFromBytesLE( const uint8_t *pBytes )
{
    UMBA_ASSERT(pBytes);

    return makeIntFromBytesLE<IntTypeT>(pBytes,sizeof(IntTypeT));
}

//-----------------------------------------------------------------------------
//! Делает IntTypeT из набора байт в порядке Big Endian
/*! \tparam IntTypeT Тип результирующего целого \param pBytes Указатель на сырые байты \param numBytes Число байт для обработки. 
    \note Нет проверки, сходится ли указанное число байт с размером типа-результата
 */
template< typename IntTypeT > inline
IntTypeT makeIntFromBytesBE( const uint8_t *pBytes, size_t numBytes )
{
    UMBA_ASSERT(pBytes);

    const uint8_t *pEnd   = pBytes-1;
    const uint8_t *pBegin = pEnd+numBytes;

    IntTypeT res = 0;
    for(; pBegin!=pEnd; --pBegin)
    {
        res <<= 8;
        res |= (IntTypeT)*pBegin;
    }

    return res;
}

//-----------------------------------------------------------------------------
//! Делает IntTypeT из набора байт в порядке Big Endian
/*! Конвертирует число байт, соответствующее размеру IntTypeT целого.
    \tparam IntTypeT Тип результирующего целого \param pBytes Указатель на сырые байты.
 */
template< typename IntTypeT > inline
IntTypeT makeIntFromBytesBE( const uint8_t *pBytes )
{
    UMBA_ASSERT(pBytes);

    return makeIntFromBytesBE<IntTypeT>(pBytes,sizeof(IntTypeT));
}

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
//! Сериализация целого в память в порядке LSB first - сперва младшие
template< typename IntTypeT > inline
void serializeLsbFirst( IntTypeT val, uint8_t *pBuf, size_t numBytes )
{
    UMBA_ASSERT(pBuf);

    for(size_t i = 0; i!=numBytes; ++i)
    {
        pBuf[i] = (uint8_t)(val&0xFF);
        val>>=8;
    }
}

//-----------------------------------------------------------------------------
//! Сериализация целого в память в порядке MSB first - сперва старшие
template< typename IntTypeT > inline
void serializeMsbFirst( IntTypeT val, uint8_t *pBuf, size_t numBytes )
{
    UMBA_ASSERT(pBuf);
    serializeLsbFirst( val, pBuf, numBytes );
    std::reverse(pBuf, pBuf+numBytes);
}

//-----------------------------------------------------------------------------
//! Сериализация целого в память в порядке LSB first - сперва младшие
template< typename IntTypeT > inline
void serializeLsbFirst( IntTypeT val, uint8_t *pBuf )
{
    serializeLsbFirst( val, pBuf, sizeof(IntTypeT) );
}

//-----------------------------------------------------------------------------
//! Сериализация целого в память в порядке MSB first - сперва старшие
template< typename IntTypeT > inline
void serializeMsbFirst( IntTypeT val, uint8_t *pBuf )
{
    serializeMsbFirst( val, pBuf, sizeof(IntTypeT) );
}

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
//! Десериализация целого из памяти в порядке LSB first - сперва младшие
template< typename IntTypeT >
IntTypeT deserializeLsbFirst( const uint8_t *pBytes, size_t numBytes )
{
    return makeIntFromBytesLE<IntTypeT>( pBytes, numBytes );
}

//-----------------------------------------------------------------------------
//! Десериализация целого из памяти в порядке MSB first - сперва старшие
template< typename IntTypeT >
IntTypeT deserializeMsbFirst( const uint8_t *pBytes, size_t numBytes )
{
    return makeIntFromBytesBE<IntTypeT>( pBytes, numBytes );
}

//-----------------------------------------------------------------------------
//! Десериализация целого из памяти в порядке LSB first - сперва младшие
template< typename IntTypeT >
IntTypeT deserializeLsbFirst( const uint8_t *pBytes )
{
    return makeIntFromBytesLE<IntTypeT>( pBytes );
}

//-----------------------------------------------------------------------------
//! Десериализация целого из памяти в порядке MSB first - сперва старшие
template< typename IntTypeT >
IntTypeT deserializeMsbFirst( const uint8_t *pBytes )
{
    return makeIntFromBytesBE<IntTypeT>( pBytes );
}

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
//! Возвращает количество бит для типа - параметра шаблона
template<typename BitsStorageType, typename BitSetSizeType>
inline
BitSetSizeType getTypeBitsCount()
{
    return sizeof(BitsStorageType) * CHAR_BIT;
}

//-----------------------------------------------------------------------------
//! Возвращает количество установленых бит. Наверное, лучше было бы вместо getSetCount использовать getSetBitsCount
template<typename BitsStorageType>
inline
unsigned getSetCount(BitsStorageType b)
{
    unsigned cnt = 0;
    while (b)
    {
        cnt++;
        b &= (b-1);
    }

    return cnt;
}

//-----------------------------------------------------------------------------
//! Возвращает количество нулевых бит. Наверное, лучше было бы вместо getClearCount использовать getClearBitsCount
template<typename BitsStorageType>
inline
unsigned getClearCount(BitsStorageType b)
{
    const unsigned bitsPerItem = getTypeBitsCount<BitsStorageType, unsigned>();
    return bitsPerItem - calcSetCount(b);
}

//-----------------------------------------------------------------------------
//! Возвращает номер первого нулевого бита (начиная с младших), или число бит в типе всего
template<typename BitsStorageType>
inline
unsigned getFirstClear( BitsStorageType bitsStorage )
{
    const unsigned bitsPerItem = getTypeBitsCount<BitsStorageType, unsigned>();
    for( unsigned i = 0; i!=bitsPerItem; ++i, bitsStorage>>=1 )
    {
        if ((bitsStorage&1)==0)
            return i;
    }

    return bitsPerItem;
}

//-----------------------------------------------------------------------------
//! Возвращает значение с установленым битом, номер которого задан аргументом
template<typename BitsStorageType, typename BitSetSizeType>
inline
BitsStorageType makeBit( BitSetSizeType bitNo )
{
    BitsStorageType bit = 1;
    return bit<<bitNo;
}

//-----------------------------------------------------------------------------
//! Возвращает маску для сброса бита - все биты установлены, кроме заданого
template<typename BitsStorageType, typename BitSetSizeType>
inline
BitsStorageType makeMask( BitSetSizeType bitNo )
{
    return ~makeBit<BitsStorageType, BitSetSizeType>(bitNo);
}

//-----------------------------------------------------------------------------
//! Возвращает значение с установленым старшим битом
template<typename BitsStorageType>
inline
BitsStorageType makeMsb()
{
    const unsigned bitsPerItem = getTypeBitsCount<BitsStorageType, unsigned>();
    //sizeof(BitsStorageType) * CHAR_BIT;
    return makeBit<BitsStorageType, unsigned>(bitsPerItem-1);
}

//-----------------------------------------------------------------------------
//! Возвращает число, в котором установлен только самый старший бит входного числа
inline
uint8_t getMsb( uint8_t u )
{
    u |= u >> 1;
    u |= u >> 2;
    u |= u >> 4;
    return u - (u >> 1);
}

//-----------------------------------------------------------------------------
//! Возвращает число, в котором установлен только самый старший бит входного числа
inline
uint16_t getMsb( uint16_t u )
{
    u |= u >> 1;
    u |= u >> 2;
    u |= u >> 4;
    u |= u >> 8;
    return u - (u >> 1);
}

//-----------------------------------------------------------------------------
//! Возвращает число, в котором установлен только самый старший бит входного числа
inline
uint32_t getMsb( uint32_t u )
{
    u |= u >> 1;
    u |= u >> 2;
    u |= u >> 4;
    u |= u >> 8;
    u |= u >> 16;
    return u - (u >> 1);
}

//-----------------------------------------------------------------------------
//! Возвращает число, в котором установлен только самый старший бит входного числа
inline
uint64_t getMsb( uint64_t u )
{
    u |= u >> 1;
    u |= u >> 2;
    u |= u >> 4;
    u |= u >> 8;
    u |= u >> 16;
    u |= u >> 32;
    return u - (u >> 1);
}

//-----------------------------------------------------------------------------
//! Возвращает номер старшего бита
template<typename IntType>
size_t getMsbNo( IntType i )
{
    size_t bitNo = 0;
    do
    {
        i >>= 1;    
        if (i)
            bitNo++;

    } while(i);

    return bitNo;
}

//-----------------------------------------------------------------------------
//! Возвращает максимальную степень двойки для заданного числа. (1<<getLog2(N)) >= N. Типа логарифм с округлением вверх
template<typename IntType>
size_t getLog2( IntType i )
{
    if (!i)
       return 0; 
    size_t res = umba::bits::getMsbNo( umba::bits::getMsb(i) );
    if ( ( ((IntType)1) << res) != i)
       return res + 1;
    return res;
}


//-----------------------------------------------------------------------------
//! Меняет порядок бит в байте на противоположный. Реализация для 64х-разрядной архитектуры
inline
uint8_t reverseBits4ops64bit(uint8_t v)
{
    return (uint8_t)(((v * 0x80200802ULL) & 0x0884422110ULL) * 0x0101010101ULL >> 32);
}

//-----------------------------------------------------------------------------
//! Меняет порядок бит в байте на противоположный. Реализация для 32х-разрядной архитектуры
inline
uint8_t reverseBits7ops32bit(uint8_t v)
{
    return (uint8_t)(((v * 0x0802UL & 0x22110UL) |
                      (v * 0x8020UL & 0x88440UL)) * 0x10101UL >> 16);
}

//-----------------------------------------------------------------------------
//! Быстрый обмен значений без использования доп памяти.
/*! Подходит для интегральных целых типов и указателей
*/
template<typename IntegralIntType>
inline
void itegralTypeFastSwap( IntegralIntType &t1, IntegralIntType &t2)
{
    t1 = t1 ^ t2;
    t2 = t2 ^ t1;
    t1 = t1 ^ t2;
}

//-----------------------------------------------------------------------------
//! Меняет порядок байт в куске сырой памяти на противоположный
inline
void reverseRawBytes( uint8_t *pBytes, unsigned numBytes )
{
   if (numBytes<2)
       return;

   uint8_t *pBytesLast = pBytes + numBytes - 1;
   for(unsigned i = 0; i!=numBytes/2; ++i, pBytes++, pBytesLast--)
      itegralTypeFastSwap(*pBytes, *pBytesLast);
}

//-----------------------------------------------------------------------------
//! Меняет порядок байт в аргументе, передаваемом по ссылке
template<typename IntType>
inline
void reverseByteOrderInplace( IntType &i )
{
    reverseRawBytes( (uint8_t*)&i, sizeof(i) );
}

//-----------------------------------------------------------------------------
//! Меняет порядок байт в аргументе, возвращает модифицированное значение
template<typename IntType>
inline
IntType reverseByteOrder( IntType i )
{
    reverseByteOrderInplace(i);
    return i;
}

//-----------------------------------------------------------------------------
//! Меняет порядок бит в аргументе, возвращает модифицированное значение
template<typename BitsStorageType>
inline
BitsStorageType reverseBits( BitsStorageType b )
{
/*
    const unsigned bitsPerItem = getTypeBitsCount<BitsStorageType, BitSetSizeType>()
    //sizeof(BitsStorageType) * CHAR_BIT;
    BitsStorageType res = b & 1;

    for (b >>= 1; b; b >>= 1)
    {   
        res <<= 1;
        res |= b & 1;
        bitsPerItem--;
    }

    res <<= bitsPerItem; // shift when v's highest bits are zero
    return res;
*/
    unsigned numBytes = sizeof(BitsStorageType);
    uint8_t *pBytes = (uint8_t *)&b;

    reverseRawBytes( pBytes, numBytes );

    for( unsigned i = 0; i != numBytes; ++i, pBytes++ )
    {
        *pBytes = reverseBits7ops32bit(*pBytes);
    }

    return b;
}

//-----------------------------------------------------------------------------
//! Меняет порядок бит в аргументе, возвращает модифицированное значение. Перегрузка для байта
inline
uint8_t reverseBits( uint8_t b )
{
    return reverseBits7ops32bit(b);
}

//-----------------------------------------------------------------------------
//! Меняет порядок бит в аргументе, возвращает модифицированное значение. Перегрузка для знакового байта
inline
int8_t reverseBits( int8_t b )
{
    return reverseBits7ops32bit((uint8_t)b);
}

//-----------------------------------------------------------------------------
//! Операции над битовым массивом
template< typename RawPtrType, typename BitsStorageType, typename BitSetSizeType>
class BitSetOperations
{


public:

    static const BitSetSizeType npos          = (BitSetSizeType)-1;                 //!< Инвалидная позиция
    static const BitSetSizeType bitsPerItem   = sizeof(BitsStorageType) * CHAR_BIT; //!< Размер типа в битах

    //! Установка бита в заданное значение
    static
    void setBit( RawPtrType pRawBitSet, BitSetSizeType totalBits, BitSetSizeType bitNo, BitsStorageType bitValue )
    {
        if (bitNo>=totalBits)
            return; //UNDONE: need assert

        if (bitValue)
            ((BitsStorageType*)pRawBitSet)[ bitNo / bitsPerItem ] |= makeBit<BitsStorageType, BitSetSizeType>( bitNo % bitsPerItem );
        else
            ((BitsStorageType*)pRawBitSet)[ bitNo / bitsPerItem ] &= makeMask<BitsStorageType, BitSetSizeType>( bitNo % bitsPerItem );
    }

    //! Получение бита
    static
    BitsStorageType getBit( RawPtrType pRawBitSet, BitSetSizeType totalBits, BitSetSizeType bitNo )
    {
        if (bitNo>=totalBits)
            return 0; //UNDONE: need assert

        const BitsStorageType* pBitSet = (BitsStorageType*)pRawBitSet;

        return (pBitSet[ bitNo / bitsPerItem ] & makeBit<BitsStorageType, BitSetSizeType>( bitNo % bitsPerItem )) == 0 ? 0 : 1;
        //const BitsStorageType &v = pBitSet[ bitNo / bitsPerItem ];
        //BitsStorageType  m = makeMask<BitsStorageType, BitSetSizeType>( bitNo % bitsPerItem );
        //return (v & m) == 0 ? 0 : 1;
    }

    //! Получить количество нулевых бит
    static
    BitSetSizeType getClearCount(RawPtrType pRawBitSet, BitSetSizeType totalBits)
    {
        const BitSetSizeType  numItems    = totalBits / bitsPerItem + ((totalBits % bitsPerItem) ? 1 : 0);

        const BitsStorageType* pBitSet = (BitsStorageType*)pRawBitSet;

        BitSetSizeType cnt = 0;
        for(BitSetSizeType i=0; i!=numItems; ++i)
        {
            cnt += getClearCount(pBitSet[i]);
        }

        if (cnt>totalBits)
            return totalBits;

        return cnt;
    }

    //! Поиск первого нулевого бита в элементе хранения, если не найден, возвращает npos
    static
    BitSetSizeType getFirstClear( BitsStorageType bitsStorage )
    {
        for( BitSetSizeType i = 0; i!=bitsPerItem; ++i, bitsStorage>>=1 )
        {
            if ((bitsStorage&1)==0)
                return i;
        }

        return npos;
    }

    //! Поиск первого ненулевого бита в битовом массиве
    static
    BitSetSizeType getFirstClear(RawPtrType pRawBitSet, BitSetSizeType totalBits)
    {
        const BitSetSizeType  numItems    = totalBits / bitsPerItem + ((totalBits % bitsPerItem) ? 1 : 0);

        const BitsStorageType* pBitSet = (BitsStorageType*)pRawBitSet;

        for(BitSetSizeType i=0; i!=numItems; ++i)
        {
            if (pBitSet[i]!=(BitsStorageType)-1)
            {
                BitSetSizeType res = bitsPerItem*i + getFirstEmpty(pBitSet[i]);
                if (res>totalBits)
                    return npos;
                return res;
            }
        }

        return npos;
    }

    //! Конвертация в 16ти-ричную цифру
    static
    char toHexDigit(uint8_t u)
    {
        u &= 0x0F;
        if (u<10)
            return '0' + u;

        return 'A' + u - 10;
    }

    //! Конвертация в двоичную цифру
    static
    char toBinDigit(uint8_t u)
    {
        u &= 0x01;
        return '0' + u;
    }

    //! Генерация 16ти-ричного текстового представления массива бит (запись в писателя символов)
    static
    void writeHexView(RawPtrType pRawBitSet, BitSetSizeType totalBits, ICharWriter *pWriter, BitSetSizeType groupSize = npos)
    {
        //const BitSetSizeType bitsPerItem = sizeof(uint8_t) * CHAR_BIT;
        const BitSetSizeType bitsPerChar = 4;
        const uint8_t *pData = (const uint8_t*)pRawBitSet;
        const bool groupSizeNpos = (groupSize == npos);

        BitSetSizeType symbolCount = 0;
        BitSetSizeType bitCount    = 0;

        while( bitCount<totalBits )
        {
            if (!groupSizeNpos)
            {
                if (symbolCount && (symbolCount % groupSize)==0)
                {
                    pWriter->writeChar(' ');
                }
            }

            if (symbolCount&1) // Выведено нечетное число символов?
                pWriter->writeChar( toHexDigit(*pData) ); // выводим младшую часть
            else
                pWriter->writeChar( toHexDigit(*pData >> bitsPerChar) ); // выводим старшую часть

            bitCount += bitsPerChar;
            symbolCount++;
            if ((symbolCount&1)==0)
                pData++;
        }
    }
    
    //! Генерация двоичного текстового представления массива бит (запись в писателя символов)
    static
    void writeBinView(RawPtrType pRawBitSet, BitSetSizeType totalBits, ICharWriter *pWriter, BitSetSizeType groupSize = npos )
    {
        const BitSetSizeType bitsPerItem = sizeof(uint8_t) * CHAR_BIT;
        //const BitSetSizeType bitsPerChar = 1;
        const uint8_t *pData = (const uint8_t*)pRawBitSet;
        const bool groupSizeNpos = (groupSize == npos);

        //BitSetSizeType symbolCount = 0;
        BitSetSizeType bitCount    = 0;

        uint8_t curByte = reverseBits(*pData);

        while( bitCount<totalBits )
        {
            if (!groupSizeNpos)
            {
                if (bitCount && (bitCount % groupSize)==0)
                {
                    pWriter->writeChar(' ');
                }
            }

            pWriter->writeChar( toBinDigit(curByte) );
            curByte>>=1;
            bitCount++;

            if ((bitCount%bitsPerItem)==0)
            {
                pData++;
                curByte = reverseBits(*pData);
            }
        }
    
    }

}; // class BitSetOperations


template< typename RawPtrType, typename BitsStorageType, typename BitSetSizeType>
class BitSet;



template< typename RawPtrTypeT, typename BitsStorageTypeT, typename BitSetSizeTypeT>
class BitSetHexView;

template< typename RawPtrTypeT, typename BitsStorageTypeT, typename BitSetSizeTypeT>
class BitSetBinView;

//! Оператор вывода в поток для BitSetHexView
template< typename RawPtrTypeT, typename BitsStorageTypeT, typename BitSetSizeTypeT>
umba::SimpleFormatter& operator<<( umba::SimpleFormatter& fmt, const BitSetHexView<RawPtrTypeT, BitsStorageTypeT, BitSetSizeTypeT> &v );

//! Оператор вывода в поток для BitSetBinView
template< typename RawPtrTypeT, typename BitsStorageTypeT, typename BitSetSizeTypeT>
umba::SimpleFormatter& operator<<( umba::SimpleFormatter& fmt, const BitSetBinView<RawPtrTypeT, BitsStorageTypeT, BitSetSizeTypeT> &v );


//-----------------------------------------------------------------------------
//! Типа вьюшки над битами тупо для распечатки в виде хекса
template< typename RawPtrType, typename BitsStorageType, typename BitSetSizeType>
class BitSetHexView
{

public:
    
    template< typename RawPtrTypeT, typename BitsStorageTypeT, typename BitSetSizeTypeT>
    friend
    umba::SimpleFormatter& operator<<( umba::SimpleFormatter& fmt, const BitSetHexView<RawPtrTypeT, BitsStorageTypeT, BitSetSizeTypeT> &v );

    typedef BitSet<RawPtrType, BitsStorageType, BitSetSizeType> BitSetType; //!< Тип битсета

    friend class BitSet<RawPtrType, BitsStorageType, BitSetSizeType>;

protected:

    //! Конструктор вью из битсета
    BitSetHexView( BitSetType *pBitSet, BitSetSizeType groupSize )
    : m_pBitSet(pBitSet)
    , m_groupSize(groupSize)
    {}

    BitSetType      *m_pBitSet;    //!< Указатель на битики
    BitSetSizeType   m_groupSize;  //!< Размер группы при выводе на печать

}; // class BitSetHexView

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
//! Типа вьюшки над битами тупо для распечатки в виде битиков - ноликов и единичек
template< typename RawPtrType, typename BitsStorageType, typename BitSetSizeType>
class BitSetBinView
{

public:
 
    template< typename RawPtrTypeT, typename BitsStorageTypeT, typename BitSetSizeTypeT>
    friend
    umba::SimpleFormatter& operator<<( umba::SimpleFormatter& fmt, const BitSetBinView<RawPtrTypeT, BitsStorageTypeT, BitSetSizeTypeT> &v );

    typedef BitSet<RawPtrType, BitsStorageType, BitSetSizeType> BitSetType; //!< Тип битсета

    friend class BitSet<RawPtrType, BitsStorageType, BitSetSizeType>;

protected:

    //! Конструктор вью из битсета
    BitSetBinView( BitSetType *pBitSet, BitSetSizeType groupSize )
    : m_pBitSet(pBitSet)
    , m_groupSize(groupSize)
    {}

    BitSetType      *m_pBitSet;    //!< Указатель на битики               
    BitSetSizeType   m_groupSize;  //!< Размер группы при выводе на печать

}; // class BitSetBinView

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
//! Операции над битовым массивом, храним указатель на битики
template< typename RawPtrType, typename BitsStorageType, typename BitSetSizeType>
class BitSet : protected BitSetOperations<RawPtrType, BitsStorageType, BitSetSizeType>
{

public:

    typedef BitSetOperations<RawPtrType, BitsStorageType, BitSetSizeType>  BaseType; //!< Базовый тип

    //#ifdef UMBA_SIMPLE_FORMATTER_H
    typedef BitSetHexView<RawPtrType, BitsStorageType, BitSetSizeType>     HexView;  //!< Тип шестнадцатиричной вьюшки
    typedef BitSetBinView<RawPtrType, BitsStorageType, BitSetSizeType>     BinView;  //!< Тип двоичной вьюшки
    //#endif /* UMBA_SIMPLE_FORMATTER_H */

    using   BaseType::npos;
    using   BaseType::bitsPerItem;

    //! Конструктор по умолчанию
    BitSet()
    : m_pBitStorage(0)
    , m_totalBits(0)
    {
    }

    //! Конструктор создания из сырого хранилища бит
    BitSet( RawPtrType pBitStorage, BitSetSizeType totalBits )
    : m_pBitStorage(pBitStorage)
    , m_totalBits(totalBits)
    {
    }

    //! Устанавливает хранилище битикии его размер
    void setStorage( RawPtrType pBitStorage, BitSetSizeType totalBits )
    {
        m_pBitStorage = pBitStorage;
        m_totalBits   = totalBits;
    }


    //! Установка бита в заданное значение
    void setBit( BitSetSizeType bitNo, BitsStorageType bitValue )
    {
        BaseType::setBit( m_pBitStorage, m_totalBits, bitNo, bitValue );
    }

    //! Получение бита
    BitsStorageType getBit( BitSetSizeType bitNo )
    {
        return BaseType::getBit( m_pBitStorage, m_totalBits, bitNo );
    }

    //! Получить количество нулевых бит
    BitSetSizeType getClearCount()
    {
        return BaseType::getClearCount( m_pBitStorage, m_totalBits );
    }

    //! Поиск первого ненулевого бита в битовом массиве
    BitSetSizeType getFirstClear(RawPtrType pRawBitSet, BitSetSizeType totalBits)
    {
        return BaseType::getFirstClear( m_pBitStorage, m_totalBits );
    }

    //! Генерация 16ти-ричного текстового представления массива бит (запись в писателя символов)
    void writeHexView(ICharWriter *pWriter, BitSetSizeType groupSize = npos)
    {
        BaseType::makeHexView( m_pBitStorage, m_totalBits, pWriter, groupSize );
    }

    //! Генерация двоичного текстового представления массива бит (запись в писателя символов)
    void writeBinView(ICharWriter *pWriter, BitSetSizeType groupSize = npos )
    {
        BaseType::makeBinView( m_pBitStorage, m_totalBits, pWriter, groupSize );
    }

    //! Возвращает шестнадцатиричную вьюшку для битсета
    HexView getHexView( BitSetSizeType groupSize = 4 )
    {
        return HexView( this, groupSize );
    }

    //! Возвращает двоичную вьюшку для битсета
    BinView getBinView( BitSetSizeType groupSize = 4 )
    {
        return BinView( this, groupSize );
    }


protected:

    RawPtrType          m_pBitStorage;  //!< Указатель на биты
    BitSetSizeType      m_totalBits  ;  //!< Число бит в наборе

}; // class BitSet







}; // namespace bits



}; // namespace umba


