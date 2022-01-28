#pragma once


#include "i_char_writer.h"

//-----------------------------------------------------------------------------

// umba::term::colors::

namespace umba
{



//-----------------------------------------------------------------------------
//! Реализация интерфейса "писателя символов" ICharWriter в никуда
struct NulCharWriter : UMBA_IMPLEMENTS ICharWriter
{
    UMBA_NON_COPYABLE_STRUCT(NulCharWriter) // Для единообразия

    NulCharWriter() {}

    //! Запись символа
    virtual
    void writeChar( char ch ) override
    {
    }

    //! Сброс всех выходных буферов
    virtual
    void flush() override {}

}; // struct NulCharWriter

typedef NulCharWriter  NullCharWriter; //!< typedef для тех, кто привык к NULL вместо NUL
//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
#if defined(IHC_OCTET_CTREAM_BUF_H)

//! Реализация интерфейса "писателя символов" ICharWriter в поток umba::ihc::IOctetIStream
struct IhcOctetStreamCharWriter : UMBA_IMPLEMENTS ICharWriter
{

    UMBA_NON_COPYABLE_STRUCT(IhcOctetStreamCharWriter)

    //! Конструктор, принимающий указатель на поток umba::ihc::IOctetIStream
    IhcOctetStreamCharWriter(umba::ihc::IOctetIStream *pStream)
    : m_pStream()
    {
        UMBA_ASSERT(m_pStream);
    }

    //! Запись символа
    virtual
    void writeChar( char ch ) override
    {
        m_pStream->write( (const umba::ihc::StreamOctetType*)&ch, (umba::ihc::StreamSize)sizeof(ch));
    }

    //! Сброс всех выходных буферов
    virtual
    void flush() override {}

protected:

    umba::ihc::IOctetIStream *m_pStream; //!< Указатель на поток umba::ihc::IOctetIStream

}; // struct IhcOctetStreamCharWriter

#endif

//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
//! Реализация интерфейса "писателя символов" ICharWriter в буфер памяти
/*!
    \tparam ZeroTerm - указывает, требуется ли поддерживать zero-termination в буфере вывода
 */
template< bool ZeroTerm = true >
struct MemCharWriter : UMBA_IMPLEMENTS ICharWriter
{

    UMBA_NON_COPYABLE_STRUCT(MemCharWriter)

    //! Конструктор, принимающий указатель на буфер и его длину
    MemCharWriter(void* pMemBuf, size_t bufSize) : m_pMemBuf((uint8_t*)pMemBuf), m_pMemBufCur((uint8_t*)pMemBuf), m_pMemEnd( ((uint8_t*)pMemBuf)+bufSize-1 ) {}

    //! Запись символа
    virtual
    void writeChar( char ch ) override
    {
        if (ZeroTerm)
        {
            if (m_pMemBufCur==(m_pMemEnd-1))
                return;
            *m_pMemBufCur++ = ch;
            *m_pMemBufCur = 0; // keep string zero terminated
        }
        else // !ZeroTerm
        {
            if (m_pMemBufCur==m_pMemEnd)
                return;
            *m_pMemBufCur++ = ch;
        }
    }

    //! Сброс всех выходных буферов
    virtual
    void flush() override {}

    //! Возвращает ASCII-Z указатель на строку. Совместимый по интерфейсу с std::string метод
    const char* c_str() const
    {
        UMBA_ASSERT( ZeroTerm == true );
        return (const char*)m_pMemBuf;
    }

    //! Возвращает указатель на данные строки (без гарантии замершающего нуля). Совместимый по интерфейсу с std::string метод
    const uint8_t* data() const
    {
        return (const uint8_t*)m_pMemBuf;
    }

    //! Возвращает размер строки. Совместимый по интерфейсу с std::string метод
    size_t size() const
    {
        return m_pMemBufCur - m_pMemBuf;
    }

    //! Возвращает true для пустой строки. Совместимый по интерфейсу с std::string метод
    bool empty() const
    {
        return m_pMemBufCur == m_pMemBuf;
    }

protected:

    uint8_t *m_pMemBuf;     //!< Указатель на начало буфера
    uint8_t *m_pMemBufCur;  //!< Указатель текущего положения в буфере
    uint8_t *m_pMemEnd;     //!< Указатель на байт за концом буфера

}; // class MemCharWriter

//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
//! Реализация интерфейса "писателя символов" ICharWriter в собственный буфер 
/*!
    \tparam BufSize  задаёт размер буфера
    \tparam ZeroTerm указывает, требуется ли поддерживать zero-termination в буфере вывода
 */
template<size_t BufSize, bool ZeroTerm = true>
struct SelfBufCharWriter : UMBA_INHERITS MemCharWriter< ZeroTerm >
{

    UMBA_NON_COPYABLE_STRUCT(SelfBufCharWriter)

    //! Конструктор
    SelfBufCharWriter() : MemCharWriter<ZeroTerm>( (void*)m_buf, BufSize ) {}

protected:
    
    uint8_t m_buf[BufSize]; //!< Буфер для записи

}; // class SelfBufCharWriter

//-----------------------------------------------------------------------------






//-----------------------------------------------------------------------------
#if !defined(UMBA_MCU_USED)

//-----------------------------------------------------------------------------
//! Реализация CharWriter, производящая вывод в строку std::string по заданной ссылке
struct StringRefCharWriter : UMBA_IMPLEMENTS ICharWriter
{

    UMBA_NON_COPYABLE_STRUCT(StringRefCharWriter)

    //! Конструктор, принимающий ссылку на строку-буфер
    StringRefCharWriter(std::string &buf) : m_buf(buf) {}

    //! Запись одного символа
    virtual
    void writeChar( char ch  //!< Символ для записи
                  ) override
    {
        m_buf.append(1,ch);
    }

    //! Запись заданного буфера
    virtual
    void writeBuf( const uint8_t* pBuf //!< Указатель на записываемый буфер
                 , size_t len          //!< Количество символов для записи
                 ) override
    {
        m_buf.append( (const std::string::value_type*)pBuf, len );
    }

    //! Запись ASCII-Z строки
    virtual
    void writeString( const char* str //!< Указатель на ASCII-Z строку
                    ) override
    {
        m_buf.append( (const std::string::value_type*)str );
    }

    //! Реализация flush - сброс кешированных данных
    virtual
    void flush() override {}

    //-------

    //! Совместимый по интерфейсу с std::stringstream  метод, возвращающий строку
    const std::string& str() const
    {
        return m_buf;
    }

    //! Возвращает ASCII-Z указатель на строку. Совместимый по интерфейсу с std::string метод
    const std::string::value_type* c_str() const
    {
        return m_buf.c_str();
    }

    //! Возвращает указатель на данные строки (без гарантии замершающего нуля). Совместимый по интерфейсу с std::string метод
    const std::string::value_type* data() const
    {
        return m_buf.data();
    }

