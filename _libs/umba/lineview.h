#pragma once

/*! 
    \file
    \brief Структура LineView и соответствующие функции
 */


#include "exception.h"

#include <string>
#include <vector>
#include <iterator>
#include <memory>

#include "linefeedtype.h"
#include "lineposinfo.h"


namespace umba
{

/*! \ingroup UMBA_CORE
    \addtogroup UMBA_LINEINFO

    Идентификаторы файлов fileId обеспечивает какое-либо хранилище файлов, которое умеет по идентификатору файла возвращать содержимое.

    @{
 */

//! View для строк на массиве символов.
/*! 
    Оператор [] не может быть реализован для данного класса, так как LineView хранит только 
    индексы в массиве символов, которым не управляет, а для получения ссылки на символ из строки в LineView
    необходимо передать базовый массив символов, на который текущий LineView ссылается.

    По этой причине вместо оператора [] реализованы функции at() и cat(), принимающие одним из аргументов базу отсчёта.
    at() принимает неконстантную базу и возвращает неконстантную ссылку, cat() - константную базу с константным результатом.

    Аналогично - begin()/cbegin(), end()/cend().

    \tparam SizeType Тип индекса строки/столбца.
    Обычно используем std::size_t, но для MK можно исползовать другие типы, например,uint16_t - для экономии памяти.

 */
template < typename SizeType = std::size_t >
struct LineView
{

    SizeType           fileId            ; //!< Идентификатор файла в хранилище (неизменяемом)

    SizeType           lineNumber        ; //!< Номер строки в файле - для человекочитаемого вывода

    SizeType           viewPos           ; //!< Индекс символа в файле (от начала файла), с которого работает данный экземпляр View

    SizeType           viewSize          ; //!< Размер данного View

    SizeType           modifiedMaxSize   ; //!< \brief Признак модифицированности строки.
                                           //!
                                           //!  Если не равно нолю, то: 1) строка хранится в хранилище модифицированных данных;
                                           //!  2) указывает на максимально допустимый размер в текущем месторасположении, 
                                           //!  до которого можно увеличивать модифицированную строку без переаллокаций

    LineFeedType       lineFeedType      ; //!< Тип перевода строки

    std::uint8_t       toBeContinued     ; //!< \brief Признак логического продолжения строки. 
                                           //!<
                                           //!< Если не равно нулю, то текущая строка заканчивается переводом строки, но логически она не заканчивается.
                                           //!< Пример: континуация строк символом '\\'.
                                           //!< Содержит количество символов, которое надо пропустить, чтобы пропустить символы континуации и 
                                           //!< символы перевода строки, чтобы перейти к следующим значащим символам

    //! Возвращает размер View, включая перевод строки
    inline SizeType size() const { return viewSize; }

    //! Возвращает true, если размер View нулевой
    inline bool empty() const { return viewSize==0; }

    //! Возвращает true, если строка заканчивается переводом строки, но имеет символы континуации и имеет неразрывное логическое продолжение
    bool isContinued()  const { return toBeContinued!=0;  }



    #if !defined(UMBA_MCU_USED)

        //! Возвращает ссылку на символ в строке данного View, принимая базу всего текста и индекс в текущем View
        template< typename StringType > typename StringType::value_type&       at( StringType &str, std::size_t idx )         const { return str[viewPos+idx]; }

        //! Возвращает константную ссылку на символ в строке данного View, принимая константную базу всего текста и индекс в текущем View
        template< typename StringType > const typename StringType::value_type& cat( const StringType &str, std::size_t idx )  const { return str[viewPos+idx]; }
       
        //! Возвращает итератор на начало строки
        template< typename StringType > typename StringType::iterator          begin(StringType &str)        const { return str.begin()+viewPos; }

        //! Возвращает константный итератор на начало строки
        template< typename StringType > typename StringType::const_iterator    cbegin(const StringType &str) const { return str.begin()+viewPos; }

        //! Возвращает итератор на конец строки
        template< typename StringType > typename StringType::iterator          end(StringType &str)          const { return str.end()+viewPos+viewSize; }

        //! Возвращает константный итератор на конец строки
        template< typename StringType > typename StringType::const_iterator    cend(const StringType &str)   const { return str.end()+viewPos+viewSize; }

    #endif

    //! Возвращает ссылку на символ в строке данного View, принимая базу всего текста и индекс в текущем View
    template< typename CharType > CharType&         at( CharType *pStr, std::size_t idx )      const { return pStr[viewPos+idx]; }

    //! Возвращает константную ссылку на символ в строке данного View, принимая константную базу всего текста и индекс в текущем View
    template< typename CharType > const CharType&   cat( const CharType *pStr, std::size_t idx ) const { return pStr[viewPos+idx]; }

    //! Возвращает итератор на начало строки
    template< typename CharType > CharType*         begin(CharType *pStr)                      const { return & pStr[viewPos]; }

    //! Возвращает константный итератор на начало строки
    template< typename CharType > const CharType*   cbegin(const CharType *pStr)               const { return & pStr[viewPos]; }

    //! Возвращает итератор на конец строки
    template< typename CharType > CharType*         end(CharType *pStr)                        const { return & pStr[viewPos+viewSize]; }

