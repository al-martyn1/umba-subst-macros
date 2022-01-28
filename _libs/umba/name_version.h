#pragma once

//----------------------------------------------------------------------------
/*! 
    \file
    \brief Работа с именоваными номерами версий
 */

//----------------------------------------------------------------------------
#include "numeric_version.h"



//----------------------------------------------------------------------------
namespace umba
{



//----------------------------------------------------------------------------
//! Версия чего либо вида Name/Major.Minor
struct NameVersion
{
    std::string        name;    //!< Имя
    NumericVersion     version; //!< Версия

    //! Создаёт версию "любая версия"
    static 
    NameVersion any()
    {
        return NameVersion{ std::string(), NumericVersion::any() };
    }

    //! Проверяет версию - "любая" ли это версия
    bool isAny() const
    {
        return version.isAny();
    }

    //! Сравнение версий
    int compare( const NameVersion ver ) const
    {
        std::string nameUpper = umba::string_plus::toupper_copy(name);
        std::string verUpper  = umba::string_plus::toupper_copy(ver.name);

        int cmp = nameUpper.compare(verUpper);
        if (cmp)
            return cmp;

        return version.compare( ver.version );
    }

    //! Сравнение версий
    int compare( const std::string &nameOther, const NumericVersion verOther ) const
    {
        std::string nameUpper = umba::string_plus::toupper_copy(name);
        std::string verUpper  = umba::string_plus::toupper_copy(nameOther);

        int cmp = nameUpper.compare(verUpper);
        if (cmp)
            return cmp;

        return version.compare( verOther );
    }

    //! Сравнение версий (покомпонентное)
    int compare( const std::string &nameOther, uint16_t major, uint16_t minor, uint32_t buildNumber ) const
    {
        return compare( NameVersion{ nameOther, NumericVersion{major, minor} } );
    }

}; // struct NameVersion




//! Версия чего либо вида Major.Minor.BuildNumber
struct NameVersionEx
{
    std::string        name;    //!< Имя   
    NumericVersionEx   version; //!< Версия

    //! Создаёт версию "любая версия"
    static 
    NameVersionEx any()
    {
        return NameVersionEx{ std::string(), NumericVersionEx::any() };
    }

    //! Проверяет версию - "любая" ли это версия
    bool isAny() const
    {
        return version.isAny();
    }

    //! Сравнение версий
    int compare( const NameVersionEx ver ) const
    {
        std::string nameUpper = umba::string_plus::toupper_copy(name);
        std::string verUpper  = umba::string_plus::toupper_copy(ver.name);

        int cmp = nameUpper.compare(verUpper);
        if (cmp)
            return cmp;

        return version.compare( ver.version );
    }

    //! Сравнение версий
    int compare( const std::string &nameOther, const NumericVersionEx verOther ) const
    {
        std::string nameUpper = umba::string_plus::toupper_copy(name);
        std::string verUpper  = umba::string_plus::toupper_copy(nameOther);

        int cmp = nameUpper.compare(verUpper);
        if (cmp)
            return cmp;

        return version.compare( verOther );
    }

    //! Сравнение версий (покомпонентное)
    int compare( const std::string &nameOther, uint16_t major, uint16_t minor, uint32_t buildNumber ) const
    {
        return compare( NameVersionEx{ nameOther, NumericVersionEx{major, minor, buildNumber} } );
    }

}; // struct NameVersionEx



} // namespace umba