    //! Возвращает размер строки. Совместимый по интерфейсу с std::string метод
    std::string::size_type size() const
    {
        return m_buf.size();
    }

    //! Возвращает true для пустой строки. Совместимый по интерфейсу с std::string метод
    bool empty() const
    {
        return m_buf.empty();
    }


protected:

    std::string &m_buf;  //!< Ссылка на строку, в которую производится фактическая запись

}; // class StringRefCharWriter

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
//! Реализация CharWriter, производящая вывод в собственный строковый std::string буфер
struct StringCharWriter : UMBA_IMPLEMENTS ICharWriter
{

    UMBA_NON_COPYABLE_STRUCT(StringCharWriter)

    //! Конструктор
    StringCharWriter() : m_buf() {}

    //-------

    //! Запись одного символа
    virtual
    void writeChar( char ch  //!< Символ для записи
                  ) override
    {
        m_buf.append(1,ch);
    }

    //! Запись заданного буфера
    virtual
    void writeBuf( const uint8_t* pBuf  //!< Указатель на записываемый буфер
                 , size_t len           //!< Количество символов для записи 
                 ) override
    {
        m_buf.append( (const std::string::value_type*)pBuf, len );
    }

    //! Запись ASCII-Z строки
    virtual
    void writeString( const char* str ) override
    {
        m_buf.append( (const std::string::value_type*)str );
    }

    //! Реализация flush - сброс кешированных данных
    virtual
    void flush() override {}

    //-------

    //! Совместимый по интерфейсу с std::stringstream метод, возвращающий строку
    const std::string& str() const
    {
        return m_buf;
    }

    //! Возвращает ASCII-Z указатель на строку. Совместимый по интерфейсу с std::string метод
    const std::string::value_type* c_str() const
    {
        return m_buf.c_str();
    }

    //! Возвращает указатель на данные строки (без гарантии замершающего нуля). Совместимый по интерфейсу с std::string метод
    const std::string::value_type* data() const
    {
        return m_buf.data();
    }

    //! Возвращает размер строки. Совместимый по интерфейсу с std::string метод
    std::string::size_type size() const
    {
        return m_buf.size();
    }

    //! Возвращает true для пустой строки. Совместимый по интерфейсу с std::string метод
    bool empty() const
    {
        return m_buf.empty();
    }

protected:

    std::string m_buf; //!< Строка-буфер

}; // class StringCharWriter





//-----------------------------------------------------------------------------
/*
   WORD getConsoleTextAttribute( HANDLE hCon )
   WORD setConsoleTextAttribute( HANDLE hCon, WORD newAttr )
   colors::SgrColor convertColorFromConsole( WORD conColor )
   WORD convertColorToConsole( colors::SgrColor clr )

STD_INPUT_HANDLE (DWORD)-10	
STD_OUTPUT_HANDLE (DWORD)-11	
STD_ERROR_HANDLE (DWORD)-12
HANDLE WINAPI GetStdHandle(
  _In_ DWORD nStdHandle
);
*/

//! Реализация CharWriter, производящая вывод в поток вывода std::ostream&
struct StdStreamCharWriter : UMBA_IMPLEMENTS ICharWriter
{

//public:
/*
    // https://stackoverflow.com/questions/18081392/discrimination-between-file-and-console-streams
    bool isAttachedToConsole() const
    {
    }
*/

    UMBA_NON_COPYABLE_STRUCT(StdStreamCharWriter)


protected:

    //! Определение типа консоли (WIN32 хэндла)
    void detectConsoleTypeHelper()
    {
        #if defined(WIN32) || defined(_WIN32)
        m_hCon = INVALID_HANDLE_VALUE;
        #endif

        switch(m_consoleType)
        {
            case term::UMBA_CONSOLETYPE_FILE:
                 break;

            case term::UMBA_CONSOLETYPE_WINDOWS_CONSOLE:
                 #if defined(WIN32) || defined(_WIN32)
                 if (&m_stream==&std::cout || &m_stream==&std::cerr)
                 {
                     m_hCon = ::GetStdHandle( &m_stream==&std::cout ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE);
                 }
                 #endif
                 break;

            case term::UMBA_CONSOLETYPE_ANSI_TERMINAL:
                 break;
        }
    }



public:

    //! Конструктор, принимающий ссылку на связанный с CharWriter'ом std::ostream
    StdStreamCharWriter(std::ostream &s)
    : m_stream(s)
    , m_consoleType(term::UMBA_CONSOLETYPE_FILE)
    #if defined(WIN32) || defined(_WIN32)
    , m_hCon(INVALID_HANDLE_VALUE)
    , m_defColor(0)
    #endif
    {
        #if defined(WIN32) || defined(_WIN32)

            if (&m_stream==&std::cout || &m_stream==&std::cerr)
            {
                winhelpers::ConsoleType ct = winhelpers::detectConsoleType( &m_stream==&std::cout ? _fileno(stdout) : _fileno(stderr) );
                switch(ct)
                {
                    case winhelpers::UMBA_CONSOLETYPE_FILE           : m_consoleType = term::UMBA_CONSOLETYPE_FILE; break;
                    case winhelpers::UMBA_CONSOLETYPE_WINDOWS_CONSOLE: m_consoleType = term::UMBA_CONSOLETYPE_WINDOWS_CONSOLE;break;
                    case winhelpers::UMBA_CONSOLETYPE_ANSI_TERMINAL  : m_consoleType = term::UMBA_CONSOLETYPE_ANSI_TERMINAL; break;
                }
                detectConsoleTypeHelper();
                if (m_hCon != INVALID_HANDLE_VALUE)
                    m_defColor = term::win32::getConsoleTextAttribute( m_hCon );
            }

        #else

            //return m_stream.tellp() == std::ostream::pos_type(-1);

            // Under Linux we assume that all terminals are ANSI
            m_consoleType = (m_stream.tellp() == std::ostream::pos_type(-1))
                          ? term::ConsoleType::ansi_terminal
                          : term::ConsoleType::file
                          ;

        #endif
    }

    //! Принудительное задание типа консоли
    void forceSetConsoleType( term::ConsoleType consoleType //!< Тип консоли/терминала
                            )
    {
        bool bChanged = m_consoleType != consoleType;
        m_consoleType = consoleType;
        detectConsoleTypeHelper();

        #if defined(WIN32) || defined(_WIN32)
        if (bChanged && m_hCon != INVALID_HANDLE_VALUE)
            m_defColor = term::win32::getConsoleTextAttribute( m_hCon );
        #endif
    }
    