    //! Возвращает константный итератор на конец строки
    template< typename CharType > const CharType*   cend(const CharType *pStr)                 const { return & pStr[viewPos+viewSize]; }

}; // struct LineView

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
//! Создаёт экземпляр LineView
template < typename SizeType > inline
LineView<SizeType>
makeLineView( SizeType fileId                             //!< Идентификатор файла в хранилище
            , SizeType lineNumber                         //!< Номер строки в файле
            , SizeType viewPos                            //!< Индекс символа в файле (от начала файла), с которого будет работать создаваемый экземпляр View
            , SizeType viewSize                           //!< Размер создаваемого View
            , LineFeedType lineFeedType = lineFeedUnknown //!< Тип перевода строки
            , uint8_t toBeContinued = 0                   //!< Признак логического продолжения строки. 
            , std::size_t modifiedMaxSize = 0             //!< Признак модифицированности строки.
            )
{
    LineView<SizeType> lineView;

    lineView.fileId          = fileId       ;
    lineView.lineNumber      = lineNumber   ;
    lineView.viewPos         = viewPos      ;
    lineView.viewSize        = viewSize     ;
    lineView.lineFeedType    = lineFeedType ;
    lineView.toBeContinued   = toBeContinued;
    lineView.modifiedMaxSize = modifiedMaxSize;

    return lineView;
}

//----------------------------------------------------------------------------
//! Разделяет входной текст на строки.
/*! 
    Производит автоопределение типа перевода строки.

    \tparam OutputIterator Итератор, который помещает значение в результирующий контейнер/поток.

        Подходящие типы:  umba::cpp::array_back_insert_iterator (результат вызова umba::cpp::array_back_inserter),
                          std::std::back_insert_iterator (результат вызова std::back_inserter)

    \tparam CharType Тип символов - char, wchar_t etc.
    \tparam SizeType Тип размера/индексов - обычно std::size_t.
    \tparam FileIdType Тип идентификатора файла - обычно std::size_t.

 */
template<typename OutputIterator, typename CharType, typename SizeType, typename FileIdType > inline
OutputIterator
splitToLineViews( const CharType *pData         //!< Указатель на данные
                , SizeType sz                   //!< Размер данных
                , FileIdType fileId             //!< Идентификатор файла
                , OutputIterator outputIterator //!< Итератор размещения результатов
                )
{
    static const CharType cr = (CharType)'\r';
    static const CharType lf = (CharType)'\n';

    enum State
    {
        wait_CR_or_LF,
        CR_wait_LF,
        LF_wait_CR
    };

    State       state      = wait_CR_or_LF;
    SizeType    pos        = 0;
    SizeType    startPos   = pos;
    SizeType    lineNumber = 0;


    for(; pos!=sz; ++pos)
    {
        CharType ch = pData[pos];

        switch(state)
        {
            case wait_CR_or_LF:
            {
                if (ch==cr)
                    state = CR_wait_LF;
                else if (ch==lf)
                    state = LF_wait_CR;
                break;

            } // case wait_CR_or_LF:

            // Ждём обычный виндовый CR LF
            case CR_wait_LF   :
            {
                if (ch==cr) // упс, поймали второй CR - наверное CR - это символ перевода строки, и мы поймали конец второй строки (пустой)
                {
                    // pos сейчас указывает на второй CR
                    //res.push_back( makeLineView<SizeType>( fileId , lineNumber, startPos, pos-startPos-1 /* size */, lineFeedCR ) ); // пушнули готовую строку CR
                    *outputIterator++ = makeLineView<SizeType>( fileId , lineNumber, startPos, pos-startPos-1 /* size */, lineFeedCR ); // пушнули готовую строку CR
                    startPos = pos;
                    lineNumber++;
                    // состояние автомата не меняем
                }
                else if (ch==lf) // обычный виндовый перевод строки
                {
                    //res.push_back( makeLineView<SizeType>( fileId , lineNumber, startPos, pos-startPos-1 /* size */, lineFeedCRLF ) ); // пушнули готовую строку CRLF
                    *outputIterator++ = makeLineView<SizeType>( fileId , lineNumber, startPos, pos-startPos-1 /* size */, lineFeedCRLF ); // пушнули готовую строку CRLF
                    startPos = pos+1;
                    state = wait_CR_or_LF;
                    lineNumber++;
                }
                else // какой-то другой символ
                {
                    //res.push_back( makeLineView<SizeType>( fileId , lineNumber, startPos, pos-startPos-1 /* size */, lineFeedCR ) ); // пушнули готовую строку CR
                    *outputIterator++ = makeLineView<SizeType>( fileId , lineNumber, startPos, pos-startPos-1 /* size */, lineFeedCR ); // пушнули готовую строку CR
                    startPos = pos;
                    state = wait_CR_or_LF;
                    lineNumber++;
                }
                break;

            } // case CR_wait_LF


            case LF_wait_CR   :
            {
                if (ch==lf) // упс, поймали второй LF - наверное LF - это символ перевода строки, и мы поймали конец второй строки (пустой)
                {
                    // pos сейчас указывает на второй CR
                    //res.push_back( makeLineView<SizeType>( fileId , lineNumber, startPos, pos-startPos-1 /* size */, lineFeedLF ) ); // пушнули готовую строку LF
                    *outputIterator++ = makeLineView<SizeType>( fileId , lineNumber, startPos, pos-startPos-1 /* size */, lineFeedLF ); // пушнули готовую строку LF
                    startPos = pos;
                    lineNumber++;
                    // состояние автомата не меняем
                }
                else if (ch==cr) // обычный маковский перевод строки
                {
                    // res.push_back( makeLineView<SizeType>( fileId , lineNumber, startPos, pos-startPos-1 /* size */, lineFeedLFCR ) ); // пушнули готовую строку LFCR
                    *outputIterator++ = makeLineView<SizeType>( fileId , lineNumber, startPos, pos-startPos-1 /* size */, lineFeedLFCR ); // пушнули готовую строку LFCR
                    startPos = pos+1;
                    state = wait_CR_or_LF;
                    lineNumber++;
                }
                else // какой-то другой символ
                {
                    // res.push_back( makeLineView<SizeType>( fileId , lineNumber, startPos, pos-startPos-1 /* size */, lineFeedLF ) ); // пушнули готовую строку LF
                    *outputIterator++ = makeLineView<SizeType>( fileId , lineNumber, startPos, pos-startPos-1 /* size */, lineFeedLF ); // пушнули готовую строку LF
                    startPos = pos;
                    state = wait_CR_or_LF;
                    lineNumber++;
                }
                break;

            } // case LF_wait_CR

        } // switch(state)

    } // for(; i!=sz; ++i)

    if (startPos>=pos)
        return outputIterator;

    switch(state)
    {
        case wait_CR_or_LF:
            // res.push_back( makeLineView<SizeType>( fileId , lineNumber, startPos, pos-startPos /* size */, lineFeedUnknown ) ); // нет никакого конца строки
            *outputIterator++ = makeLineView<SizeType>( fileId , lineNumber, startPos, pos-startPos /* size */, lineFeedUnknown ); // нет никакого конца строки
            break;

        case CR_wait_LF   :
            // res.push_back( makeLineView<SizeType>( fileId , lineNumber, startPos, pos-startPos-1 /* size */, lineFeedCR ) ); // пушнули готовую строку CR
            *outputIterator++ = makeLineView<SizeType>( fileId , lineNumber, startPos, pos-startPos-1 /* size */, lineFeedCR ); // пушнули готовую строку CR
            break;

        case LF_wait_CR   :
            // res.push_back( makeLineView<SizeType>( fileId , lineNumber, startPos, pos-startPos-1 /* size */, lineFeedLF) ); // пушнули готовую строку LF
            *outputIterator++ = makeLineView<SizeType>( fileId , lineNumber, startPos, pos-startPos-1 /* size */, lineFeedLF ); // пушнули готовую строку LF
            break;

        default:
            break;
    }

    return outputIterator;
}

//----------------------------------------------------------------------------
//! Разделяет входной текст на строки.
/*! 
    Производит автоопределение типа перевода строки.

    \tparam CharType Тип символов - char, wchar_t etc.
    \tparam SizeType Тип размера/индексов - обычно std::size_t.
    \tparam FileIdType Тип идентификатора файла - обычно std::size_t.

    \returns Вектор LineView's, содержащий результат разбора

 */
template<typename CharType, typename SizeType, typename FileIdType  /* = std::size_t */ > inline
std::vector< umba::LineView< SizeType > >
splitToLineViews( const CharType *pData         //!< Указатель на данные
                , SizeType sz                   //!< Размер данных
                , FileIdType fileId             //!< Идентификатор файла
                )
{
    std::vector< umba::LineView< SizeType > > res;
    res.reserve(sz/48);
    splitToLineViews( pData, sz, (std::size_t)fileId, std::back_inserter(res) );
    return res;
}

//----------------------------------------------------------------------------
//! Производит обработку континуаций строк
/*! 
    Континуация (продолжение) - приём, используемый во многих языках программирования,
    в различных форматах конфигурационных файлов, в различных DSL etc.
    Суть в том, что задаётся некоторый маркер, обозначающий, что следующая строка является
    логическим продолжением текущей, не смотря на наличие перевода строки.
    Такой маркер должен непосредственно предшествовать символам перевода строки.

    Обычно используется символ '\\'.

    Данная функция позволяет использовать многосимвольные continuation-последовательности 
    (до 255 символов длиной).

    Данная функция не меняет размеры входного массива, не производит вставки или удаления элементов.
    Данная функция только маркирует подходящие элементы.

    \tparam CharType Тип символов - char, wchar_t etc
    \tparam SizeType Тип размера/индексов - обычно std::size_t.
    \tparam FileIdType Тип идентификатора файла - обычно std::size_t.

 */
template<typename CharType, typename SizeType  /* = std::size_t */ > inline
void processLineContinuations( const CharType *pData, SizeType szData
                             , umba::LineView< SizeType > *pViews, SizeType szViews
                             , const CharType *pContinuationStr, SizeType szContinuationStr
                             )
{
    if (szContinuationStr>255)
        szContinuationStr = 255;

    std::size_t nBytesToCmp = szContinuationStr*sizeof(CharType);

    SizeType i = 0;

    for( ; i!=szViews; ++i)
    {
        if (pViews[i].size() < szContinuationStr)
            continue;

        const CharType *pCmp = pViews[i].cbegin(pData) + pViews[i].size() - szContinuationStr;
        if (0!=std::memcmp( (const void*)pCmp, (const void*)pContinuationStr, nBytesToCmp ))
            continue;

        pViews[i].viewSize      -= szContinuationStr;
        pViews[i].toBeContinued  = (uint8_t)szContinuationStr;

    }
}

//----------------------------------------------------------------------------
//! Производит обработку континуаций строк. Версия для одного символа континуации
template<typename CharType, typename SizeType  /* = std::size_t */ > inline
void processLineContinuations( const CharType *pData, SizeType szData
                             , umba::LineView< SizeType > *pViews, SizeType szViews
                             , CharType continuationChar
                             )
{

    processLineContinuations<CharType, SizeType>( pData, szData, pViews, szViews, &continuationChar, 1 );
}

//----------------------------------------------------------------------------
//! Производит обработку континуаций строк. Версия для вектора
template<typename CharType, typename SizeType  /* = std::size_t */ > inline
void processLineContinuations( const CharType *pData, SizeType szData
                             , std::vector< umba::LineView< SizeType > > &views
                             , const CharType *pContinuationStr, SizeType szContinuationStr
                             )
{
    if (!views.empty())
        processLineContinuations<CharType, SizeType>( pData, szData, views.data(), views.size(), pContinuationStr, szContinuationStr );
}

//----------------------------------------------------------------------------
//! Производит обработку континуаций строк. Версия для вектора и одного символа континуации
template<typename CharType, typename SizeType  /* = std::size_t */ > inline
void processLineContinuations( const CharType *pData, SizeType szData
                             , std::vector< umba::LineView< SizeType > > &views
                             , CharType continuationChar )
{
    if (!views.empty())
        processLineContinuations<CharType, SizeType>( pData, szData, views.data(), views.size(), continuationChar );
}




/* У нас есть два итератора на элементы umba::LineView< SizeType >.
   Требуется проитерироваться по символам.

   Требуется пропускать символы континуации и символы конца строки при этом.
   Символы конца строки требуется опционально заменять на заданный фиксированный символ.
 */


//! helpers for LineView iterators
namespace liv
{

