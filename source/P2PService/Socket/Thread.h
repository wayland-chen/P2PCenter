#if !defined(Thread_H)
#define Thread_H


#include "SysEvent.h"

//#define UseWin32Thread

#if !defined(UseWin32Thread)
#include <process.h>
#endif

////////////////////////////////////////////////////////////////////////
// Thread 

class Thread
{
public:
	enum
	{
		idCancel,
		idSuspend,
		idResume,
		idEnd,
		idNoEvents
	};

private:
	
	static long	m_delayTime;			// define time to delay for a thread sleep 

	void *		m_pThreadData;			// data for this thread

	HANDLE		m_hThread;				// handle to thread
	DWORD		m_idThread;				// id of thread
	SysEvent	m_events[idNoEvents];	// base thread events

public:
	Thread ();
	~Thread ();


	// create/destroy
	bool 		create			( void * pThreadData = NULL );
	void		release			();


	// thread methods
	void *		getData			()				{ return m_pThreadData; }
	HANDLE		getHandle		()				{ return m_hThread; }
	DWORD		getId			()				{ return m_idThread; }

	// event methods
	bool		createThreadEvents ();

	HANDLE		getEvent		( long id )		{ return m_events[id].getEvent(); }
	void		setEvent		( long id )		{ m_events[id].set(); }
	void		resetEvent		( long id )		{ m_events[id].reset(); }

	HANDLE		getEndEvent		()				{ return m_events[idEnd].getEvent(); }
	void		setEndEvent		()				{ m_events[idEnd].set(); }

	HANDLE		getCancelEvent	()				{ return m_events[idCancel].getEvent(); }
	void		setCancelEvent	()				{ m_events[idCancel].set(); }
	void		resetCancelEvent	()			{ m_events[idCancel].reset(); }

	// control
	void		stop			();
	void		suspend			();
	void		resume			();
	void		cancel			();


	// run method
	virtual long run  ();

	// virtual idle method 
	virtual void idle ()
	{}

	// static thread proc
	#if defined(UseWin32Thread)
	static DWORD WINAPI		 threadProc		( LPVOID parameter );

	#else
	static unsigned _stdcall threadProc		( LPVOID parameter );
	#endif
};


#endif