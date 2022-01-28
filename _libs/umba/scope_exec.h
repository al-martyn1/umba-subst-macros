#pragma once

#include "stl.h"



//----------------------------------------------------------------------------

//! Простанство имён библиотеки UMBA
namespace umba{



//----------------------------------------------------------------------------
//! Выполняет заданные действия при объявлении и при окончании блока
template< typename ExecEnter, typename  ExecLeave >
class ScopeExec
{
    ExecEnter   m_execEnter;
    ExecLeave   m_execLeave;

public:

    ScopeExec() = delete;
    ScopeExec( const ScopeExec &scopeExec ) = delete;
    ScopeExec(ScopeExec&&) = default;
    ScopeExec& operator=(const ScopeExec &scopeExec) = delete;

    ScopeExec( const ExecEnter &execEnter, const ExecLeave &execLeave )
    : m_execEnter(execEnter)
    , m_execLeave(execLeave)
    {
        m_execEnter();
    }

    ~ScopeExec()
    {
        try
        {
            m_execLeave();
        }
        catch(...)
        {
            // Тупо ловим все исключения. Не айс, но обычно ничего страшного
        }
    }

}; // class ScopeExec


//----------------------------------------------------------------------------
//! Выполняет заданное действие при окончании блока
template< typename  ExecLeave >
class LeaveScopeExec
{
    ExecLeave   m_execLeave;

public:

    LeaveScopeExec() = delete;
    LeaveScopeExec( const LeaveScopeExec &scopeExec) = delete;
    LeaveScopeExec(LeaveScopeExec&&) = default;
    LeaveScopeExec& operator=(const LeaveScopeExec &scopeExec) = delete;

    LeaveScopeExec( const ExecLeave &execLeave ) : m_execLeave(execLeave) { }
    ~LeaveScopeExec()
    {
        try
        {
            m_execLeave();
        }
        catch(...)
        {
            // Тупо ловим все исключения. Не айс, но обычно ничего страшного
        }
    }

}; // class LeaveScopeExec

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
//! Конструирует ScopeExec - для С++ 11
template< typename ExecEnter, typename  ExecLeave >
inline
ScopeExec<ExecEnter, ExecLeave> makeScopeExec( const ExecEnter &execEnter, const ExecLeave &execLeave )
{
    //return std::move( ScopeExec<ExecEnter, ExecLeave>( execEnter, execLeave ) );
    return ScopeExec<ExecEnter, ExecLeave>( execEnter, execLeave );
}

//! Конструирует ScopeExec - если makeScopeExec не будет работать. execEnter, execLeave - надо объявить отдельно.
#define UMBA_MAKE_SCOPE_EXEC( execEnter, execLeave ) \
             umba::ScopeExec< decltype(execEnter), decltype(execLeave) >( execEnter, execLeave )

//----------------------------------------------------------------------------


//! Конструирует LeaveScopeExec - для С++ 11
template< typename  ExecLeave >
inline
LeaveScopeExec<ExecLeave> makeLeaveScopeExec( const ExecLeave &execLeave )
{
    //return std::move( LeaveScopeExec<ExecLeave>( execLeave ) );
    return LeaveScopeExec<ExecLeave>( execLeave );
}


//! Конструирует LeaveScopeExec - если makeLeaveScopeExec не будет работать. execLeave - надо объявить отдельно.
#define UMBA_MAKE_LEAVE_SCOPE_EXEC( execLeave ) \
             umba::LeaveScopeExec< decltype(execLeave) >( execLeave )





//----------------------------------------------------------------------------


} // namespace umba