   //! Xz чо
   template< typename CharType, typename SizeType >
   struct LineViewDataInfo
   {
       CharType  *pData;    //!< Xz чо
       SizeType   dataSize; //!< Xz чо
   };

   /*
   template< typename CharType, typename SizeType, typename FileIdType >
   struct SampleDataGetter
   {
       LineViewDataInfo operator()( FileIdType fileId, bool modified) const;
   }
   */

} // namespace liv

//----------------------------------------------------------------------------





//----------------------------------------------------------------------------
//! Базовая реализация итератора по символам входных данных посредством LineView
/*! 

    \tparam CharType         Тип символов - char, wchar_t etc.
    \tparam SizeType         Тип размера/индексов - обычно std::size_t.
    \tparam DataGetter       Тип, возвращающий данные файла по его fileId
    \tparam LineViewIterator Тип итератора по последовательности LineView
 */
template< typename CharType, typename SizeType, typename DataGetter, typename LineViewIterator >
class LineViewSymbolIteratorBaseImpl
{

protected:

    DataGetter                        m_dataGetter         ; //!< Дата геттер
    LineViewIterator                  m_lviIterator        ; //!< Текущий LineView Iterator
    LineViewIterator                  m_lviIteratorEnd     ; //!< LineView Iterator End
    umba::LineView< SizeType >        m_cachedLineView     ; //!< Кешированный LineView
    CharType                         *m_pSymbolsData       ; //!< Указатель на данные текущего файла
    CharType                          m_dummyLinefeed      ; //!< Подстановка для символов перевода строки
    SizeType                          m_linePos            ; //!< Позиция в текущей строке


