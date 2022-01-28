#pragma once

//----------------------------------------------------------------------------

/*! \file
    \brief Хелперы для дампа
*/

//----------------------------------------------------------------------------

#if !defined(UMBA_MCU_USED)
    #include <string>
    #include <cstring>
    #include <vector>
#endif

#include "umba/format_utils.h"



//----------------------------------------------------------------------------
namespace umba
{

//----------------------------------------------------------------------------
namespace utils
{


//------------------------------
//! Возвращает количество памяти, требуемое для шестнадцатиричного дампа
inline
size_t calcDumpSize( size_t dataSize )
{
    return dataSize*3 + 1;
}

//------------------------------
//! Возвращает количество памяти, требуемое для дампа в двоичном виде
inline
size_t calcDumpSizeBin( size_t dataSize )
{
    return dataSize*9 + 1;
}

//------------------------------
//! Формирует шестнадцатиричный дамп одного байта
inline
const char* dumpSingleByte( char *pBuf, uint8_t bt )
{
    *pBuf++ /* [i  ] */  = umba::format_utils::digitToChar( (bt>>4)&0x0F );
    *pBuf++ /* [i+1] */  = umba::format_utils::digitToChar( (bt   )&0x0F );
    *pBuf++ /* [i+2] */  = 0;
    return pBuf;
}

//------------------------------
//! Формирует двоичный дамп одного байта
inline
const char* dumpSingleByteBin( char *pBuf, uint8_t bt )
{
    for(size_t i =0; i!=8u; ++i, bt<<=1)
    {
        *pBuf++ = bt&0x80 ? '1' : '0';
    }
    *pBuf++ = 0;
    return pBuf;
}

//------------------------------
//! Возвращает количество памяти, требуемое для дампа строки (escaped-строки)
inline
size_t calcEscapedStringDumpSize( const char *pData, size_t dataSize )
{
    size_t requiredSize = 0;
    for(size_t i=0; i!=dataSize; ++i)
    {
        uint8_t uch = (uint8_t)pData[i];

        switch(uch)
        {
            case '\'': // \'
            case '\"': // \"
            case '\\': // 
            case '\0': // \0
            case '\a': // \a, bell, 0x07
            case '\b': // 0x08
            case '\f': // Form Feed, 0x0C
            case '\n': // LF, 0x0A
            case '\r': // CR, 0x0D
            case '\t': // tab, 0x09
            case '\v': // vertical tab, 0x0b
                       requiredSize += 2; 
                       break;
        
            default:
        
                       if (uch<(uint8_t)' ' || uch>=0x7F)
                           requiredSize += 4; // \xXX
                       else
                           requiredSize += 1;
        }
    }
    
    return requiredSize+1;
}



} // namespace utils

//----------------------------------------------------------------------------





//----------------------------------------------------------------------------
//! Удаляет пробелы из строки дампа.
inline
const char* dumpStripSpaces( char *pBuf, bool bStrip = true )
{
    if (!bStrip)
        return pBuf;

    size_t len = std::strlen(pBuf);
    if (!len)
        return pBuf;

    size_t chPos = len;
    size_t i     = len;

    for(; i!=0; i--)
    {
        char ch = pBuf[i-1];
        if (ch==' ')
            continue;
        pBuf[--chPos] = ch;
    }
    
    return &pBuf[chPos];
}

//----------------------------------------------------------------------------





//----------------------------------------------------------------------------
//! Формирует HEX-дамп. Места в буфере должно быть достаточно.
inline
const char* dump( char *pBuf, const uint8_t *data, size_t dataSize )
{
    if (!dataSize)
    {
        *pBuf = 0;
        return pBuf;
    }

    // need to dump at least one byte
    utils::dumpSingleByte(pBuf, *data);

    size_t i = 1;
    for(; i != dataSize; ++i)
    {
        pBuf[i*3-1] = ' ';
        utils::dumpSingleByte( &pBuf[i*3], data[i] );
    }

    pBuf[i*3-1] = 0;

    return pBuf;
}

//----------------------------------------------------------------------------
//! Формирует BIN-дамп. Места в буфере должно быть достаточно.
inline
const char* dumpBin( char *pBuf, const uint8_t *data, size_t dataSize )
{
    if (!dataSize)
    {
        *pBuf = 0;
        return pBuf;
    }

    // need to dump at least one byte
    utils::dumpSingleByteBin(pBuf, *data);

    size_t i = 1;
    for(; i != dataSize; ++i)
    {
        pBuf[i*9-1] = ' ';
        utils::dumpSingleByteBin( &pBuf[i*9], data[i] );
    }

    pBuf[i*9-1] = 0;

    return pBuf;
}

//----------------------------------------------------------------------------
//! Формирует HEX-дамп без пробелов. Места в буфере должно быть достаточно.
inline
const char* dumpSolid( char *pBuf, const uint8_t *data, size_t dataSize )
{
    if (!dataSize)
    {
        *pBuf = 0;
        return pBuf;
    }

    size_t i = 0;
    for(; i != dataSize; ++i)
    {
        pBuf[i*2-0] = umba::format_utils::digitToChar( ((data[i])>>4)&0x0F );
        pBuf[i*2+1] = umba::format_utils::digitToChar( ((data[i])   )&0x0F );
    }

    pBuf[i*2] = 0;

    return pBuf;
}

//----------------------------------------------------------------------------
//! Формирует HEX-дамп с ограничением по длине, добавляя элипсисы, если нужно. Места в буфере должно быть достаточно.
template<typename LimitType>
inline
const char* dump( char *pBuf, const uint8_t *data, size_t dataSizeArg, LimitType elipsisLimit )
{
    if (!dataSizeArg)
    {
        *pBuf = 0;
        return pBuf;
    }

    // need to dump at least one byte
    //pBuf[0] = umba::format_utils::digitToChar( ((*data)>>4)&0x0F );
    //pBuf[1] = umba::format_utils::digitToChar( ((*data)   )&0x0F );
    utils::dumpSingleByte(pBuf, *data);

    size_t dataSize = dataSizeArg;
    if (dataSize>(size_t)elipsisLimit)
        dataSize = (size_t)elipsisLimit;

    size_t i = 1;
    for(; i != dataSize; ++i)
    {
        pBuf[i*3-1] = ' ';
        utils::dumpSingleByte( &pBuf[i*3], data[i] );
        //pBuf[i*3-0] = umba::format_utils::digitToChar( ((data[i])>>4)&0x0F );
        //pBuf[i*3+1] = umba::format_utils::digitToChar( ((data[i])   )&0x0F );
    }

    if (dataSize == dataSizeArg)
    {
        pBuf[i*3-1] = 0;
    }
    else
    {
        pBuf[i*3-1] = '.';
        pBuf[i*3-0] = '.';
        pBuf[i*3+1] = '.';
        pBuf[i*3-2] = 0;
    }

    return pBuf;
}

//------------------------------
//! Формирует escaped-строку, где непечатные символы выводятся escape-последовательности. Места в буфере должно быть достаточно
inline
const char* escapedStringDump( char *pBuf, const char *pData, size_t dataSize )
{
    size_t pos = 0;

    for(size_t i=0; i!=dataSize; ++i)
    {
        uint8_t uch = (uint8_t)pData[i];

        switch(uch)
        {
            case '\'': pBuf[pos++] = '\\'; pBuf[pos++] = '\''; break;
            case '\"': pBuf[pos++] = '\\'; pBuf[pos++] = '\"'; break;
            case '\\': pBuf[pos++] = '\\'; pBuf[pos++] = '\\'; break;
            case '\0': pBuf[pos++] = '\\'; pBuf[pos++] = '0' ; break;
            case '\a': pBuf[pos++] = '\\'; pBuf[pos++] = 'a' ; break;
            case '\b': pBuf[pos++] = '\\'; pBuf[pos++] = 'b' ; break;
            case '\f': pBuf[pos++] = '\\'; pBuf[pos++] = 'f' ; break;
            case '\n': pBuf[pos++] = '\\'; pBuf[pos++] = 'n' ; break;
            case '\r': pBuf[pos++] = '\\'; pBuf[pos++] = 'r' ; break;
            case '\t': pBuf[pos++] = '\\'; pBuf[pos++] = 't' ; break;
            case '\v': pBuf[pos++] = '\\'; pBuf[pos++] = 'v' ; break;
            default:
                       if (uch<(uint8_t)' ' || uch>=0x7F)
                       {
                           // \xXX
                           pBuf[pos++] = '\\'; 
                           pBuf[pos++] = 'x' ;
                           utils::dumpSingleByte( &pBuf[pos], uch );
                           pos+=2;
                       }
                       else
                       {
                           pBuf[pos++] = (char)uch;
                       }
        }
    }

    pBuf[pos] = 0;
    
    return pBuf;
}




//----------------------------------------------------------------------------
/*
inline
const char* dump( char *pBuf, const uint8_t *data, uint64_t dataSize )
{
    return dump( pBuf, data, (size_t)dataSize );
}

//----------------------------------------------------------------------------
template<typename LimitType>
inline
const char* dump( char *pBuf, const uint8_t *data, uint64_t dataSizeArg, LimitType elipsisLimit )
{
    return dump( pBuf, data, (size_t)dataSizeArg, elipsisLimit );
}

//----------------------------------------------------------------------------
inline
const char* dump( char *pBuf, const uint8_t *data, int dataSize )
{
    return dump( pBuf, data, (size_t)dataSize );
}

//----------------------------------------------------------------------------
template<typename LimitType>
inline
const char* dump( char *pBuf, const uint8_t *data, int dataSizeArg, LimitType elipsisLimit )
{
    return dump( pBuf, data, (size_t)dataSizeArg, elipsisLimit );
}

//----------------------------------------------------------------------------
*/




//----------------------------------------------------------------------------
#if !defined(UMBA_MCU_USED)

// Плюсовые версии - для больших компов
// Дублировать код не хочется, поэтому это просто обертки над "сишными" микроконтроллерными вариантами

//----------------------------------------------------------------------------
//! Формирует HEX-дамп.
inline
std::string dump( const uint8_t *data, size_t dataSize )
{
    std::vector< char > buf;
    buf.resize( utils::calcDumpSize(dataSize), (char)0 );
    return dump( &buf[0], data, dataSize );
}

//----------------------------------------------------------------------------
//! Формирует HEX-дамп с ограничением по длине, добавляя элипсисы, если нужно.
template<typename LimitType>
inline
std::string dump( const uint8_t *data, size_t dataSize, LimitType elipsisLimit )
{
    std::vector< char > buf;
    buf.resize( utils::calcDumpSize(dataSize)+4, (char)0 );
    return dump( &buf[0], data, dataSize, elipsisLimit );
}

//----------------------------------------------------------------------------
/*
inline
std::string dump( const uint8_t *data, uint64_t dataSize )
{
    return dump( data, (size_t)dataSize );
}

template<typename LimitType>
inline
std::string dump( const uint8_t *data, uint64_t dataSize, LimitType elipsisLimit )
{
    return dump( data, (size_t)dataSizeArg, elipsisLimit );
}

inline
std::string dump( const uint8_t *data, int dataSize )
{
    return dump( data, (size_t)dataSize );
}

template<typename LimitType>
inline
std::string dump( const uint8_t *data, int dataSize, LimitType elipsisLimit )
{
    return dump( data, (size_t)dataSizeArg, elipsisLimit );
}
*/

//----------------------------------------------------------------------------
//! Формирует HEX-дамп без пробелов. Места в буфере должно быть достаточно.
inline
std::string dumpStripSpaces( std::string str, bool bStrip = true )
{
    str.append(1,(char)0); // data() может возвращать не терминированную нулем строку
    // вызываем чисто сишечную версию, которая хочет ASCII-Z
    return std::string( dumpStripSpaces( (char*)str.data(), bStrip) );
}

//----------------------------------------------------------------------------
//! Формирует escaped-строку, где непечатные символы выводятся escape-последовательности.
inline
std::string escapedStringDump( const char *pData, size_t dataSize )
{
    std::vector< char > buf;
    buf.resize( utils::calcEscapedStringDumpSize(pData, dataSize), (char)0 );
    return escapedStringDump( &buf[0], pData, dataSize );
}



#endif
//----------------------------------------------------------------------------






}; // namespace umba

