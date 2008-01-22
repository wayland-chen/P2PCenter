/****************************************************************************
 *
 *  Copyright (C) 2001-2006 Kevin(fuwenke@gmail.com), Inc. All rights reserved.
 *
 *  This program is free software.  It may be distributed under the terms
 *  in the file LICENSE, found in the top level of the source distribution.
 *
 */

#include "KTimer.h"
//#include "KApp.h"
#include "KThread.h"
#include "../misc/KDebug.h"

#define TVNORM(tv) while( tv.tv_usec > 1000*1000 ) { tv.tv_usec -= 1000*1000; tv.tv_sec++; }

CKTimer::CKTimer( CKTimerResponse* pResponse ) :
    m_pResponse(pResponse),
    m_mode(Disabled),
    m_next(0),
    m_interval(0)
{
    // Empty
}

CKTimer::~CKTimer( void )						
{
    Disable();
}

void CKTimer::SetResponse( CKTimerResponse* pResponse )
{
    m_pResponse = pResponse;
}

CKTimer::Mode CKTimer::GetMode( void )
{
    return m_mode;
}

UINT32 CKTimer::GetTimeout( void )
{
    return m_next;
}

void CKTimer::SetRelative( UINT32 msec )
{
    Set( CurrentTime() + msec );
    m_mode = SingleShot;
}

void CKTimer::SetAbsolute( UINT32 t )
{
    Set( t );
    m_mode = SingleShot;
}

void CKTimer::SetRepeating( UINT32 msec )
{
    Set( CurrentTime() + msec );
    m_interval = msec;
    m_mode = Repeating;
}

void CKTimer::Disable( void )
{
    if( m_mode != Disabled )
    {
        CKEventThread* pSelf;
        m_mode = Disabled;
#ifdef NO_RTTI
        pSelf = (CKEventThread*)CKThread::This(); //XXX: very bad, upgrade compiler
#else
        pSelf = (CKEventThread*)CKThread::This();
#endif
        assert_or_ret( pSelf );
        pSelf->DelTimer( this );
    }
}

UINT32 CKTimer::CurrentTime( void )
{
#ifdef _UNIX
    timeval tv;
    gettimeofday( &tv, NULL );
    return (tv.tv_sec*1000 + tv.tv_usec/1000);
#endif
#ifdef _WIN32
    return ::GetTickCount();
#endif
}

void CKTimer::Set( UINT32 t )
{
    m_next = t;

    CKEventThread* pSelf;
#ifdef NO_RTTI
        pSelf = (CKEventThread*)CKThread::This(); //XXX: very bad, upgrade compiler
#else
        pSelf = (CKEventThread*)CKThread::This();
#endif
    assert_or_ret( pSelf );

    if( m_mode != Disabled ) pSelf->DelTimer( this );
    pSelf->AddTimer( this );
}

CKTimerResponse* CKTimer::GetResponse( void )
{
    assert( m_pResponse );
    return m_pResponse;
}