    //! Возвращает ссылку на предопределённый думми перевод строки
    CharType& getDummyLf() const
    {
        static CharType lf = (CharType)'\n';
        lf = (CharType)'\n'; // reinit
        return lf;
    }

    //! Возвращает указатель на предопределённый думми перевод строки
    CharType* getDummyLfPtr() const
    {
        CharType& lf = getDummyLf();
        return &lf;
    }

public:

    //! Конструктор копирования
    LineViewSymbolIteratorBaseImpl( const LineViewSymbolIteratorBaseImpl &livsi )
    : m_dataGetter         (livsi.m_dataGetter        )
    , m_lviIterator        (livsi.m_lviIterator       )
    , m_lviIteratorEnd     (livsi.m_lviIteratorEnd    )
    , m_cachedLineView     (livsi.m_cachedLineView)
    , m_pSymbolsData       (livsi.m_pSymbolsData      )
    , m_dummyLinefeed      (livsi.m_dummyLinefeed     )
    , m_linePos            (livsi.m_linePos           )
    {
    }

    //! Конструктор из DataGetter и LineViewIterator'а конца
    LineViewSymbolIteratorBaseImpl( const DataGetter &dataGetter, LineViewIterator end, CharType dummyLinefeed = (CharType)0 )
    : m_dataGetter         (dataGetter)
    , m_lviIterator        ()
    , m_lviIteratorEnd     (end)
    , m_cachedLineView     ()
    , m_pSymbolsData       ()
    , m_dummyLinefeed      (dummyLinefeed)
    , m_linePos            ()
    {
    }

