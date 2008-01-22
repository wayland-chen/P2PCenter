// TextFile.cpp: implementation of the CColumn class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Thread.h"


#pragma warning( disable:4800 )

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

long Thread::m_delayTime = 500;

/////////////////////////////////////////////////////////////////////////////////
// Thread

Thread::Thread () :

	m_pThreadData(0),			// data for this thread
	m_hThread(0),				// handle to thread
	m_idThread(0)				// id of thread
{}

Thread::~Thread ()

{
	release();
}

// create/destroy
bool Thread::create ( void * pThreadData )
{
	// get ptr to data
	m_pThreadData = pThreadData;

	// create events
	if ( !createThreadEvents() )
		return false;

	// create thread

	#if defined(UseWin32Thread)

	m_hThread = 
	CreateThread(NULL,0, Thread::threadProc, (LPVOID) this, 0, &m_idThread );

	#else

	m_hThread = (HANDLE)
	_beginthreadex( NULL, 0, Thread::threadProc, (LPVOID) this, 0,
                    (unsigned int *) &m_idThread );

	#endif

	if ( m_hThread == 0 )
		return false;

	// success so start thread
	//::ResumeThread( m_hThread );

	return true;
}

void Thread::release ()
{
	// if valid stop thread
	if ( m_hThread )
	{
		// wait for thread end
		stop();

		// release events
		for ( long id=0; id<idNoEvents; id++ )
			m_events[id].release();

		// destroy thread
		::CloseHandle( m_hThread );

		m_hThread  = 0;
		m_idThread = 0;
	}
}

bool Thread::createThreadEvents ()
{
	// create events
	for ( long id=0; id<idNoEvents; id++ )
		m_events[id].create();

	return true;
}

// control
void Thread::stop ()
{
	// set stop event and wait
	m_events[idEnd].set();
	::Sleep(m_delayTime);

}


void Thread::cancel ()
{
	// set stop event and wait
	m_events[idCancel].set();
	::Sleep(m_delayTime);

}

void Thread::suspend ()
{
	// set stop event and wait
	m_events[idSuspend].set();
	::Sleep(m_delayTime);

}

void Thread::resume	()
{
	// set stop event and wait
	m_events[idResume].set();
	::Sleep(m_delayTime);

}

// thread proc

#if defined(UseWin32Thread)

DWORD WINAPI	Thread::threadProc ( LPVOID parameter )

#else

unsigned _stdcall Thread::threadProc ( LPVOID parameter )

#endif

{
	if ( !parameter )
		return -1;

	// start thread
	Thread * pThread = (Thread *) parameter;
	int ret = pThread->run();

	// exit the thread
	#if defined(UseWin32Thread)

	ExitThread( -1 );

	#else

	_endthreadex( -1 );

	#endif


	return ret;
}

long Thread::run ()
{
	// base class run does nothing
	return -1;
}