    //! Запись одного символа
    virtual
    void writeChar( char ch  //!< Символ для записи
                  ) override
    {
        //if (ch=='\n' && !isAttachedToConsole())
        //    m_stream.put('\r'); // expand single \n to \r\n sequence when stream is regular file

        beforeWriteChar( ch );

        m_stream.put(ch);
        

        #if defined(WIN32) || defined(_WIN32)

            #ifndef UMBA_CHAR_WRITTERS_DISABLE_OUTPUT_TO_DEBUGGER
                if (&m_stream==&std::cout || &m_stream==&std::cerr)
                {
                    char tmp[] = { ch, 0 };
                    OutputDebugStringA( tmp );
                }
            #endif

        #endif

        if (isTerminal() && ch=='\n')
        {
           #if defined(WIN32) || defined(_WIN32)
               //termIncrementVerticalPos();
           #endif
        }

        afterWriteChar( ch );
    }

/*
    // Используется дефолтная реализация
    void writeBuf( const uint8_t* pBuf, size_t len ) override
    {
        m_stream.write( (const std::ostream::char_type*)pBuf, len );
    }
*/

    //! Реализация flush - сброс кешированных данных
    virtual
    void flush() override
    {
        m_stream.flush();
    }

    //! Возвращает true, если консоль в текстовом режиме (автопреобразование LF -> CR LF)
    virtual
    bool isTextMode() override
    {
        /*
        //m_stream.openmode & 
        //std::ios_base::openmode
        if (isAttachedToConsole())
            return true;
        return false;
        */
        #if defined(WIN32) || defined(_WIN32)
            //if (m_consoleType == term::UMBA_CONSOLETYPE_FILE)
            //    return false;
            //return true;
            return false;
        #else
            return false;
        #endif
    }

    //! Возвращает true, если консоль является терминалом
    virtual
    bool isTerminal() override
    {
        return m_consoleType != term::UMBA_CONSOLETYPE_FILE;

        //if (isAttachedToConsole())
        //    return true;
        return false;
    }

    //! Возвращает true, если консоль является ANSI-терминалом с поддержкой Escape-последовательностей
    virtual
    bool isAnsiTerminal() override
    {
        return m_consoleType != term::UMBA_CONSOLETYPE_FILE;
        //return false; //UNDONE: detect parent process is ansi terminal compatible
    }

    //! Помещает EOL в поток вывода консоли
    virtual
    void putEndl() // end line - LF or CR LF
    {
        /*
        if (isTextMode())
           writeChar( '\n' );
        else
        {
           //putDefEndl();
            #if defined(_WIN32)
            writeString( "\r\n" ); // Под Windows или при использовании MCU манипулятор новой строки использует последовательность "\r\n"
            #endif
        }
        */
        writeChar( '\n' );
    }

    //! Помещает CR (возврат каретки) в поток вывода консоли
    virtual
    void putCR() override // carriage return
    {
        if (isTerminal())
            writeChar( '\r' );
    }

    //! Помещает FF (Form Feed - прокрутка страницы) в поток вывода консоли
    virtual
    void putFF() override // form feed
    {
        if (isTerminal())
            writeChar( '\f' );
    }

    //! Настраивает цвета Ansi-терминала
    virtual void setTermColors(term::colors::SgrColor clr) override
    {
        if (m_consoleType==term::UMBA_CONSOLETYPE_ANSI_TERMINAL)
        {
            setAnsiTermColorsImpl( clr );
        }
        #if defined(WIN32) || defined(_WIN32)
        else if (m_consoleType==term::UMBA_CONSOLETYPE_WINDOWS_CONSOLE)
        {
            if (m_hCon!=INVALID_HANDLE_VALUE)
            {
                term::win32::setConsoleTextAttribute( m_hCon, term::win32::convertColorToConsole(clr, m_defColor) );
            }
        }
        #endif
    }

    //! Настраивает цвета Ansi-терминала по умолчанию
    virtual void setDefaultTermColors() override
    {
        if (m_consoleType==term::UMBA_CONSOLETYPE_ANSI_TERMINAL)
        {
            setAnsiTermDefaultColorsImpl( );
        }
        #if defined(WIN32) || defined(_WIN32)
        else if (m_consoleType==term::UMBA_CONSOLETYPE_WINDOWS_CONSOLE)
        {
            if (m_hCon!=INVALID_HANDLE_VALUE)
            {
                if (!m_defColor)
                    term::win32::setConsoleTextAttribute( m_hCon, FOREGROUND_RED  |FOREGROUND_GREEN|FOREGROUND_BLUE );
                else
                    term::win32::setConsoleTextAttribute( m_hCon, m_defColor );
            }
        }
        #endif
    }



    // https://docs.microsoft.com/en-us/windows/console/setconsolecursorposition
    // https://docs.microsoft.com/en-us/windows/console/getconsolecursorinfo

    // https://docs.microsoft.com/en-us/windows/console/coord-str

    // https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences

protected:


    void termIncrementVerticalPos()
    {
        consoleCoord.X  = 0;
        consoleCoord.Y += 1;
    }

    #if defined(WIN32) || defined(_WIN32)

        COORD getConsoleCursorPosition() const
        {
            CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
       
            if (!GetConsoleScreenBufferInfo(m_hCon, &csbiInfo))
                return COORD{ 0, 0 };
       
            return csbiInfo.dwCursorPosition;
        }

        COORD getConsoleScreenSize() const
        {
            CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
       
            if (!GetConsoleScreenBufferInfo(m_hCon, &csbiInfo))
                return COORD{ 0, 0 };
       
            return csbiInfo.dwSize;
        }

        COORD   consoleCoord = COORD{ 0, 0 };

        int     spinnerLine     = 0;
        int     spinnerEndLine  = 0;

        int     linesScrolled      = 0;
        int     curLineCharsPutted = 0;
        // int     spinnerEndLine  = 0;
    
    #endif

    void beforeWriteChar( char ch )
    {
        #if defined(WIN32) || defined(_WIN32)

            /*
            COORD curPos  = getConsoleScreenSize();
            COORD scrSize = getConsoleCursorPosition();

            bool isLastLine = (curPos.Y==(scrSize.Y-1));

            if (ch=='\r')
            {
                curLineCharsPutted = 0;
            }
            else if (ch=='\r')
            */

            // if (curLineCharsPutted)

        #endif
    
    }

    void afterWriteChar( char ch )
    {
    }

public:

    
    //! Установка каретки (курсора) консоли. 0 - выключена, 1 - полоска внизу, 2 - половина, 3 - во всю высоту
    virtual void terminalSetCaret( int csz ) override
    {
        if (m_consoleType==term::UMBA_CONSOLETYPE_ANSI_TERMINAL)
        {
            //UNDONE: !!!
        }
        #if defined(WIN32) || defined(_WIN32)
        else if (m_consoleType==term::UMBA_CONSOLETYPE_WINDOWS_CONSOLE && m_hCon!=INVALID_HANDLE_VALUE)
        {
            CONSOLE_CURSOR_INFO ci{ 25, TRUE };
            switch(csz)
            {
                case 0: ci.bVisible = FALSE;
                        break;

                case 1: ci.dwSize   = 25;
                        break;

                case 2: ci.dwSize   = 55; // 60; // 50;
                        break;

                default: 
                        ci.dwSize   = 100;
            }

            SetConsoleCursorInfo( m_hCon, &ci );

        }
        #endif
    }