    //! Хелпер (пере)инициализации LineViewIterator итератором
    void initWithLviIterator( const LineViewIterator &lviIterator )
    {
        m_lviIterator = lviIterator;

        if (lviIterator==m_lviIteratorEnd)
        {
            m_linePos = 0;
            m_pSymbolsData = 0;
            return;
        }

        m_cachedLineView = *lviIterator;
        liv::LineViewDataInfo<CharType, SizeType> dataInfo = m_dataGetter( m_cachedLineView.fileId, m_cachedLineView.modifiedMaxSize>0 );

        m_linePos = 0;
        m_pSymbolsData = & m_cachedLineView.at(dataInfo.pData,0);
    }

protected:


    //! Возвращает LinePosInfo. CharType ptr - для унификации сигнатуры, не используется
    umba::LinePosInfo<SizeType> getLinePosInfo( CharType *ptr = 0) const
    {
        if (m_lviIterator==m_lviIteratorEnd)
        {
            UMBA_ASSERT_FAIL();
        }

        umba::LinePosInfoEx<SizeType, CharType> res;
        res.fileId     = m_cachedLineView.fileId    ;
        res.lineNumber = m_cachedLineView.lineNumber;
        res.linePos    = m_linePos     ;
        //res.pData      = m_pSymbolsData;
        return res;

        //return umba::LinePosInfo<SizeType> { m_cachedLineView.fileId, m_cachedLineView.lineNumber, m_linePos };
    }

    //! Возвращает LinePosInfoEx, инициализированный переданным указателем
    umba::LinePosInfoEx<SizeType, CharType> getLinePosInfoEx( CharType *ptr ) const
    {
        if (m_lviIterator==m_lviIteratorEnd)
        {
            UMBA_ASSERT_FAIL();
        }

        umba::LinePosInfoEx<SizeType, CharType> res;
        res.fileId     = m_cachedLineView.fileId    ;
        res.lineNumber = m_cachedLineView.lineNumber;
        res.linePos    = m_linePos     ;
        //res.pData      = m_pSymbolsData;
        res.pData      = ptr;
        return res;

        //return umba::LinePosInfoEx<SizeType, CharType> { m_cachedLineView.fileId, m_cachedLineView.lineNumber, m_linePos, m_pSymbolsData };
    }

    //! Возвращает LinePosInfoEx, инициализированный собственным указателем
    umba::LinePosInfoEx<SizeType, CharType> getLinePosInfoEx( ) const
    {
        return getLinePosInfoEx( m_pSymbolsData );
    }


    //! Базовая реализация операции разыменования
    umba::LinePosInfoEx<SizeType,CharType> dereferenceOperatorImpl() const
    {
         //const umba::LineView< SizeType > > &lvi = *m_lviIterator;
         const umba::LineView< SizeType > &lvi = m_cachedLineView;

         if (m_linePos < lvi.viewSize) // simple in current line
             //return *m_pSymbolsData;
             return getLinePosInfoEx(m_pSymbolsData);

         if (!lvi.toBeContinued)
         {
             if (m_dummyLinefeed!=(CharType)0)
                 //return m_dummyLinefeed;
                 return getLinePosInfoEx((char*)&m_dummyLinefeed);

             SizeType lfLen = getLineFeedTypeLength(lvi.lineFeedType);
             if (!lfLen)
                 //return getDummyLf(); // return reserved \n for unknown linefeed type
                 return getLinePosInfoEx(getDummyLfPtr());

             SizeType diff = m_linePos - lvi.viewSize;
             if (diff>=lfLen)
             {
                 UMBA_ASSERT_FAIL();
             }

             //return *m_pSymbolsData;
             return getLinePosInfoEx(m_pSymbolsData);
         }

         UMBA_ASSERT_FAIL();
         //return *m_pSymbolsData;
         return getLinePosInfoEx(m_pSymbolsData);
    }


public:

    //! Префиксный инкремент
    LineViewSymbolIteratorBaseImpl& operator++()
    {
        const umba::LineView< SizeType > &lvi = *m_lviIterator;

        SizeType nextLinePos = m_linePos+1;

        if (nextLinePos < lvi.viewSize) // nextLinePos всё равно внутри строки
        {
            ++m_linePos;
            ++m_pSymbolsData;
            return *this;
        }

        if (!lvi.toBeContinued) // строка без продолжения, нужно показать linefeed
        {
            SizeType diff = nextLinePos - lvi.viewSize; // насколько мы выйдем за строку?
            SizeType lineFeedLen = getLineFeedTypeLength(lvi.lineFeedType);

            if (m_dummyLinefeed!=(CharType)0) // у нас есть переопределённый перевод строки?
                lineFeedLen = 1;

            if (!lineFeedLen)    // у строки есть конец строки, но он не определён, тогда используется m_lf с длиной в 1 символ
                lineFeedLen = 1;

            if (diff<lineFeedLen) // мы уже не в тексте строки, но за конец строки (вместе с переводом строки) ещё не выйдем
            {
                ++m_linePos;
                ++m_pSymbolsData;
                return *this;
            }

        }

        // Нужно менять строку

        if (m_lviIterator==m_lviIteratorEnd)
        {
            UMBA_ASSERT_FAIL();
        }

        ++m_lviIterator;

        initWithLviIterator(m_lviIterator);

        return *this;

    }

