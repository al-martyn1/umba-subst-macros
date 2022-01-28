#pragma once

//-----------------------------------------------------------------------------

/*! 
    \file
    \brief Umba-исключения
 */

//-----------------------------------------------------------------------------

/*!
    \ingroup UMBA_CORE
    \addtogroup UMBA_INTERNALS
*/

#include <exception>
#include <iostream>

#include "umba/umba.h"
#include "umba/i_char_writer.h"
#include "umba/char_writers.h"
//#include "umba/simple_formatter.h"



// https://habr.com/ru/post/163393/
// https://habr.com/ru/post/163745/

// EXCEPTION_EXECUTE_HANDLER
// EXCEPTION_CONTINUE_SEARCH


#if defined(_MSC_VER)

    #define UMBA_SEH_TRY_BEGIN()                    __try {
    #define UMBA_SEH_TRY_END()                      }

    #define UMBA_SEH_CATCH_BEGIN()                  __except(EXCEPTION_EXECUTE_HANDLER) {
    #define UMBA_SEH_CATCH_END()                    }

    #define UMBA_SEH_CATCH_CONTINUE_SEARCH_BEGIN()  __except(EXCEPTION_CONTINUE_SEARCH) {
    #define UMBA_SEH_CATCH_CONTINUE_SEARCH_END()    }

#else

    #define UMBA_SEH_TRY_BEGIN()                    {
    #define UMBA_SEH_TRY_END()                      }

    #define UMBA_SEH_CATCH_BEGIN()                  {
    #define UMBA_SEH_CATCH_END()                    }

    #define UMBA_SEH_CATCH_CONTINUE_SEARCH_BEGIN()  {
    #define UMBA_SEH_CATCH_CONTINUE_SEARCH_END()    }

#endif

    #define UMBA_SEH_TRY_END_CATCH()                UMBA_SEH_CATCH_BEGIN() \
                                                    UMBA_SEH_CATCH_END()






namespace umba
{


//----------------------------------------------------------------------------
//! Ошибка работы с файлом
class FileException: virtual public std::exception
{

public:

    //! Конструктор из текста сообщения и имени файла
    UMBA_EXPLICIT
    FileException( const std::string& msg, const std::string& fileName )
    : m_message   (msg)
    , m_fileName  (fileName)
    {
        if (!m_message.empty())
        {
            //char lastCh = m_message.back();
            char lastCh = m_message[m_message.size()-1];//.back();
            if (lastCh!='.' && lastCh!='!' && lastCh!='?')
                m_message.append(".");
        }
    }

    //! Деструктор
    virtual ~FileException() throw () {}

    //! Возврат указателя на текст ошибки
    virtual 
    const char* what() const throw ()
    {
       return m_message.c_str();
    }           

    //! Возвращает имя файла
    virtual
    const char* getFileName() const throw()
    {
        return m_fileName.c_str();
    }
    
protected:

    std::string      m_message   ; //!< Текст сообщения
    std::string      m_fileName  ; //!< Имя файла

}; // class FileException

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
//! Ошибка разбора файла
class FileParsingException: virtual public FileException
{

public:

    //! Конструктор из текста сообщения, имени файла, номера строки и позиции в строке. Нумерация с нуля
    UMBA_EXPLICIT
    FileParsingException( const std::string& msg, const std::string& fileName, size_t lineNumber = (size_t)-1, size_t linePos = (size_t)-1 )
    : FileException(msg, fileName)
    , m_lineNumber(lineNumber)
    , m_linePos(linePos)
    {}

    //! Деструктор
    virtual ~FileParsingException() throw () {}

    //! Возвращает номер строки
    virtual
    size_t getLineNumber() const throw()
    {
        return m_lineNumber;
    }

    //! Возвращает позицию в строке
    virtual
    size_t getLinePos() const throw()
    {
        return m_linePos;
    }

protected:

    size_t           m_lineNumber; //!< Номер строки
    size_t           m_linePos;    //!< Позиция в строке

}; // class FileParsingException

//----------------------------------------------------------------------------





//----------------------------------------------------------------------------
//! Ошибка разбора двоичного файла
class FileBinaryParsingException: virtual public FileException
{

public:

    //! Конструктор из текста сообщения, имени файла, смещения в файле и строки с дампом части файла
    UMBA_EXPLICIT
    FileBinaryParsingException( const std::string& msg, const std::string& fileName, size_t offset, const std::string &dump = std::string() )
    : FileException(msg, fileName)
    , m_offset(offset)
    , m_dump(dump)
    {}

    //! Конструктор из текста сообщения, имени файла, смещения в файле и указателя на данные файла (и размер формируемого дампа)
    UMBA_EXPLICIT
    FileBinaryParsingException( const std::string& msg, const std::string& fileName, size_t offset, const uint8_t *pData, size_t numBytes )
    : FileException(msg, fileName)
    , m_offset(offset)
    , m_dump(strGetDumpHelper(pData, numBytes))
    {}

    //! Деструктор
    virtual ~FileBinaryParsingException() throw () {}

    //! Возвращает смещение в файле
    virtual
    size_t getOffset() const throw()
    {
        return m_offset;
    }

    //! Возвращает указатель на строку дампа
    virtual
    const char* getDump() const throw()
    {
        return m_dump.c_str();
    }

    //! Возвращает true, если дамп задан
    virtual
    bool hasDump() const throw()
    {
        return !m_dump.empty();
    }

protected:

    //! Хелпер формирования строки с дампом
    std::string strGetDumpHelper( const uint8_t *pData, size_t numBytes ) const
    {
        #ifdef UMBA_SIMPLE_FORMATTER_H
        ::umba::StringCharWriter charWritter;
        ::umba::SimpleFormatter  ss(&charWritter);
        using namespace ::umba::omanip;

        ss.unsetf( ::umba::SimpleFormatter :: showbase );

        for( size_t i = 0; i!=numBytes; ++i)
        {
            if (i)
                ss<<" ";
            ss<<hex<<pData[i];
        }

        return charWritter.str();
        #else
        return std::string();
        #endif
    }