    virtual void terminalSetSpinnerMode( bool m ) override
    {
        #if defined(WIN32) || defined(_WIN32)

        // Проблема в том, что если был длинный текст или переносы строки, то вывод сдвинулся ниже, а старое проскролилось наверх
        // Тут надо считать при выводе каждого символа - если количество выведенных символов больше ширины консоли,
        // или были переводы строки - то были вероятно и скролы (если вывод осуществлялся в последнюю строчку)

        if (m)
        {
            // Become spinner mode
            // auto curCoords = getConsoleCursorPosition();
            // spinnerLine    = curCoords.Y;

            // Нужно очистить текст от старого спиннера
        }
        else
        {
            // Нужно очистить старый текст

            // Нужно перейти на ту строку, на которой спиннер был начат
            // auto curCoords = getConsoleCursorPosition();
            // spinnerEndLine = curCoords.Y;

            //terminalClearLineRemaining()
            // terminalClearRemaining( int maxLines = -1 ) override
        }

        #endif
    }

    //! Перейти в абсолютный 0.
    virtual void terminalMove2Abs0() override
    {
        if (m_consoleType==term::UMBA_CONSOLETYPE_ANSI_TERMINAL)
        {
            //UNDONE: !!!
        }
        #if defined(WIN32) || defined(_WIN32)
        else if (m_consoleType==term::UMBA_CONSOLETYPE_WINDOWS_CONSOLE && m_hCon!=INVALID_HANDLE_VALUE)
        {
            consoleCoord = COORD{ 0, 0 };
            SetConsoleCursorPosition(m_hCon, consoleCoord );
        }
        #endif
    }

    //! Перейти ниже на строку на нулевую позицию.
    virtual void terminalMove2Down() override
    {
        if (m_consoleType==term::UMBA_CONSOLETYPE_ANSI_TERMINAL)
        {
            //UNDONE: !!!
        }
        #if defined(WIN32) || defined(_WIN32)
        else if (m_consoleType==term::UMBA_CONSOLETYPE_WINDOWS_CONSOLE && m_hCon!=INVALID_HANDLE_VALUE)
        {
            consoleCoord.X  = 0;
            consoleCoord.Y += 1;
            SetConsoleCursorPosition(m_hCon, consoleCoord );
        }
        #endif
    }

    //! Перейти в нулевую позицию в текущей строке
    virtual void terminalMove2Line0() override
    {
        if (m_consoleType==term::UMBA_CONSOLETYPE_ANSI_TERMINAL)
        {
            //UNDONE: !!!
        }
        #if defined(WIN32) || defined(_WIN32)
        else if (m_consoleType==term::UMBA_CONSOLETYPE_WINDOWS_CONSOLE && m_hCon!=INVALID_HANDLE_VALUE)
        {
            consoleCoord = getConsoleCursorPosition();
            consoleCoord.X = 0;
            SetConsoleCursorPosition(m_hCon, consoleCoord );
        }
        #endif
    }

    //! Перейти на заданную позицию в текущей строке
    virtual void terminalMove2LinePos( int pos ) override
    {
        if (m_consoleType==term::UMBA_CONSOLETYPE_ANSI_TERMINAL)
        {
            //UNDONE: !!!
        }
        #if defined(WIN32) || defined(_WIN32)
        else if (m_consoleType==term::UMBA_CONSOLETYPE_WINDOWS_CONSOLE && m_hCon!=INVALID_HANDLE_VALUE)
        {
            consoleCoord = getConsoleCursorPosition();
            consoleCoord.X = (SHORT)pos;
            SetConsoleCursorPosition(m_hCon, consoleCoord );
        }
        #endif
    }

    //! Очистить в текущей строке N позиции от текущего положения
    virtual void terminalClearLine( int maxPosToClear=-1 ) override
    {
        if (m_consoleType==term::UMBA_CONSOLETYPE_ANSI_TERMINAL)
        {
            //UNDONE: !!!
        }
        #if defined(WIN32) || defined(_WIN32)
        else if (m_consoleType==term::UMBA_CONSOLETYPE_WINDOWS_CONSOLE && m_hCon!=INVALID_HANDLE_VALUE)
        {
            COORD curCoords = getConsoleCursorPosition();
            COORD conSize   = getConsoleScreenSize();
            if (conSize.X!=0 && conSize.Y!=0)
            {
                int numPositionsToClear = conSize.X - curCoords.X;
                if (numPositionsToClear>0 && maxPosToClear>=0 && numPositionsToClear>maxPosToClear)
                {
                    numPositionsToClear = maxPosToClear;
                }

                if (numPositionsToClear>0)
                {
                    term::win32::setConsoleTextAttribute( m_hCon, FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE );
                    DWORD charsWritten = 0;
                    FillConsoleOutputCharacter( m_hCon, (TCHAR)' ', numPositionsToClear, curCoords, &charsWritten);
                }
            }
        }
        #endif
    
    }

    virtual void terminalClearRemaining( int maxLines = -1 ) override
    {
        if (m_consoleType==term::UMBA_CONSOLETYPE_ANSI_TERMINAL)
        {
            //UNDONE: !!!
        }
        #if defined(WIN32) || defined(_WIN32)
        else if (m_consoleType==term::UMBA_CONSOLETYPE_WINDOWS_CONSOLE && m_hCon!=INVALID_HANDLE_VALUE)
        {
            COORD curCoords = getConsoleCursorPosition();
            COORD conSize   = getConsoleScreenSize();
            if (conSize.X!=0 && conSize.Y!=0)
            {
                DWORD charsWritten = 0;

                /*
                if (curCoords.X<conSize.X)
                {
                    FillConsoleOutputCharacter( m_hCon, (TCHAR)' ', conSize.X - curCoords.X, curCoords, &charsWritten);
                }

                for(++curCoords.Y; curCoords.Y<conSize.Y; ++curCoords.Y)
                {
                    FillConsoleOutputCharacter( m_hCon, (TCHAR)' ', conSize.X, curCoords, &charsWritten);
                }
                */

                int remainingLines = conSize.Y-curCoords.Y-1;
                if (maxLines>=0)
                    remainingLines = maxLines;

                term::win32::setConsoleTextAttribute( m_hCon, FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE );
                FillConsoleOutputCharacter( m_hCon, (TCHAR)' ', (conSize.X - curCoords.X) + (remainingLines)*conSize.X , curCoords, &charsWritten);
            }
        }
        #endif
    }


