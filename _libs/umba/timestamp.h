#pragma once

/*!
    \file
    \brief Представление временного интервала в сутках, часах, минутах, секундах и милисекундах
*/


#include "umba/umba.h"

#if !defined(UMBA_MCU_USED)
    #include <iostream>
    #include <iomanip>
#endif


//----------------------------------------------------------------------------
namespace umba
{



//----------------------------------------------------------------------------
//! Способ форматирования тайм-штампа (разбиения на дни/часы/минуты/секунды)
enum class TimestampFormat
{
    days,
    hours,
    minutes,
    seconds
};

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
//! Штамп времени (в милисекундах). На самом деле - скорее временной интервал в днях и долях дня
struct TimestampMSec
{
    static const uint32_t invalidValue = (uint32_t)-1; //!< Недопустимое значение для поля данной структуры

    uint32_t days         = invalidValue; //!< Целые дни (сутки) интервала
    uint32_t hours        = invalidValue; //!< Целые часы интервала
    uint32_t minutes      = invalidValue; //!< Целые минуты интервала
    uint32_t seconds      = invalidValue; //!< Целые секунды интервала
    uint32_t milliSeconds = invalidValue; //!< Милисекунды интервала

}; // struct TimestampMSec


//! Для совместимости объявляем просто тип Timestamp
typedef TimestampMSec Timestamp;

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
//! Конвертим штамп времени в более удобное для вывода представление
/*! 
    Засада может быть, если логи пишутся больше 130 лет и мы внезапно решили их распарсить.
    Как известно, милисекунд в 32х битах влезает 48+ суток. Секунд, соответственно - 48000 суток, что есть 48000/365 - 131 год
  */
inline
TimestampMSec makeTimestampMSec( uint64_t ts              // Интервал времени в милисекундах
                               , TimestampFormat tsFormat // Способ представления интервала
                               )
{
    Timestamp tsi;

    tsi.milliSeconds = (uint32_t)(ts % 1000u);
    tsi.seconds     = ts / 1000u;
    if (tsFormat==TimestampFormat::seconds)
        return tsi;

    tsi.minutes = tsi.seconds / 60u;
    tsi.seconds = tsi.seconds % 60u;
    if (tsFormat==TimestampFormat::minutes)
        return tsi;

    tsi.hours   = tsi.minutes / 60u;
    tsi.minutes = tsi.minutes % 60u;
    if (tsFormat==TimestampFormat::hours)
        return tsi;

    tsi.days    = tsi.hours / 24u;
    tsi.hours   = tsi.hours % 24u;

    return tsi;
}

//----------------------------------------------------------------------------
//! Для совместимости со старым кодом. \copydetails makeTimestampMSec
inline
TimestampMSec makeTimestamp( uint64_t ts, TimestampFormat tsFormat )
{
    return makeTimestampMSec( ts, tsFormat );
}

//----------------------------------------------------------------------------
//! \copydoc makeTimestampMSec
inline
Timestamp makeTimestampMSec( uint32_t ts, TimestampFormat tsFormat )
{
    return umba::makeTimestampMSec( (uint64_t)ts, tsFormat );
}

//----------------------------------------------------------------------------
//! Для совместимости со старым кодом. \copydetails makeTimestampMSec
inline
TimestampMSec makeTimestamp( uint32_t ts, TimestampFormat tsFormat )
{
    return makeTimestampMSec( ts, tsFormat );
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
//! Для совместимости со старым кодом.
typedef Timestamp TimestampTimeInfo;

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
#ifdef UMBA_SIMPLE_FORMATTER_H
//! Вывод таймштампа в поток
inline
umba::SimpleFormatter& operator<<( umba::SimpleFormatter &s, const Timestamp &tsi )
{
    using namespace umba::omanip;
    if (tsi.days != tsi.invalidValue)
        s<<setw(2)<<fill('0')<<tsi.days<<":";
    if (tsi.hours != tsi.invalidValue)
        s<<setw(2)<<fill('0')<<tsi.hours<<":";
    if (tsi.minutes != tsi.invalidValue)
        s<<setw(2)<<fill('0')<<tsi.minutes<<":";
    //if (tsi.seconds != tsi.invalidValue)
    s<<setw(2)<<fill('0')<<tsi.seconds<<".";

    s<<setw(3)<<fill('0')<<tsi.milliSeconds;

    return s;
}
#endif

//----------------------------------------------------------------------------
#if !defined(UMBA_MCU_USED)
//! Вывод таймштампа в поток
inline
std::ostream& operator<<( std::ostream &s, const Timestamp &tsi )
{
    //using namespace std::iomanip;
    using std::setw;
    using std::setfill;
    if (tsi.days != tsi.invalidValue)
        s<<setw(2)<<setfill('0')<<tsi.days<<":";
    if (tsi.hours != tsi.invalidValue)
        s<<setw(2)<< setfill('0')<<tsi.hours<<":";
    if (tsi.minutes != tsi.invalidValue)
        s<<setw(2)<< setfill('0')<<tsi.minutes<<":";
    //if (tsi.seconds != tsi.invalidValue)
    s<<setw(2)<< setfill('0')<<tsi.seconds<<".";

    s<<setw(3)<< setfill('0')<<tsi.milliSeconds;

    return s;
}
#endif



} // namespace umba


