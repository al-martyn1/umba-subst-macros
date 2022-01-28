#pragma once

//----------------------------------------------------------------------------
/*! 
    \file
    \brief Работа с номерами версий
 */

//----------------------------------------------------------------------------
#include <stdint.h>

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
namespace umba
{




//----------------------------------------------------------------------------
//! Версия чего либо вида Major.Minor
struct NumericVersion
{
    uint16_t    majorVersion;  //!< Major часть
    uint16_t    minorVersion;  //!< Minor часть


    //! Упаковка версии в 32х битное целое 
    uint32_t    pack() const
    {
        return ((uint32_t)majorVersion) << 16 |
               ((uint32_t)minorVersion);
    }

    //! Распаковка версии из 32х битного целого
    static
    NumericVersion unpack(uint32_t v)
    {
        return NumericVersion{ (uint16_t)((v>>16)&0xFFFF), (uint16_t)((v)&0xFFFF) };
    }

    //! Создаёт версию "любая версия"
    static 
    NumericVersion any()
    {
        return NumericVersion{ (uint16_t)-1, (uint16_t)-1 };
    }

    //! Проверяет версию - "любая" ли это версия
    bool isAny() const
    {
        return majorVersion == (uint16_t)-1 && minorVersion == (uint16_t)-1;
    }

    //! Сравнение версий
    int compare( const NumericVersion ver //!< Версия для сравнения
               ) const
    {
        if (isAny() || ver.isAny())
            return 0; 

        if (majorVersion < ver.majorVersion)
            return -1;
        if (majorVersion < ver.majorVersion)
            return  1;

        if (minorVersion < ver.minorVersion)
            return -1;
        if (minorVersion < ver.minorVersion)
            return  1;

        return 0;
    }

    //! Сравнение версий (покомпонентное)
    int compare( uint16_t major, uint16_t minor ) const
    {
        return compare( NumericVersion{major, minor} );
    }

};

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
//! Версия чего либо вида Major.Minor.BuildNumber
struct NumericVersionEx
{
    uint16_t    majorVersion; //!< Major часть
    uint16_t    minorVersion; //!< Minor часть
    uint32_t    buildNumber ; //!< Номер сборки

    //! Упаковка версии в 32х битное целое (без BuildNumber части)
    uint32_t    pack() const
    {
        return ((uint32_t)majorVersion) << 16 |
               ((uint32_t)minorVersion);
    }

    //! Распаковка версии из 32х битного целого (без BuildNumber части)
    static
    NumericVersionEx unpack(uint32_t v)
    {
        return NumericVersionEx{ (uint16_t)((v>>16)&0xFFFF), (uint16_t)((v)&0xFFFF), 0 };
    }

    //! Создаёт версию "любая версия"
    static 
    NumericVersionEx any()
    {
        return NumericVersionEx{ (uint16_t)-1, (uint16_t)-1, (uint32_t)-1 };
    }

    //! Проверяет версию - "любая" ли это версия
    bool isAny() const
    {
        return majorVersion == (uint16_t)-1 && minorVersion == (uint16_t)-1;
    }

    //! Сравнение версий
    int compare( const NumericVersionEx ver ) const
    {
        if (isAny() || ver.isAny())
            return 0;

        if (majorVersion < ver.majorVersion)
            return -1;
        if (majorVersion < ver.majorVersion)
            return  1;

        if (minorVersion < ver.minorVersion)
            return -1;
        if (minorVersion < ver.minorVersion)
            return  1;

        if (buildNumber < ver.buildNumber)
            return -1;
        if (buildNumber < ver.buildNumber)
            return  1;

        return 0;
    }

    //! Сравнение версий (покомпонентное)
    int compare( uint16_t major, uint16_t minor, uint32_t buildNumber ) const
    {
        return compare( NumericVersionEx{major, minor, buildNumber} );
    }

};

//----------------------------------------------------------------------------





//----------------------------------------------------------------------------

} // namespace umba