    size_t           m_offset; //!< Смещение от начала файла
    std::string      m_dump;   //!< Дамп

}; // class FileBinaryParsingException

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
// Note - required to be
// using namespace ::umba::omanip
// or
// using namespace std::iomanip
// declared before using this macro

//----------------------------------------------------------------------------
//! Шаблон настройки потока вывода для вывода информации в HEX виде
template< typename StreamType >
inline
void exceptionHandlingConfigureStreamHex( StreamType & ss, bool setHex )
{
    // Bjarne, 740
    ss.setf( setHex ? std::ios_base::hex : std::ios_base::dec, std::ios_base::basefield );
}

//----------------------------------------------------------------------------
#ifdef UMBA_SIMPLE_FORMATTER_H
//! Специализация шаблона настройки потока вывода для вывода информации в HEX виде для потока umba::SimpleFormatter
template< >
inline
void exceptionHandlingConfigureStreamHex<::umba::SimpleFormatter>( ::umba::SimpleFormatter & ss, bool setHex )
{
    ss.base( setHex ? 16 : 10 );
}
#endif

//----------------------------------------------------------------------------
//! Шаблон настройки потока вывода для вывода информации с расцветкой
template< typename StreamType >
inline
void exceptionHandlingConfigureStreamColoringError( StreamType & ss )
{
}

//----------------------------------------------------------------------------
#ifdef UMBA_SIMPLE_FORMATTER_H
//! Специализация шаблона настройки потока вывода для вывода информации с расцветкой для потока umba::SimpleFormatter
template< >
inline
void exceptionHandlingConfigureStreamColoringError<::umba::SimpleFormatter>( ::umba::SimpleFormatter & ss )
{
    using namespace ::umba::omanip;
    //ss<<error<<"error:";
    //ss<<normal<<" ";

    ss<<error;
}
#endif

//----------------------------------------------------------------------------
//! Шаблон для вывода префикса ошибки
template< typename StreamType >
inline
void exceptionHandlingConfigureStreamColoringErrorMessage( StreamType & ss )
{
}

//----------------------------------------------------------------------------
#ifdef UMBA_SIMPLE_FORMATTER_H
//! Специализация шаблон для вывода префикса ошибки для потока umba::SimpleFormatter
template< >
inline
void exceptionHandlingConfigureStreamColoringErrorMessage<::umba::SimpleFormatter>( ::umba::SimpleFormatter & ss )
{
    using namespace ::umba::omanip;
    ss<<"error:";
    ss<<normal<<" ";
}
#endif

//----------------------------------------------------------------------------






//----------------------------------------------------------------------------
//! Шаблон вывода FileParsingException в поток вывода
template< typename Stream >
inline
void printException(Stream &stream, const ::umba::FileParsingException &e)
{
    //using namespace ::umba::omanip;

    exceptionHandlingConfigureStreamColoringError(stream);

    size_t lineNumber = e.getLineNumber();
    size_t linePos    = e.getLinePos();

    if (lineNumber==(size_t)-1)
    {
        stream<<e.getFileName()<<": ";
        //<<e.what()<<"\n";
    }
    else if (linePos==(size_t)-1)
    {
        stream<<e.getFileName()<<":"<<(lineNumber+1)<<": "; //<<e.what()<<"\n";
    }
    else
    {
        stream<<e.getFileName()<<":"<<(lineNumber+1)<<":"<<(linePos+1)<<": "; //<<e.what();
    }

    exceptionHandlingConfigureStreamColoringErrorMessage(stream);
    stream<<e.what()<<"\n";
}

//----------------------------------------------------------------------------
//! Шаблон вывода FileBinaryParsingException в поток вывода
template< typename Stream >
inline
void printException(Stream &stream, const ::umba::FileBinaryParsingException &e)
{
    //using namespace ::umba::omanip;

    exceptionHandlingConfigureStreamColoringError(stream);
    stream<<e.getFileName()<<": offset "<<e.getOffset()<<" (";
    exceptionHandlingConfigureStreamHex( stream, true );
    stream<<e.getOffset();
    exceptionHandlingConfigureStreamHex( stream, false );
    stream<<")"<<": ";
    
    exceptionHandlingConfigureStreamColoringErrorMessage(stream);
    stream<<e.what();
    if (e.hasDump())
        stream<<". Dump: "<<e.getDump();
    stream<<"\n";
}

//----------------------------------------------------------------------------
//! Шаблон вывода FileException в поток вывода
template< typename Stream >
inline
void printException(Stream &stream, const ::umba::FileException &e)
{
    //using namespace ::umba::omanip;
    exceptionHandlingConfigureStreamColoringError(stream);
    stream<<e.getFileName()<<": ";
    exceptionHandlingConfigureStreamColoringErrorMessage(stream);
    stream<<e.what()<<"\n";
}

//----------------------------------------------------------------------------
//! Шаблон вывода std::exception в поток вывода
template< typename Stream >
inline
void printException(Stream &stream, const std::exception &e)
{
    //using namespace ::umba::omanip;
    exceptionHandlingConfigureStreamColoringError(stream);
    exceptionHandlingConfigureStreamColoringErrorMessage(stream);
    stream<<e.what()<<"\n";
}

//----------------------------------------------------------------------------
//! Шаблон вывода информации "неизвестная ошибка" в поток вывода
template< typename Stream >
inline
void printUnknownException(Stream &stream)
{
    exceptionHandlingConfigureStreamColoringError(stream);
    exceptionHandlingConfigureStreamColoringErrorMessage(stream);
    stream<<"Unknown error\n";
}

//----------------------------------------------------------------------------

















//----------------------------------------------------------------------------
//! Макрос выкидывания исключения FileParsingException \ingroup UMBA_INTERNALS
#define UMBA_EXCEPTION_THROW_FILEPARSINGEXCEPTION( fileInfoGetFrom ,msg ) \
             throw ::umba::FileParsingException( msg, fileInfoGetFrom.getFileName(), fileInfoGetFrom.getLineNumber() )

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
//! Макрос печати исключения в поток \ingroup UMBA_INTERNALS
#define UMBA_EXCEPTION_TO_STREAM_PRINT( stream, e )            ::umba::printException(stream, e)
//! Макрос печати исключения в поток с последующим переводом строки \ingroup UMBA_INTERNALS
#define UMBA_EXCEPTION_TO_STREAM_PRINTLN( stream, e )          do{ ::umba::printException(stream, e); stream<<"\n"; } while(0)

//! Макрос печати "неизвестная ошибка" в поток \ingroup UMBA_INTERNALS
#define UMBA_UNKNOWN_EXCEPTION_TO_STREAM_PRINT( stream )       ::umba::printUnknownException(stream)
//! Макрос печати "неизвестная ошибка" в поток с последующим переводом строки \ingroup UMBA_INTERNALS
#define UMBA_UNKNOWN_EXCEPTION_TO_STREAM_PRINTLN( stream )     do{ ::umba::printUnknownException(stream); stream<<"\n"; } while(0)

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
//! Макрос ловли исключений с выводом в обычный поток
#define UMBA_EXCEPTION_CATCH_LOG_TO_STREAM( errFlag, stream )                          \
                                                                                       \
    catch( const ::umba::FileBinaryParsingException &e )                                 \
    {                                                                                  \
        UMBA_EXCEPTION_TO_STREAM_PRINTLN( stream, e );                                 \
        errFlag = true;                                                                \
    }                                                                                  \
    catch( const ::umba::FileParsingException &e )                                       \
    {                                                                                  \
        UMBA_EXCEPTION_TO_STREAM_PRINTLN( stream, e );                                 \
        errFlag = true;                                                                \
    }                                                                                  \
    catch( const ::umba::FileException &e )                                              \
    {                                                                                  \
        UMBA_EXCEPTION_TO_STREAM_PRINTLN( stream, e );                                 \
        errFlag = true;                                                                \
    }                                                                                  \
    catch( const std::exception &e )                                                   \
    {                                                                                  \
        UMBA_EXCEPTION_TO_STREAM_PRINTLN( stream, e );                                 \
        errFlag = true;                                                                \
    }                                                                                  \
    catch( ... )                                                                       \
    {                                                                                  \
        UMBA_UNKNOWN_EXCEPTION_TO_STREAM_PRINTLN(stream);                              \
        errFlag = true;                                                                \
    }

//----------------------------------------------------------------------------
//! Макрос ловли исключений с выводом в поток umba::SimpleFormatter
#define UMBA_EXCEPTION_CATCH_LOG_TO_UMBA_STREAM( errFlag, stream )                     \
                                                                                       \
    catch( const ::umba::FileBinaryParsingException &e )                                 \
    {                                                                                  \
        UMBA_EXCEPTION_TO_STREAM_PRINTLN( stream, e );                                 \
        errFlag = true;                                                                \
    }                                                                                  \
    catch( const ::umba::FileParsingException &e )                                       \
    {                                                                                  \
        UMBA_EXCEPTION_TO_STREAM_PRINTLN( stream, e );                                 \
        errFlag = true;                                                                \
    }                                                                                  \
    catch( const ::umba::FileException &e )                                              \
    {                                                                                  \
        UMBA_EXCEPTION_TO_STREAM_PRINTLN( stream, e );                                 \
        errFlag = true;                                                                \
    }                                                                                  \
    catch( const std::exception &e )                                                   \
    {                                                                                  \
        UMBA_EXCEPTION_TO_STREAM_PRINTLN( stream, e );                                 \
        errFlag = true;                                                                \
    }                                                                                  \
    catch( ... )                                                                       \
    {                                                                                  \
        UMBA_UNKNOWN_EXCEPTION_TO_STREAM_PRINTLN(stream);                              \
        /*using namespace ::umba::omanip;*/                                              \
        /*stream<<error;*/                                                             \
        /*stream<<"Error: unknown error\n";*/                                          \
        /*stream<<": error:"<<normal<<" "<<"Unknown error\n";*/                        \
        errFlag = true;                                                                \
    }




}; // namespace umba

