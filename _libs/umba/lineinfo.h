#pragma once

//-----------------------------------------------------------------------------

/*! 
    \file
    \brief LineInfo и сопутствующее. Устаревшая (непосредственно содержащая строку) версия API
 */

//-----------------------------------------------------------------------------

#include "exception.h"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "lineview.h"
#include "linefeedtype.h"
#include "lineposinfo.h"

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
namespace umba
{



/*! \ingroup UMBA_CORE
 *  \addtogroup UMBA_LINEINFO
 *  @{
 */



//----------------------------------------------------------------------------
//! Информация о строке файла, вместе со строкой
/*! 
    \tparam StringType Тип строки
 */

template<typename StringType = std::string>
struct LineInfo
{
    typename StringType::size_type   fileId;        //!< Идентификатор файла
    typename StringType::size_type   lineNumber;    //!< Номер строки
    StringType                       text;          //!< Текст строки
    LineFeedType                     lineFeedType;  //!< Тип перевода строки

}; // struct LineInfo

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
//! Создаёт экземпляр LineInfo
template<typename StringType > inline
LineInfo<StringType> makeLineInfo( std::size_t fileId        //!< Идентификатор файла
                                 , std::size_t lineNumber    //!< Номер строки
                                 , const StringType &text    //!< Текст строки
                                 , LineFeedType lineFeedType = lineFeedUnknown //!< Тип перевода строки
                                 )
{
    LineInfo<StringType> li;

    li.fileId       = fileId;
    li.lineNumber   = lineNumber;
    li.text         = text;
    li.lineFeedType = lineFeedType;

    return li;
}

//----------------------------------------------------------------------------
//! Создаёт экземпляр LineInfo из LineView и текста файла, заданного сырым указателем на данные
template<typename StringType > inline
LineInfo<StringType> makeLineInfo( const typename StringType::value_type *pAllText             //!< Текст файла
                                 , const LineView< typename StringType::size_type > &lineView  //!< Вьюшка строки
                                 )
{
    LineInfo<StringType> li;

    li.fileId       = lineView.fileId;
    li.lineNumber   = lineView.lineNumber;
    li.text         = StringType( pAllText + lineView.viewPos, lineView.viewSize );
    li.lineFeedType = lineView.lineFeedType;

    return li;
}

//----------------------------------------------------------------------------
//! Создаёт экземпляр LineInfo из LineView и текста файла, заданного сырым указателем на данные с учётом признака модифицированности
template<typename StringType > inline
LineInfo<StringType> makeLineInfo( const typename StringType::value_type *pAllText             //!< Текст файла
                                 , const typename StringType::value_type *pModifiedTextBuf     //!< Буфер изменений
                                 , const LineView< typename StringType::size_type > &lineView  //!< Вьюшка строки
                                 )
{
    return makeLineInfo<StringType>( lineView.modified ? pModifiedTextBuf : pAllText, lineView );
}

//----------------------------------------------------------------------------
//! Создаёт экземпляр LineInfo из LineView и текста файла, заданного строкой
template<typename StringType > inline
LineInfo<StringType> makeLineInfo( const StringType &allText                                   //!< Текст файла  
                                 , const LineView< typename StringType::size_type > &lineView  //!< Вьюшка строки
                                 )
{
    return makeLineInfo<StringType>( allText.c_str(), lineView );
}

//----------------------------------------------------------------------------
//! Создаёт экземпляр LineInfo из LineView и текста файла, заданного строкой с учётом признака модифицированности
template<typename StringType > inline
LineInfo<StringType> makeLineInfo( const StringType &allText                                    //!< Текст файла    
                                 , const StringType &modifiedTextBuf                            //!< Буфер изменений
                                 , const LineView< typename StringType::size_type > &lineView   //!< Вьюшка строки  
                                 )
{
    return makeLineInfo<StringType>( lineView.modified ? modifiedTextBuf : allText, lineView );
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
//! Создаёт вектор LineInfo из вектора LineView и текста файла, заданного сырым указателем на данные
template<typename StringType > inline
std::vector< LineInfo< StringType > > makeLineInfoVector( const typename StringType::value_type *pAllText                         //!< Текст файла  
                                                        , const std::vector< LineView< typename StringType::size_type > > &views  //!< Вектор вьюшек
                                                        )
{
    std::vector< LineInfo< StringType > > res;
    res.reserve( views.size() );

    bool prevLineToBeContinued = false;

    //std::vector< LineView< SizeType > >::const_iterator vit = views.begin();
    std::vector< LineView< typename StringType::size_type > >::const_iterator vit = views.begin();
    for(; vit != views.end(); ++vit )
    {
        LineInfo<StringType> newLineInfo = makeLineInfo<StringType>(pAllText,*vit);
        if (prevLineToBeContinued && !res.empty())
            res[res.size()-1].text.append( newLineInfo.text );
        else
            res.push_back( newLineInfo );

        prevLineToBeContinued = vit->toBeContinued;
    }

    return res;
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
//! Разбирает текст на строки и создаёт вектор LineInfo
template<typename StringType > inline
std::vector< LineInfo< StringType > >
splitToLineInfos( const typename StringType::value_type *pData    //!< Текст файла  
                , typename StringType::size_type         sz       //!< Размер файла
                , std::size_t                            fileId   //!< Идентификатор файла
                )
{
    typedef typename StringType::size_type   SizeType;
    typedef typename StringType::value_type  CharType;

    std::vector< LineView< SizeType > > views = splitToLineViews< CharType, SizeType >( pData, sz, fileId );

    //return makeLineInfoVector<LineInfo< SizeType > >( pData, views );
    return makeLineInfoVector< StringType >( pData, views );

    /*
    std::vector< LineInfo< StringType > > res;
    res.reserve( views.size() );

    bool prevLineToBeContinued = false;

    std::vector< LineView< SizeType > >::const_iterator vit = views.begin();
    for(; vit != views.end(); ++vit )
    {
        if (prevLineToBeContinued && !res.empty())
            res[res.size()-1].text.append( makeLineInfo<StringType>(pData,*vit).text );
        else
            res.push_back( makeLineInfo<StringType>(pData,*vit) );

        li.text
    }

    return res;
    */
}

//----------------------------------------------------------------------------
//! Разбирает текст на строки и создаёт вектор LineInfo
template<typename StringType > inline
std::vector< LineInfo< StringType > >
splitToLineInfos( const StringType &str      //!< Текст файла  
                , std::size_t       fileId   //!< Идентификатор файла
                )
{
    return splitToLineInfos<StringType>( str.data(), str.size(), fileId );
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
//! Формирует строку с читабельным концом строки для отладки
template<typename StringType > inline
StringType makeLineInfoDebugPresentationStr( const LineInfo< StringType > &li, LineFeedType lineFeedType = lineFeedUnknown )
{
    StringType strRes = li.text;

    switch(li.lineFeedType)
    {
        case lineFeedCRLF: 
        case lineFeedLFCR: 
        case lineFeedCR  : 
        case lineFeedLF  : 
                           strRes.append( makeLineFeedTypeDebugPresentationStr<StringType>(li.lineFeedType));
                           break;
        default          : 
        {
            if (lineFeedType != lineFeedUnknown)
            {
                strRes.append( makeLineFeedTypeDebugPresentationStr<StringType>(lineFeedType));
            }
        }
    }

    return strRes;
}

//----------------------------------------------------------------------------
//! Формирует строку с читабельным концом строки для отладки
template<typename StringType > inline
StringType makeLineInfoDebugPresentationStr( const std::vector< LineInfo< StringType > > &liVec )
{
    StringType res; res.reserve( liVec.size()*48 );

    std::vector< LineInfo< StringType > >::const_iterator lit = liVec.begin();
    for(; lit!=liVec.end(); ++lit)
    {
        res.append( makeLineInfoDebugPresentationStr<StringType>( *lit ) );
    }

    return res;
}

//----------------------------------------------------------------------------
//! Читает файл из std::istream& в вектор LineInfo
template<typename StringType>
inline
std::vector< LineInfo<StringType> > readLines( std::istream &is, size_t fileId )
{
    std::vector< LineInfo<StringType> > res;
    #ifdef UMBA_CXX_HAS_STD11
    res.reserve(10000);
    #else
    res.reserve(1000);
    #endif

    size_t lineNo = 0;
    std::string strReaded;

    while( std::getline( is, strReaded ) )
    {
        #ifdef UMBA_CXX_HAS_STD11

        res.push_back( LineInfo<StringType>{ fileId, lineNo++, strReaded } );

        #else

        LineInfo li;
        li.fileId     = fileId;
        li.lineNumber = lineNo++;
        li.text       = strReaded;
        res.push_back( li );

        #endif

    }

    #ifdef UMBA_CXX_HAS_STD11
    res.shrink_to_fit();
    #endif

    return res;
}


// End of UMBA_LINEINFO
/*! @}*/


} // namespace umba