    //! Переход в абсолютную позицию
    virtual void terminalMove2Pos( int x, int y ) override
    {
        if (m_consoleType==term::UMBA_CONSOLETYPE_ANSI_TERMINAL)
        {
            //UNDONE: !!!
        }
        #if defined(WIN32) || defined(_WIN32)
        else if (m_consoleType==term::UMBA_CONSOLETYPE_WINDOWS_CONSOLE && m_hCon!=INVALID_HANDLE_VALUE)
        {
            consoleCoord.X = (SHORT)x;
            consoleCoord.Y = (SHORT)y;
            SetConsoleCursorPosition(m_hCon, consoleCoord );
        }
        #endif
    }


/*
   WORD getConsoleTextAttribute( HANDLE hCon )
   WORD setConsoleTextAttribute( HANDLE hCon, WORD newAttr )
m_defColor
   colors::SgrColor convertColorFromConsole( WORD conColor )
   WORD convertColorToConsole( colors::SgrColor clr )
*/

protected:

    std::ostream      &m_stream;              //!< Поток вывода для консоли
    term::ConsoleType  m_consoleType;         //!< Тип консоли
    #if defined(WIN32) || defined(_WIN32)
    HANDLE             m_hCon;                //!< Хэндл консоли
    WORD               m_defColor;            //!< Цвет последовательностейумолчанию
    #endif

}; // class StdStreamCharWriter

//-----------------------------------------------------------------------------



#else /* UMBA_MCU_USED defined */



} // namespace umba

#if __CC_ARM || ( (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) )

    #include <stdio.h>
    #include <rt_sys.h>
    #include <rt_misc.h>

#endif

/*

    #if ( (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) )

        asm(".global __use_no_semihosting_swi\n");
        
    #elif __CC_ARM
    
        #pragma import(__use_no_semihosting_swi)
        
    #endif

    extern "C"
    { 
        int fputc(int c, FILE *f)
        {
            return ITM_SendChar(c);
        }

        int fgetc(FILE *f)
        {
            char ch = 0;

            return((int)ch);
        }

        int ferror(FILE *f)
        {
            return EOF;
        }

        void _ttywrch(int ch)
        {
            ITM_SendChar(ch);            
        }
        
        char *_sys_command_string(char *cmd, int len)
        {
            return NULL;
        }
        
        // вызывается после main
        void _sys_exit(int return_code) 
        {
            while(1);
        }        
    }
*/


//-----------------------------------------------------------------------------
namespace umba
{



//-----------------------------------------------------------------------------
// Serial Wire Viewer - SWV
// http://blog.atollic.com/cortex-m-debugging-introduction-to-serial-wire-viewer-swv-event-and-data-tracing
// http://blog.atollic.com/cortex-m-debugging-printf-redirection-to-a-debugger-console-using-swv/itm-part-1
// http://blog.atollic.com/cortex-m-debugging-printf-redirection-to-a-debugger-console-using-swv/itm-part-2
// http://www.keil.com/appnotes/files/apnt_297_v102.pdf

//! Реализация CharWriter, производящая вывод в отладчик - SWV - Serial Wire Viewer
struct SwvCharWritter : UMBA_IMPLEMENTS ICharWriter
{

    UMBA_NON_COPYABLE_STRUCT(SwvCharWritter)

    SwvCharWritter() : ICharWriter() {};

    //! Запись одного символа
    virtual
    void writeChar( char ch  //!< Символ для записи
                  ) override
    {
#if __CC_ARM || ( (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) )
        ITM_SendChar(ch);
#endif
    }

    //! Помещает EOL в поток вывода консоли
    virtual
    void putEndl()
    {
        writeString( "\r\n" );
    }

    //! Реализация flush - сброс кешированных данных
    virtual
    void flush() override
    {}

}; // struct SwdCharWritter

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
#ifdef LUART_UART_HANDLE_H
// Заголовок legacy uart подключен перед использованием райтера, 
// поэтому мы можем сделать реализацию райтера, пихающую всё в uart

} // namespace umba

// На самом деле нам нужен <algorithm>, но он подключается в umba/stl.h
#include "umba/stl.h"

// std::strlen
#include <cstring>

#include "critical_section.h"

// UMBA_IS_IN_ISR()

#define UMBA_LEGACY_UART_CHAR_WRITTER_LOCK()    UMBA_CRITICAL_SECTION(globalCriticalSection)

//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
namespace umba
{



//-----------------------------------------------------------------------------
//! Реализация CharWriter, производящая вывод в классический UART
template< size_t BufSize=512 >
struct LegacyUartCharWriter : UMBA_IMPLEMENTS ICharWriter
{

    UMBA_NON_COPYABLE_STRUCT(LegacyUartCharWriter)

    LegacyUartCharWriter( uart::Handle &uart )
    : m_uart(uart)
    , m_endIdx(0)
    , m_textMode(false)
    , m_term(false)
    , m_ansiTerm(false)
    , m_fastBlink(false)
    {
    }

    //! Запись одного символа
    virtual
    void writeChar( char ch ) override
    {
        /*
        if (m_textMode && ch=='\n')
        {
            writeChar( '\r' );
        }

        blockingWrite((const uint8_t *)(&ch), sizeof(ch));
        */

        writeBuf( (const uint8_t*)&ch, 1 );
    }

    //! Запись ASCII-Z строки
    virtual
    void writeString( const char* str ) override
    {
        writeBuf( (const uint8_t*)str, std::strlen(str) );
    }

    //! Запись буфера/массива символов/байт
    virtual
    void writeBuf( const uint8_t* pBuf, size_t len ) override
    {
        if (!m_uart.isInited())
            return;
        //пытаемся сбросить свой буфер в UART
        trySendOwnBuf();

        // А ничего отсылать и не надо
        if (!pBuf || !len)
            return;

        /*
        // Если в ничего нет в буфере, режим двоичный
        // записываем прямо тот буфер, который нам передан
	    // остаток не ждем и кладем в свой буфер
        if (!m_endIdx && !m_textMode)
        {
            // отсылаем данные из переданного буфера до тех пор, пока остаток не можем положить в свой буфер
            while( len > BufSize);
            {
                size_t bytesWritten = writeToUartNonBlock( pBuf, len );
                len  -= bytesWritten;
                pBuf += bytesWritten;
                // Блокировки объекта не нужно, так как мы не меняем содержимое своего буфера

                if (isIrqProcessed())
                {
                    // Так как мы в обработчике прерывания, то мы можем сделать только одну отсылку
                    // и остаток просто отбросим
                    if (len > BufSize)
                        len = BufSize;
                    break;
                }
            } 
            
            if (len) // У нас осталось только такой кусок, который гарантированно влезает в буфер
            {
                // кладем остаток себе в буфер
                // Защищаем момент изменения буфера и индекса
                UMBA_LEGACY_UART_CHAR_WRITTER_LOCK();
                std::memcpy( m_buf, pBuf, len );
                m_endIdx = len;
            }

            return;
        }
        */

        if (isIrqProcessed())
        {
            size_t bufAvailSize = 0;

            {
                UMBA_LEGACY_UART_CHAR_WRITTER_LOCK();
                bufAvailSize = BufSize - m_endIdx;
                if (len>bufAvailSize)
                    len = bufAvailSize;
            }

            if (m_textMode)
            {
                size_t requiredLen = 0;
                size_t i = 0;
                for( ; i!=len; ++i)
                {
                    size_t nextRequiredLen = requiredLen + getTextModeByteRequiredBufSpace( pBuf[i] );
                    if (nextRequiredLen>bufAvailSize)
                    {
                        break;
                    }

                    requiredLen = nextRequiredLen;
                }

                len = i;
            }
        }

        //---
        //size_t getUartMaxLen() const
        tampBuffer(pBuf, len);
        //--
    }