    //! Постфиксный инкремент
    UMBA_DECLARE_DEPRECATED_FN_MSG( "Postfix version too slow, don't use it", LineViewSymbolIteratorBaseImpl& operator++(int) )
    {
        //! \cond DOXYGEN_SUNDAY_NEVER_COMES
        LineViewSymbolIteratorBaseImpl thisCopy = *this; //!< warning: Member thisCopy (variable) of class umba::LineViewLinePosInfoExIterator is not documented.
        this->operator++();
        return thisCopy;
        //! \endcond
    }

    //! Сравнение на равенство
    bool operator==( const LineViewSymbolIteratorBaseImpl &it )
    {
        if (m_lviIterator!=it.m_lviIterator)
            return false;

        return m_pSymbolsData==it.m_pSymbolsData;
    }

    //! Сравнение на неравенство
    bool operator!=( const LineViewSymbolIteratorBaseImpl &it )
    {
        return !operator==(it);
    }

    //! Переход к следующей строке
    LineViewSymbolIteratorBaseImpl getNextLineBegin() const
    {
        LineViewIterator it = m_lviIterator;

        if (it==m_lviIteratorEnd)
        {
            //UMBA_ASSERT_FAIL(); // и так уже в конце концов
        }
        else
        {
            if (!it->toBeContinued)
            {
                //++next; // достаточно одной таблэтки
                ++it;
            }
            else
            {
                //while(next!=m_lviIteratorEnd && next->toBeContinued)
                //    ++next;
                while(it!=m_lviIteratorEnd && it->toBeContinued)
                    ++it;
            }
        }
        
        LineViewSymbolIteratorBaseImpl res = *this;
        //res.initWithLviIterator(m_lviIterator);
        res.initWithLviIterator(it);

        return res;
    }

    //! Возвращает итератор конца
    LineViewSymbolIteratorBaseImpl getEndIterator() const
    {
        LineViewSymbolIteratorBaseImpl res = *this;
        res.initWithLviIterator(m_lviIteratorEnd);
        return res;
    }


}; // class LineViewSymbolIteratorBaseImpl


template< typename CharType, typename SizeType, typename DataGetter, typename LineViewIterator >
class LineViewSymbolIterator;


//----------------------------------------------------------------------------
//! Итератор по символам входных данных посредством LineView. Указатель нассимвол возвращается в структуре LinePosInfoEx
/*! 

    \tparam CharType         Тип символов - char, wchar_t etc.
    \tparam SizeType         Тип размера/индексов - обычно std::size_t.
    \tparam DataGetter       Тип, возвращающий данные файла по его fileId
    \tparam LineViewIterator Тип итератора по последовательности LineView
 */
template< typename CharType, typename SizeType, typename DataGetter, typename LineViewIterator >
class LineViewLinePosInfoExIterator : public LineViewSymbolIteratorBaseImpl<CharType, SizeType, DataGetter, LineViewIterator>
{

public:

    typedef LineViewSymbolIteratorBaseImpl<CharType, SizeType, DataGetter, LineViewIterator>  BaseImpl; //!< typedef для родительского класса

    //! Конструктор из LineViewSymbolIteratorBaseImpl
    LineViewLinePosInfoExIterator( const BaseImpl &livsi ) : BaseImpl(livsi) {}

    //! Конструктор копирования
    LineViewLinePosInfoExIterator( const LineViewLinePosInfoExIterator &livsi ) : BaseImpl(livsi) {}

    //! Конструктор из DataGetter и LineViewIterator'а конца
    LineViewLinePosInfoExIterator( const DataGetter &dataGetter, LineViewIterator end, CharType dummyLinefeed = (CharType)0 )
    : BaseImpl(dataGetter, end, dummyLinefeed)
    {
    }

    //! Конструктор из LineViewSymbolIterator
    LineViewLinePosInfoExIterator( const LineViewSymbolIterator<CharType, SizeType, DataGetter, LineViewIterator> &livsi );
    //LineViewLinePosInfoExIterator( const LineViewSymbolIterator<> &livsi ) : LineViewSymbolIteratorBaseImpl(livsi) {}


public:

    
    //! Оператор разыменования
    umba::LinePosInfoEx<SizeType,CharType> operator*() const
    {
         return dereferenceOperatorImpl();
    }

    //! Префиксный инкремент
    LineViewLinePosInfoExIterator& operator++()
    {
        LineViewSymbolIteratorBaseImpl::operator++();
        return *this;

    }

    //! Постфиксный инкремент
    UMBA_DECLARE_DEPRECATED_FN_MSG( "Postfix version too slow, don't use it", LineViewLinePosInfoExIterator& operator++(int) )
    {
        //! \cond DOXYGEN_SUNDAY_NEVER_COMES
        LineViewLinePosInfoExIterator theThisCopy = *this; //!< warning: Member theThisCopy (variable) of class umba::LineViewLinePosInfoExIterator is not documented.
        this->operator++();
        return theThisCopy;
        //! \endcond
    }

    //! Сравнение на равенство
    bool operator==( const LineViewLinePosInfoExIterator &it )
    {
        return BaseImpl::operator==(it);
    }

    //! Сравнение на неравенство
    bool operator!=( const BaseImpl &it )
    {
        return BaseImpl::operator!=(it);
    }

    //! Переход к следующей строке
    LineViewLinePosInfoExIterator getNextLineBegin() const
    {
        return BaseImpl::getNextLineBegin();
    }

