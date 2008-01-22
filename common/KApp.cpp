 /****************************************************************************
 *
 *  Copyright (C) 2001-2006 Kevin(fuwenke@gmail.com), Inc. All rights reserved.
 *
 *  This program is free software.  It may be distributed under the terms
 *  in the file LICENSE, found in the top level of the source distribution.
 *
 */

#include "KApp.h"
#include "misc/KDebug.h"

#ifndef _WIN32
#include <signal.h>
#endif

CKApp* g_pApp = NULL;

#ifdef _UNIX
extern pthread_key_t g_keyself;
#endif
#ifdef _WIN32
extern DWORD g_tlsself;
#endif

/*****************************************************************************
 *
 * CKApp
 *
 *****************************************************************************/

CKApp::CKApp(  )
{
#ifdef _WIN32
	//WSADATA wd;
	//int res = ::WSAStartup( MAKEWORD(2,0), &wd );
	//if( res != 0 || wd.wVersion < MAKEWORD(2,0) )
	//{
	//	dbgout( "Cannot start winsock 2.0" );
	//}
#endif

    g_pApp = this;
    atexit( dump_alloc_heaps );
#ifdef _UNIX
    pthread_key_create( &g_keyself, NULL );
    pthread_setspecific( g_keyself, this );
#endif
#ifdef _WIN32
    //g_tlsself = ::TlsAlloc();
    //::TlsSetValue( g_tlsself, this );
	g_tlsself = (DWORD)this;
#endif
}

CKApp::~CKApp( void )
{
#ifdef _UNIX
    pthread_key_delete( g_keyself );
#endif
#ifdef _WIN32
    //::TlsFree( g_tlsself );
	g_tlsself = 0;
#endif

#ifdef _WIN32
	::WSACleanup();
#endif
}

bool CKApp::StartApp( void )
{
	m_bWantStop = false;
	return StartThread();
}

void CKApp::StopApp( void )
{
	m_bWantStop = true;
	WaitForStop();
}

bool CKApp::Init( void )
{
	return true;
}

int CKApp::Exit( void )
{
    return 0;
}

/*
 * Become process lead and detach from tty
 * Code lifted from fetchmail 5.0.0
 */
void CKApp::Daemonize( void )
{
#ifdef _UNIX
  /* Ignore BSD terminal stop signals */
#ifdef  SIGTTOU
    signal(SIGTTOU, SIG_IGN);
#endif
#ifdef  SIGTTIN
    signal(SIGTTIN, SIG_IGN);
#endif
#ifdef  SIGTSTP
    signal(SIGTSTP, SIG_IGN);
#endif

    /* Change to root dir so we don't hold any mount points open */
    if( chdir( "/" ) !=  0 )
    {
        dbgout( "chdir failed (%s)", strerror(errno) );
    }

    /* In case we were not started in the background, fork and let
       the parent exit.  Guarantees that the child is not a process
       group leader */

    pid_t childpid;
    if( (childpid = fork()) < 0 )
    {
        dbgout( "fork failed (%s)", strerror(errno) );
        return;
    }
    else if( childpid > 0 )
    {
        exit( 0 );  /* parent */
    }


    /* Make ourselves the leader of a new process group with no
       controlling terminal */

    /* POSIX makes this soooo easy to do */
    if( setsid() < 0 )
    {
        dbgout( "setsid failed (%s)", strerror(errno) );
        return;
    }

    /* lose controlling tty */
    signal( SIGHUP, SIG_IGN );
    if( (childpid = fork()) < 0 )
    {
        dbgout( "fork failed (%s)", strerror(errno) );
        return;
    }
    else if( childpid > 0 )
    {
        exit( 0 );    /* parent */
    }
#endif
}