    //! Выводит перевод строки
    virtual
    void putEndl()
    {
        if (isTerminal()) // isAnsiTerminal()
            writeString( "\r\n" );
        else
            writeString( "\r\n" );
        flush();
    }

    //! Сброс всех выходных буферов
    virtual
    void flush() override
    {
        if (isIrqProcessed())
            return; // single time try from IRQ handlers

        size_t rest = trySendOwnBuf();

        while(rest)
        {
            rest = trySendOwnBuf();
        }
    }

    //! Ожидание сброса буферов
    virtual
    void waitFlushDone()
    {
        while (!m_uart.isTransmitComplete()) {}
    }

    //! Возвращает true, если режим вывода - текстовый - символ '\n' транслируется в последовательность CR LF или LF в зависимости от платформы
    virtual
    bool isTextMode() override
    {
        return m_textMode;
    }

    //! Возвращает true, если данный CharWriter - терминал
    virtual
    bool isTerminal() override
    {
        //if (!isTextMode())
        //    return false;

        return m_term; // 
    }

    //! Возвращает true, если данный CharWriter - Ansi-терминал с поддержкой ESC-последовательностей
    virtual
    bool isAnsiTerminal() override
    {
        if (!isTerminal())
            return false;
        return m_ansiTerm;
    }

    //! Выводит возврат каретки
    virtual
    void putCR() override // carriage return
    {
        if (isTerminal())
            writeChar( '\r' );
        else
            putEndl();

        flush();
    }

    //! Выводит Form Feed
    virtual
    void putFF() override // form feed
    {
        if (isTerminal())
        {
            writeString( "\n----------------\n" ); // Frame separator - для удобства разделения фреймов в логе
            writeChar( '\f' );
        }
        else
            putEndl();

        flush();
    }

    //! Производит установку цвета выводимого текста (текст и фон)
    virtual void setTermColors(term::colors::SgrColor clr) override
    {
        if (isAnsiTerminal())
            setAnsiTermColorsImpl(clr, m_fastBlink);
    }

    //! Устанавливает текстовый режим вывода
    LegacyUartCharWriter& setTextMode( bool ftm = true )
    {
        m_textMode = ftm;
        return *this;
    }

    //! Устанавливает режим работы в терминал
    LegacyUartCharWriter& setTerminalMode( bool fTerm = true )
    {
        m_term = fTerm;
        return *this;
    }

    //! Устанавливает режим работы в Ansi-терминал
    LegacyUartCharWriter& setAnsiTerminalMode( bool fTerm = true )
    {
        if (fTerm)
            m_term = true;
        m_ansiTerm = fTerm;
        return *this;
    }

    //! Устанавливает режим Fast Blink
    LegacyUartCharWriter& setAtFastBlink( bool fFast = true )
    {
        m_fastBlink = fFast;
        return *this;
    }

    //! Возвращает максимальное число байт, которое может быть записано без блокировки
    virtual
    size_t getNonBlockMax()
    {
        return BufSize - m_endIdx;  
    }


protected:

    uart::Handle     &m_uart;           //!< Хэндл классического UART'а
    uint8_t          m_buf[BufSize];    //!< Буфер
    volatile size_t  m_endIdx;          //!< Индекс конца
    bool             m_textMode;        //!< Флаг текстового режима
    bool             m_term;            //!< Флаг режима терминала
    bool             m_ansiTerm;        //!< Флаг режима ANSI-терминала
    bool             m_fastBlink;       //!< Флаг режима Fast Blink???

    //! Возвращает количество реальных байт для хранения символа
    size_t getTextModeByteRequiredBufSpace( uint8_t b )
    {
        #if defined(UMBA_MCU_USED) || defined(_WIN32)
        if (b=='\n')
            return 2;
        #endif
        return 1;
    }

    //! Кладёт перевод строки в текстовом режиме
    bool putLinefeedToBufInTextMode()
    {
        #if defined(UMBA_MCU_USED) || defined(_WIN32)
            if ( m_endIdx>=(BufSize-1) )
                return false;
            m_buf[m_endIdx++] = '\r';
            m_buf[m_endIdx++] = '\n';
        #else
            m_buf[m_endIdx++] = '\n';
        #endif

        return true;
    }

    //! Тампит буфер
    void tampBuffer(const uint8_t* &pBuf, size_t &len)
    {
        while(len)
        {
            // пытаемся отправить кусочек буфера
            trySendOwnBuf();

            // пытаемся засунуть хоть что-нибудь в буфер
            if (!m_textMode)
            {
                // Защищаем момент изменения буфера и индекса
                UMBA_LEGACY_UART_CHAR_WRITTER_LOCK();
                
                // в двоичном режиме тупо пересылаем массивами
                size_t bufAvailSize = BufSize - m_endIdx;
                size_t numBytesToCopy = bufAvailSize > len ? len : bufAvailSize;

                if (numBytesToCopy)
                {
                    std::memcpy( &m_buf[m_endIdx], pBuf, numBytesToCopy );
                    len      -= numBytesToCopy;
                    pBuf     += numBytesToCopy;
                    m_endIdx += numBytesToCopy;
                }
            }
            else
            {
                // Защищаем момент изменения буфера и индекса
                UMBA_LEGACY_UART_CHAR_WRITTER_LOCK();
                while( m_endIdx!=BufSize && len )
                {
                    if (*pBuf=='\n')
                    {
                        if (!putLinefeedToBufInTextMode())
                            break; // нет места для двух символов
                        pBuf++;
                        len--;
                    }
                    else
                    {
                        m_buf[m_endIdx++] = *pBuf;
                        pBuf++;
                        len--;
                    }

                } // while( m_endIdx!=BufSize && len )
            }

            if (isIrqProcessed())
               break;


        } // while(len)
    }