    //! Возвращает итератор конца
    LineViewLinePosInfoExIterator getEndIterator() const
    {
        return BaseImpl::getEndIterator();
    }


}; // class LineViewLinePosInfoExIterator





//----------------------------------------------------------------------------
//! Итератор по символам входных данных посредством LineView
/*! 

    \tparam CharType         Тип символов - char, wchar_t etc.
    \tparam SizeType         Тип размера/индексов - обычно std::size_t.
    \tparam DataGetter       Тип, возвращающий данные файла по его fileId
    \tparam LineViewIterator Тип итератора по последовательности LineView
 */
template< typename CharType, typename SizeType, typename DataGetter, typename LineViewIterator >
class LineViewSymbolIterator : public LineViewSymbolIteratorBaseImpl<CharType, SizeType, DataGetter, LineViewIterator>
{

public:

    typedef LineViewSymbolIteratorBaseImpl<CharType, SizeType, DataGetter, LineViewIterator>  BaseImpl; //!< typedef для родительского класса

    //! Конструктор из LineViewSymbolIteratorBaseImpl
    LineViewSymbolIterator( const BaseImpl &livsi ) : BaseImpl(livsi) {}

    //! Конструктор копирования
    LineViewSymbolIterator( const LineViewSymbolIterator &livsi ) : BaseImpl(livsi) {}

    //! Конструктор из DataGetter и LineViewIterator'а конца
    LineViewSymbolIterator( const DataGetter &dataGetter, LineViewIterator end, CharType dummyLinefeed = (CharType)0 )
    : BaseImpl(dataGetter, end, dummyLinefeed)
    {
    }

    //! Конструктор из LineViewLinePosInfoExIterator
    LineViewSymbolIterator( const LineViewLinePosInfoExIterator<CharType, SizeType, DataGetter, LineViewIterator> &livsi ) : BaseImpl(livsi) {}


public:

    //! Оператор разыменования
    const CharType& operator*() const
    {
         umba::LinePosInfoEx<SizeType,CharType> &lpi = dereferenceOperatorImpl();
         return *lpi.pData;
    }

    //! Префиксный инкремент
    LineViewSymbolIterator& operator++()
    {
        LineViewSymbolIteratorBaseImpl::operator++();
        return *this;

    }

    //! Постфиксный инкремент
    UMBA_DECLARE_DEPRECATED_FN_MSG( "Postfix version too slow, don't use it", LineViewSymbolIterator& operator++(int) )
    {
        //! \cond DOXYGEN_SUNDAY_NEVER_COMES
        LineViewSymbolIterator theThisCopy = *this; //!< warning: Member theThisCopy (variable) of class umba::LineViewLinePosInfoExIterator is not documented.
        this->operator++();
        return theThisCopy;
        //! \endcond
    }

    //! Сравнение на равенство
    bool operator==( const LineViewSymbolIterator &it )
    {
        return BaseImpl::operator==(it);
    }

    //! Сравнение на неравенство
    bool operator!=( const BaseImpl &it )
    {
        return BaseImpl::operator!=(it);
    }

    //! Переход к следующей строке
    LineViewSymbolIterator getNextLineBegin() const
    {
        return BaseImpl::getNextLineBegin();
    }

