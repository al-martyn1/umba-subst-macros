#pragma once

//----------------------------------------------------------------------------

/*! 
    \file
    \brief Qt-хелпер для отображения MessageBox
 */

//----------------------------------------------------------------------------

#include <QObject>
#include <QMessageBox>
#include <QString>

#include <string>

#include "umba/umba.h"
#include "umba/i_char_writer.h"
#include "umba/simple_formatter.h"

#include "umba/exception.h"

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
namespace umba {
namespace qtui {



//-----------------------------------------------------------------------------
//! Тип выводимого сообщения
enum class MessageType
{
    defaultValue,  //!< Вывод какой-то по умолчанию
    info,          //!< Уровень info
    warn,          //!< Уровень warn
    err            //!< Уровень err
};

//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
//! Вывод информационного сообщения (при помощи Qt MessageBox) - QString-версия
inline
void showSimpleMessage( MessageType messageType
                      , const QString &msg
                      , const QString &info    = QString()
                      , const QString &details = QString()
                      )
{
    QMessageBox msgBox;
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);

    msgBox.setText(msg);

    if (info.size()!=0)
        msgBox.setInformativeText(info);

    if (details.size()!=0)
        msgBox.setDetailedText(details);

    switch(messageType)
    {
        case MessageType::info:
             msgBox.setIcon(QMessageBox::Information);
             break;

        case MessageType::warn:
             msgBox.setIcon(QMessageBox::Warning);
             break;

        case MessageType::err:
             msgBox.setIcon(QMessageBox::Critical);
             break;
             
    }

    msgBox.exec();

}

//-----------------------------------------------------------------------------
//! Вывод информационного сообщения (при помощи Qt MessageBox) - std::string-версия
inline
void showSimpleMessage( MessageType messageType
                      , const std::string &msg
                      , const std::string &info    = std::string()
                      , const std::string &details = std::string()
                      )
{
    showSimpleMessage( messageType, QString::fromStdString(msg), QString::fromStdString(info), QString::fromStdString(details) );
}

//-----------------------------------------------------------------------------
//! Вывод информационного сообщения (при помощи Qt MessageBox) - std::wstring-версия
inline
void showSimpleMessage( MessageType messageType
                      , const std::wstring &msg
                      , const std::wstring &info    = std::wstring()
                      , const std::wstring &details = std::wstring()
                      )
{
    showSimpleMessage( messageType, QString::fromStdWString(msg), QString::fromStdWString(info), QString::fromStdWString(details) );
}

//-----------------------------------------------------------------------------
//! Вывод информационного сообщения (при помощи Qt MessageBox) - const char*-версия
inline
void showSimpleMessage( MessageType messageType
                      , const char *msg
                      , const char *info    = ""
                      , const char *details = ""
                      )
{
    showSimpleMessage( messageType, QString::fromLocal8Bit(msg), QString::fromLocal8Bit(info), QString::fromLocal8Bit(details) );
}

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
//! Вывод сообщения об ошибке (при помощи Qt MessageBox) - QString-версия
inline
void showSimpleError( const QString &msg
                    , const QString &info    = QString()
                    , const QString &details = QString()
                    )
{
    showSimpleMessage( MessageType::err, msg, info, details );
}

//-----------------------------------------------------------------------------
//! Вывод сообщения об ошибке (при помощи Qt MessageBox) - std::string-версия
inline
void showSimpleError( const std::string &msg
                    , const std::string &info    = std::string()
                    , const std::string &details = std::string()
                    )
{
    showSimpleError( QString::fromStdString(msg), QString::fromStdString(info), QString::fromStdString(details) );
}

//-----------------------------------------------------------------------------
//! Вывод сообщения об ошибке (при помощи Qt MessageBox) - std::wstring-версия
inline
void showSimpleError( const std::wstring &msg
                    , const std::wstring &info    = std::wstring()
                    , const std::wstring &details = std::wstring()
                    )
{
    showSimpleError( QString::fromStdWString(msg), QString::fromStdWString(info), QString::fromStdWString(details) );
}

//-----------------------------------------------------------------------------
//! Вывод сообщения об ошибке (при помощи Qt MessageBox) - const char*-версия
inline
void showSimpleError( const char *msg
                    , const char *info    = ""
                    , const char *details = ""
                    )
{
    showSimpleError( QString::fromLocal8Bit(msg), QString::fromLocal8Bit(info), QString::fromLocal8Bit(details) );
}

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
//! Поддержка перехвата UMBA-Exceptions с выводом сообщения об ошибке в виде Qt-MessageBox'а
#define UMBA_EXCEPTION_CATCH_SHOW_UI_MESSAGE( errFlag )                                              \
                                                                                                     \
    catch( const umba::FileBinaryParsingException &e )                                               \
    {                                                                                                \
        umba::StringCharWriter charWritter;                                                          \
        umba::SimpleFormatter  ss(&charWritter);                                                     \
        using namespace umba::omanip;                                                                \
                                                                                                     \
        ss<<e.getFileName()<<": offset "<<e.getOffset()<<" (";                                       \
        umba::exceptionHandlingConfigureStreamHex( ss, true );                                       \
        ss<<e.getOffset();                                                                           \
        umba::exceptionHandlingConfigureStreamHex( ss, false );                                      \
        ss<<")"<<"\n\nError: "<<e.what();                                                            \
                                                                                                     \
        umba::qtui::showSimpleError( std::string("Failed to read data file!"), charWritter.str(), e.getDump() ); \
                                                                                                     \
        errFlag = true;                                                                              \
    }                                                                                                \
    catch( const umba::FileParsingException &e )                                                     \
    {                                                                                                \
        umba::StringCharWriter charWritter;                                                          \
        umba::SimpleFormatter  ss(&charWritter);                                                     \
                                                                                                     \
        ss<<e.getFileName()<<":"<<e.getLineNumber()<<"\n\nError: "<<e.what()<<"\n";                  \
                                                                                                     \
        umba::qtui::showSimpleError( std::string("Failed to read file!"), charWritter.str() );       \
                                                                                                     \
        errFlag = true;                                                                              \
    }                                                                                                \
    catch( const umba::FileException &e )                                                            \
    {                                                                                                \
        umba::StringCharWriter charWritter;                                                          \
        umba::SimpleFormatter  ss(&charWritter);                                                     \
                                                                                                     \
        ss<<e.getFileName()<<"\n\nError: "<<e.what()<<"\n";                                          \
                                                                                                     \
        umba::qtui::showSimpleError( std::string("Failed to read file!"), charWritter.str() );       \
                                                                                                     \
        errFlag = true;                                                                              \
    }                                                                                                \
    catch( const std::exception &e )                                                                 \
    {                                                                                                \
        umba::StringCharWriter charWritter;                                                          \
        umba::SimpleFormatter  ss(&charWritter);                                                     \
                                                                                                     \
        ss<<"Error: "<<e.what()<<"\n";                                                               \
                                                                                                     \
        umba::qtui::showSimpleError( std::string("Something goes wrong!"), charWritter.str() );      \
                                                                                                     \
        errFlag = true;                                                                              \
    }                                                                                                \
    catch( ... )                                                                                     \
    {                                                                                                \
        umba::qtui::showSimpleError( "Something goes wrong!", "Error: unknown error" );              \
        errFlag = true;                                                                              \
    }

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------

} // namespace qtui
} // namespace umba




