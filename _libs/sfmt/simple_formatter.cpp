#include <algorithm>

#include "inc/umba/simple_formatter.h"
#include "umba/preprocessor.h"



namespace umba
{

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
SimpleFormatter::SimpleFormatter(ICharWriter *charWriter)
    : m_charWriter(charWriter)
    , m_charWriterProxy( this )
    , m_disableStack()
    , m_disableOutput(false)
{
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
SimpleFormatter::SimpleFormatter()
    : m_charWriter(0)
    , m_charWriterProxy( this )
    , m_disableStack()
    , m_disableOutput(false)
{
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
SimpleFormatter::FormatFlags SimpleFormatter::flags() const
{
    return m_formatState.flags;
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
SimpleFormatter::FormatFlags SimpleFormatter::flags( SimpleFormatter::FormatFlags flags )
{
    FormatFlags res = m_formatState.flags;
    m_formatState.flags = flags;
    return res;
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
SimpleFormatter::FormatFlags SimpleFormatter::setf( SimpleFormatter::FormatFlags flags )
{
    FormatFlags res = m_formatState.flags;
    m_formatState.flags |= flags;
    return res;
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
SimpleFormatter::FormatFlags SimpleFormatter::setf( SimpleFormatter::FormatFlags flags, SimpleFormatter::FormatFlags mask )
{
    FormatFlags res = m_formatState.flags;
    m_formatState.flags &= ~mask;
    m_formatState.flags |= flags&mask;
    return res;
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
SimpleFormatter::FormatFlags SimpleFormatter::unsetf( SimpleFormatter::FormatFlags mask )
{
    FormatFlags res = m_formatState.flags;
    m_formatState.flags &= ~mask;
    return res;
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
SimpleFormatter::FormatState SimpleFormatter::getState() const
{
    return m_formatState;
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
void SimpleFormatter::setState( SimpleFormatter::FormatState &st )
{
    m_formatState = st;
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
unsigned SimpleFormatter::getColor(ColoringLevel lvl)
{
    unsigned idx = (unsigned)lvl;
    if (idx >= (unsigned)umba::ColoringLevel::num_levels)
       idx = (unsigned)umba::ColoringLevel::normal;
    return m_coloringLevelColors[idx];
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
void SimpleFormatter::setColor(ColoringLevel lvl, umba::term::colors::SgrColor clr )
{
    unsigned idx = (unsigned)lvl;
    if (idx >= (unsigned)umba::ColoringLevel::num_levels)
       idx = (unsigned)umba::ColoringLevel::normal;
    m_coloringLevelColors[idx] = clr;
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
void SimpleFormatter::coloring( ColoringLevel lvl )
{
    //coloring(getColor(lvl));
/*
    if (lvl==ColoringLevel::normal)
    {
        if (m_charWriter)
            m_charWriter->setDefaultTermColors();
    }
    else
*/
    {
        coloring(getColor(lvl));
    }
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
void SimpleFormatter::coloring( umba::term::colors::SgrColor clr )
{
    if (m_disableOutput) return;
    if (m_charWriter)
        m_charWriter->setTermColors(clr);
}

//-----------------------------------------------------------------------------
// for iomanips - each manip must call saveFormatState
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
void SimpleFormatter::saveFormatState()
{
    if (m_stateSaved)
       return;
    m_formatStateSaved = m_formatState;
    m_stateSaved = true;
}

//-----------------------------------------------------------------------------
// each builtin output operation must call restoreFormatState before exiting
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
void SimpleFormatter::restoreFormatState()
{
    if (!m_stateSaved)
        return;
    m_formatState = m_formatStateSaved;
    m_stateSaved = false;
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
int SimpleFormatter::width() const
{
    return m_formatState.width;
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
int SimpleFormatter::width( int w )
{
    int res = m_formatState.width;
    m_formatState.width = w;
    return res;
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
int SimpleFormatter::precision() const
{
    return m_formatState.precision;
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
int SimpleFormatter::precision( int p )
{
    int res = m_formatState.precision;
    m_formatState.precision = p;
    return res;
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
char SimpleFormatter::fill() const
{
    return m_formatState.fill;
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
char SimpleFormatter::fill( char c )
{
    int res = m_formatState.fill;
    m_formatState.fill = c;
    return res;
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
int SimpleFormatter::groupsize() const
{
    return m_formatState.groupSize;
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
int SimpleFormatter::groupsize(int size)
{
    int res = m_formatState.groupSize;
    m_formatState.groupSize = size;
    return res;
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
int SimpleFormatter::decgroupsize() const
{
    return m_formatState.decGroupSize;
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
int SimpleFormatter::decgroupsize(int size)
{
    int res = m_formatState.decGroupSize;
    m_formatState.decGroupSize = size;
    return res;
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
char SimpleFormatter::groupsep() const
{
    return m_formatState.groupSep;
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
char SimpleFormatter::groupsep(char sepCh)
{
    char res = m_formatState.groupSep;
    m_formatState.groupSep = sepCh;
    return res;
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
char SimpleFormatter::decgroupsep() const
{
    return m_formatState.decGroupSep;
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
char SimpleFormatter::decgroupsep(char sepCh)
{
    char res = m_formatState.decGroupSep;
    m_formatState.decGroupSep = sepCh;
    return res;
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
char SimpleFormatter::decpoint() const
{
    return m_formatState.decimalPoint;
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
char SimpleFormatter::decpoint(char pntCh)
{
    char res = m_formatState.decimalPoint;
    m_formatState.decimalPoint = pntCh;
    return res;
}
    
//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
int SimpleFormatter::baseFromFlags(SimpleFormatter::FormatFlags flags) const
{
    flags &= basefield;
    switch(flags)
       {
        case dec:   return 10;
        case bin:   return  2;
        case oct:   return  8;
        default:    return  16;
       }
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
SimpleFormatter::FormatFlags SimpleFormatter::baseToFlags(int b) const
{
    switch(b)
       {
        case 2 : return bin;
        case 8 : return oct;
        case 16: return hex;
        default: return dec;
       }
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
int SimpleFormatter::base() const
{
    return baseFromFlags(flags());
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
int SimpleFormatter::base(int b)
{
    int res = base();
    m_formatState.flags = (m_formatState.flags & ~basefield) | baseToFlags(b);
    return res;
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
void SimpleFormatter::flush()
{
    if (m_disableOutput) return;
    m_charWriter->flush();
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
void SimpleFormatter::waitFlushDone()
{
    if (m_disableOutput) return;
    m_charWriter->waitFlushDone();
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
void SimpleFormatter::putEndl()
{
    if (m_disableOutput) return;
    if (m_charWriter) 
        m_charWriter->putEndl();
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
void SimpleFormatter::putCR()
{
    if (m_disableOutput) return;
    if (m_charWriter) 
        m_charWriter->putCR();
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
void SimpleFormatter::putFF()
{
    if (m_disableOutput) return;
    if (m_charWriter) 
        m_charWriter->putFF();
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
void SimpleFormatter::writeBuf( const uint8_t *pBuf, size_t sz )
{
    if (m_disableOutput) return;
    if (m_charWriter) 
        m_charWriter->writeBuf(pBuf, sz);
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
void SimpleFormatter::writeBuf( const char *pBuf, size_t sz )
{
    writeBuf((const uint8_t*)pBuf, sz);
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
void SimpleFormatter::pushLock( bool disableOutput )
{
    m_disableStack.push( m_disableOutput );
    m_disableOutput |= disableOutput;
}

//-----------------------------------------------------------------------------
UMBA_SIMPLE_FORMATTER_INLINE_FUNCTION
void SimpleFormatter::popLock()
{
    m_disableOutput = m_disableStack.top();
    m_disableStack.pop();
}

}; // namespace umba