    //! Возвращает итератор конца
    LineViewSymbolIterator getEndIterator() const
    {
        return BaseImpl::getEndIterator();
    }


}; // class LineViewSymbolIterator


template< typename CharType, typename SizeType, typename DataGetter, typename LineViewIterator > inline
LineViewLinePosInfoExIterator<CharType, SizeType, DataGetter, LineViewIterator>::LineViewLinePosInfoExIterator( const LineViewSymbolIterator<CharType, SizeType, DataGetter, LineViewIterator> &livsi )
: LineViewLinePosInfoExIterator<CharType, SizeType, DataGetter, LineViewIterator>::BaseImpl(livsi) {}
//<CharType, SizeType, DataGetter, LineViewIterator>






//! Хелпер для создания итератора LineViewSymbolIterator. Возвращает базовый тип итератора, который преобразуется к нужному
template< typename CharType, typename SizeType, typename DataGetter, typename LineViewIterator > inline
LineViewSymbolIteratorBaseImpl< CharType, SizeType, DataGetter, LineViewIterator >
makeLineViewSymbolIterator( DataGetter dataGetter, LineViewIterator cur, LineViewIterator end, CharType dummyLF )
{
    LineViewSymbolIteratorBaseImpl< CharType, SizeType, DataGetter, LineViewIterator > res = 
        LineViewSymbolIteratorBaseImpl< CharType, SizeType, DataGetter, LineViewIterator >(dataGetter, end, dummyLF);
    //LineViewSymbolIteratorBaseImpl( const DataGetter &dataGetter, LineViewIterator cur, LineViewIterator end, CharType dummyLinefeed = (CharType)0 )

    // res.m_dataGetter     = dataGetter;
    // res.m_lviIterator    = b;
    // res.m_lviIteratorEnd = e;
    // res.m_dummyLinefeed  = dummyLF;

    res.initWithLviIterator(cur);
    return res;
}

#if 0
// Дубликат? Откуда взялся?
//! Хелпер для создания итератора LineViewSymbolIterator. Возвращает базовый тип итератора, который преобразуется к нужному
template< typename CharType, typename SizeType, typename DataGetter, typename LineViewIterator > inline
LineViewSymbolIteratorBaseImpl< CharType, SizeType, DataGetter, LineViewIterator >
makeLineViewSymbolIterator( DataGetter dataGetter, LineViewIterator cur, LineViewIterator end, CharType dummyLF )
{
    LineViewSymbolIteratorBaseImpl< CharType, SizeType, DataGetter, LineViewIterator > res = 
        LineViewSymbolIteratorBaseImpl< CharType, SizeType, DataGetter, LineViewIterator >(dataGetter, end, dummyLF);
    //LineViewSymbolIteratorBaseImpl( const DataGetter &dataGetter, LineViewIterator cur, LineViewIterator end, CharType dummyLinefeed = (CharType)0 )

    // res.m_dataGetter     = dataGetter;
    // res.m_lviIterator    = b;
    // res.m_lviIteratorEnd = e;
    // res.m_dummyLinefeed  = dummyLF;

    res.initWithLviIterator(cur);
    return res;
}
#endif


//! Хелпер для создания итератора LineViewSymbolIterator. Возвращает базовый тип итератора, который преобразуется к нужному
template< typename CharType, typename SizeType, typename DataGetter, typename LineViewIterator > inline
LineViewSymbolIteratorBaseImpl< CharType, SizeType, DataGetter, LineViewIterator >
makeLineViewSymbolIterator( DataGetter dataGetter, LineViewIterator cur, LineViewIterator end )
{
    return makeLineViewSymbolIterator< CharType, SizeType, DataGetter, LineViewIterator >( dataGetter, cur, end, (CharType)0 );
}

//! Хелпер для создания итератора конца LineViewSymbolIterator. Возвращает базовый тип итератора, который преобразуется к нужному
template< typename CharType, typename SizeType, typename DataGetter, typename LineViewIterator > inline
LineViewSymbolIteratorBaseImpl< CharType, SizeType, DataGetter, LineViewIterator >
makeLineViewSymbolIteratorEnd( DataGetter dataGetter, LineViewIterator end )
{
    return makeLineViewSymbolIterator< CharType, SizeType, DataGetter, LineViewIterator >( dataGetter, end, end );
}

//! Хелпер для создания итератора конца LineViewSymbolIterator. Возвращает базовый тип итератора, который преобразуется к нужному
template< typename CharType, typename SizeType, typename DataGetter, typename LineViewIterator > inline
LineViewSymbolIteratorBaseImpl< CharType, SizeType, DataGetter, LineViewIterator >
makeLineViewSymbolIteratorLineEnd( LineViewSymbolIteratorBaseImpl< CharType, SizeType, DataGetter, LineViewIterator > iter )
{
    return iter.getNextLineBegin();
}


namespace cpp{

//! Перегрузка umba::cpp::is_space для LinePosInfoEx
template< typename CharType, typename SizeType>
bool is_space( const umba::LinePosInfoEx<SizeType,CharType> &lvi )
{
    return umba::cpp::is_space( *lvi.pData );
}

//! Перегрузка umba::cpp::is_space_ot_tab для LinePosInfoEx
template< typename CharType, typename SizeType>
bool is_space_ot_tab( const umba::LinePosInfoEx<SizeType,CharType> &lvi )
{
    return umba::cpp::is_space_ot_tab( *lvi.pData );
}

//! Перегрузка umba::cpp::is_whitespace для LinePosInfoEx
template< typename CharType, typename SizeType>
bool is_whitespace( const umba::LinePosInfoEx<SizeType,CharType> &lvi )
{
    return umba::cpp::is_whitespace( *lvi.pData );
}

} // namespace cpp

/*
//! Операция ltrim
template <typename LineViewSymbolIteratorType, typename TrimPred> inline
LineViewSymbolIteratorType ltrim( LineViewSymbolIteratorType _b
                                , const LineViewSymbolIteratorType &e
                                , const TrimPred &isSpacePred
                                )
{
    LineViewSymbolIteratorType b = _b;
    for(; b!=e; ++b)
    {
        //if ( isSpacePred(*((*b).pData)) )
        if ( isSpacePred(*b) )
            continue;
        return b;
    }

    return e;
}
*/


/*

T& operator* ()
    {
        return *pData;
    }

    T* operator-> ()
    {
        return pData;
    }

    SizeType           fileId        ;
    SizeType           lineNumber    ;
    SizeType           viewPos       ;
    SizeType           viewSize      ;
    LineFeedType       lineFeedType  ;
    bool               toBeContinued ; // flag, that marks that next line is the continuation of current line 
    bool               modified      ; // line is modified and data itself is located in modified strings pool

    inline SizeType size() const { return viewSize; }

    inline bool empty() const { return viewSize==0; }


    //#if !defined(UMBA_MCU_USED)

        template< typename StringType > // const version of at, idx must be in range [0, size)
        const typename StringType::value_type& cat( const StringType &str, std::size_t idx ) const { return str[viewPos+idx]; }

*/

#if 0
template<typename CharType, typename SizeType  /* = std::size_t */ > inline
std::vector< umba::LineView< SizeType > >
splitToLineViews( const CharType *pData, SizeType sz, uint64_t fileId )
{
    std::vector< umba::LineView< SizeType > > res;
    res.reserve(sz/48);
    splitToLineViews( pData, sz, (sts::size_type)fileId, std::back_inserter(res) );
    return res;
}
#endif


// End of UMBA_LINEINFO
/*! @}*/


} // namespace umba


