#pragma once

//-----------------------------------------------------------------------------

/*! 
    \file
    \brief Вывод битсетов в поток
 */

//-----------------------------------------------------------------------------

#include "umba/bits.h"
#include "umba/simple_formatter.h"


namespace umba
{

namespace bits
{


template< typename RawPtrTypeT, typename BitsStorageTypeT, typename BitSetSizeTypeT>
inline
umba::SimpleFormatter& operator<<( umba::SimpleFormatter& fmt, const BitSetHexView<RawPtrTypeT, BitsStorageTypeT, BitSetSizeTypeT> &v )
{
    v.m_pBitSet->makeHexView( fmt.getCharWritter(), v.m_groupSize );
    return fmt;
}

template< typename RawPtrTypeT, typename BitsStorageTypeT, typename BitSetSizeTypeT>
inline
umba::SimpleFormatter& operator<<( umba::SimpleFormatter& fmt, const BitSetBinView<RawPtrTypeT, BitsStorageTypeT, BitSetSizeTypeT> &v )
{
    v.m_pBitSet->makeBinView( fmt.getCharWritter(), v.m_groupSize );
    return fmt;
}


} // namespace bits

} // namespace umba