    //! Возвращает количество байт в буфере UART'а
    size_t getUartMaxLen() const
    {
        return UART_UART_TX_BUF_SIZE;
    }

    //! Блокирующая запись в UART
    void blockingWriteToUart( const uint8_t* pBuf, size_t len )
    {
        while(!m_uart.sendLocalArray( (const uint8_t *)pBuf, len ) )
        {}
    }

    //! Возвращает true, если находимся в обработчике прерывания
    bool isIrqProcessed() const
    {
        if (UMBA_IS_IN_ISR()) 
            return true;
        return false;
    }

    //! Пытается отправить свой буфер
    size_t trySendOwnBuf()
    {
        UMBA_LEGACY_UART_CHAR_WRITTER_LOCK();

        if (!m_endIdx)
            return m_endIdx;

        size_t bytesWritten = writeToUartNonBlock( m_buf, m_endIdx );
        if (bytesWritten)
        {
            size_t restSize = m_endIdx - bytesWritten;
            if (restSize)
                std::memmove( m_buf, &m_buf[bytesWritten], restSize );
            m_endIdx = restSize;
        }

        return m_endIdx;
    }


    //! Неблокирующая запись - возвращает сколько байт записано
    size_t writeToUartNonBlock( const uint8_t* pBuf, size_t len )
    {
        if (len > getUartMaxLen())
            len = getUartMaxLen();

        //UMBA_LEGACY_UART_CHAR_WRITTER_LOCK();
        if (!m_uart.isTransmitComplete())
            return 0;

        if (!m_uart.sendLocalArray( pBuf, len ) )
            return 0;

        return len;
    }

}; // class LegacyUartCharWriter

//-----------------------------------------------------------------------------


#endif

#endif /* #if !defined(UMBA_MCU_USED) */

//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
#if !defined(UMBA_MCU_USED)




#else /* no UMBA_MCU_USED defined else branch */

} // namespace umba

#if __CC_ARM || ( (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) )

    #include <stdio.h>
    #include <rt_sys.h>
    #include <rt_misc.h>

#endif



//-----------------------------------------------------------------------------
namespace umba
{



//-----------------------------------------------------------------------------
// Serial Wire Viewer - SWV
// http://blog.atollic.com/cortex-m-debugging-introduction-to-serial-wire-viewer-swv-event-and-data-tracing
// http://blog.atollic.com/cortex-m-debugging-printf-redirection-to-a-debugger-console-using-swv/itm-part-1
// http://blog.atollic.com/cortex-m-debugging-printf-redirection-to-a-debugger-console-using-swv/itm-part-2
// http://www.keil.com/appnotes/files/apnt_297_v102.pdf

//! Реализация CharWriter, производящая вывод в отладчик - SWV - Serial Wire Viewer
struct SwvCharWritter : UMBA_IMPLEMENTS ICharWriter
{

    UMBA_NON_COPYABLE_STRUCT(SwvCharWritter)

    SwvCharWritter() : ICharWriter() {};

    //! Запись одного символа
    virtual
    void writeChar( char ch  //!< Символ для записи
                  ) override
    {
#if __CC_ARM || ( (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) )
        ITM_SendChar(ch);
#endif
    }

    //! Помещает EOL в поток вывода консоли
    virtual
    void putEndl()
    {
        writeString( "\r\n" );
    }

    //! Реализация flush - сброс кешированных данных
    virtual
    void flush() override
    {}

}; // struct SwdCharWritter

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
#ifdef LUART_UART_HANDLE_H
// Заголовок legacy uart подключен перед использованием райтера, 
// поэтому мы можем сделать реализацию райтера, пихающую всё в uart

} // namespace umba

// На самом деле нам нужен <algorithm>, но он подключается в umba/stl.h
#include "umba/stl.h"

// std::strlen
#include <cstring>

#include "critical_section.h"

// UMBA_IS_IN_ISR()

#define UMBA_LEGACY_UART_CHAR_WRITTER_LOCK()    UMBA_CRITICAL_SECTION(globalCriticalSection)

//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
namespace umba
{



//-----------------------------------------------------------------------------
//! Реализация CharWriter, производящая вывод в классический UART
template< size_t BufSize=512 >
struct LegacyUartCharWriter : UMBA_IMPLEMENTS ICharWriter
{

    UMBA_NON_COPYABLE_STRUCT(LegacyUartCharWriter)

    LegacyUartCharWriter( uart::Handle &uart )
    : m_uart(uart)
    , m_endIdx(0)
    , m_textMode(false)
    , m_term(false)
    , m_ansiTerm(false)
    , m_fastBlink(false)
    {
    }

    //! Запись одного символа
    virtual
    void writeChar( char ch ) override
    {
        writeBuf( (const uint8_t*)&ch, 1 );
    }

    //! Запись ASCII-Z строки
    virtual
    void writeString( const char* str ) override
    {
        writeBuf( (const uint8_t*)str, std::strlen(str) );
    }

    //! Запись буфера/массива символов/байт
    virtual
    void writeBuf( const uint8_t* pBuf, size_t len ) override
    {
        if (!m_uart.isInited())
            return;
        //пытаемся сбросить свой буфер в UART
        trySendOwnBuf();

        // А ничего отсылать и не надо
        if (!pBuf || !len)
            return;

        if (isIrqProcessed())
        {
            size_t bufAvailSize = 0;

            {
                UMBA_LEGACY_UART_CHAR_WRITTER_LOCK();
                bufAvailSize = BufSize - m_endIdx;
                if (len>bufAvailSize)
                    len = bufAvailSize;
            }

            if (m_textMode)
            {
                size_t requiredLen = 0;
                size_t i = 0;
                for( ; i!=len; ++i)
                {
                    size_t nextRequiredLen = requiredLen + getTextModeByteRequiredBufSpace( pBuf[i] );
                    if (nextRequiredLen>bufAvailSize)
                    {
                        break;
                    }

                    requiredLen = nextRequiredLen;
                }

                len = i;
            }
        }

        //---
        //size_t getUartMaxLen() const
        tampBuffer(pBuf, len);
        //--
    }

    //! Выводит перевод строки
    virtual
    void putEndl()
    {
        if (isTerminal()) // isAnsiTerminal()
            writeString( "\r\n" );
        else
            writeString( "\r\n" );
        flush();
    }

    //! Сброс всех выходных буферов
    virtual
    void flush() override
    {
        if (isIrqProcessed())
            return; // single time try from IRQ handlers

        size_t rest = trySendOwnBuf();

        while(rest)
        {
            rest = trySendOwnBuf();
        }
    }

    //! Ожидание сброса буферов
    virtual
    void waitFlushDone()
    {
        while (!m_uart.isTransmitComplete()) {}
    }

    //! Возвращает true, если режим вывода - текстовый - символ '\n' транслируется в последовательность CR LF или LF в зависимости от платформы
    virtual
    bool isTextMode() override
    {
        return m_textMode;
    }

    //! Возвращает true, если данный CharWriter - терминал
    virtual
    bool isTerminal() override
    {
        //if (!isTextMode())
        //    return false;

        return m_term; // 
    }

    //! Возвращает true, если данный CharWriter - Ansi-терминал с поддержкой ESC-последовательностей
    virtual
    bool isAnsiTerminal() override
    {
        if (!isTerminal())
            return false;
        return m_ansiTerm;
    }

    //! Выводит возврат каретки
    virtual
    void putCR() override // carriage return
    {
        if (isTerminal())
            writeChar( '\r' );
        else
            putEndl();

        flush();
    }

    //! Выводит Form Feed
    virtual
    void putFF() override // form feed
    {
        if (isTerminal())
        {
            writeString( "\n----------------\n" ); // Frame separator - для удобства разделения фреймов в логе
            writeChar( '\f' );
        }
        else
            putEndl();

        flush();
    }

    //! Производит установку цвета выводимого текста (текст и фон)
    virtual void setTermColors(term::colors::SgrColor clr) override
    {
        if (isAnsiTerminal())
            setAnsiTermColorsImpl(clr, m_fastBlink);
    }

    //! Устанавливает текстовый режим вывода
    LegacyUartCharWriter& setTextMode( bool ftm = true )
    {
        m_textMode = ftm;
        return *this;
    }

    //! Устанавливает режим работы в терминал
    LegacyUartCharWriter& setTerminalMode( bool fTerm = true )
    {
        m_term = fTerm;
        return *this;
    }

    //! Устанавливает режим работы в Ansi-терминал
    LegacyUartCharWriter& setAnsiTerminalMode( bool fTerm = true )
    {
        if (fTerm)
            m_term = true;
        m_ansiTerm = fTerm;
        return *this;
    }

    //! Устанавливает режим Fast Blink
    LegacyUartCharWriter& setAtFastBlink( bool fFast = true )
    {
        m_fastBlink = fFast;
        return *this;
    }

    //! Возвращает максимальное число байт, которое может быть записано без блокировки
    virtual
    size_t getNonBlockMax()
    {
        return BufSize - m_endIdx;  
    }


protected:

    uart::Handle     &m_uart;           //!< Хэндл классического UART'а
    uint8_t          m_buf[BufSize];    //!< Буфер
    volatile size_t  m_endIdx;          //!< Индекс конца
    bool             m_textMode;        //!< Флаг текстового режима
    bool             m_term;            //!< Флаг режима терминала
    bool             m_ansiTerm;        //!< Флаг режима ANSI-терминала
    bool             m_fastBlink;       //!< Флаг режима Fast Blink???

    //! Возвращает количество реальных байт для хранения символа
    size_t getTextModeByteRequiredBufSpace( uint8_t b )
    {
        #if defined(UMBA_MCU_USED) || defined(_WIN32)
        if (b=='\n')
            return 2;
        #endif
        return 1;
    }

    //! Кладёт перевод строки в текстовом режиме
    bool putLinefeedToBufInTextMode()
    {
        #if defined(UMBA_MCU_USED) || defined(_WIN32)
            if ( m_endIdx>=(BufSize-1) )
                return false;
            m_buf[m_endIdx++] = '\r';
            m_buf[m_endIdx++] = '\n';
        #else
            m_buf[m_endIdx++] = '\n';
        #endif

        return true;
    }

    //! Тампит буфер
    void tampBuffer(const uint8_t* &pBuf, size_t &len)
    {
        while(len)
        {
            // пытаемся отправить кусочек буфера
            trySendOwnBuf();

            // пытаемся засунуть хоть что-нибудь в буфер
            if (!m_textMode)
            {
                // Защищаем момент изменения буфера и индекса
                UMBA_LEGACY_UART_CHAR_WRITTER_LOCK();
                
                // в двоичном режиме тупо пересылаем массивами
                size_t bufAvailSize = BufSize - m_endIdx;
                size_t numBytesToCopy = bufAvailSize > len ? len : bufAvailSize;

                if (numBytesToCopy)
                {
                    std::memcpy( &m_buf[m_endIdx], pBuf, numBytesToCopy );
                    len      -= numBytesToCopy;
                    pBuf     += numBytesToCopy;
                    m_endIdx += numBytesToCopy;
                }
            }
            else
            {
                // Защищаем момент изменения буфера и индекса
                UMBA_LEGACY_UART_CHAR_WRITTER_LOCK();
                while( m_endIdx!=BufSize && len )
                {
                    if (*pBuf=='\n')
                    {
                        if (!putLinefeedToBufInTextMode())
                            break; // нет места для двух символов
                        pBuf++;
                        len--;
                    }
                    else
                    {
                        m_buf[m_endIdx++] = *pBuf;
                        pBuf++;
                        len--;
                    }

                } // while( m_endIdx!=BufSize && len )
            }

            if (isIrqProcessed())
               break;


        } // while(len)
    }

    //! Возвращает количество байт в буфере UART'а
    size_t getUartMaxLen() const
    {
        return UART_UART_TX_BUF_SIZE;
    }

    //! Блокирующая запись в UART
    void blockingWriteToUart( const uint8_t* pBuf, size_t len )
    {
        while(!m_uart.sendLocalArray( (const uint8_t *)pBuf, len ) )
        {}
    }

    //! Возвращает true, если находимся в обработчике прерывания
    bool isIrqProcessed() const
    {
        if (UMBA_IS_IN_ISR()) 
            return true;
        return false;
    }

    //! Пытается отправить свой буфер
    size_t trySendOwnBuf()
    {
        UMBA_LEGACY_UART_CHAR_WRITTER_LOCK();

        if (!m_endIdx)
            return m_endIdx;

        size_t bytesWritten = writeToUartNonBlock( m_buf, m_endIdx );
        if (bytesWritten)
        {
            size_t restSize = m_endIdx - bytesWritten;
            if (restSize)
                std::memmove( m_buf, &m_buf[bytesWritten], restSize );
            m_endIdx = restSize;
        }

        return m_endIdx;
    }


    //! Неблокирующая запись - возвращает сколько байт записано
    size_t writeToUartNonBlock( const uint8_t* pBuf, size_t len )
    {
        if (len > getUartMaxLen())
            len = getUartMaxLen();

        //UMBA_LEGACY_UART_CHAR_WRITTER_LOCK();
        if (!m_uart.isTransmitComplete())
            return 0;

        if (!m_uart.sendLocalArray( pBuf, len ) )
            return 0;

        return len;
    }

}; // class LegacyUartCharWriter

//-----------------------------------------------------------------------------


#endif

#endif /* #if !defined(UMBA_MCU_USED) */

//-----------------------------------------------------------------------------



}; // namespace umba


