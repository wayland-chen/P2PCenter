#if !defined(SysEvent_H)
#define SysEvent_H






#include <string>
using namespace std;



////////////////////////////////////////////////////////////////////////
// SysEvent 

class SysEvent
{
	string		m_name;				// name of event
	HANDLE		m_hEvent;			// handle to user event
	bool		m_attached;			// shows if just attached to event

public:
	SysEvent () :
		m_name (),
		m_hEvent (NULL),
		m_attached(false)
	{}

	SysEvent ( SysEvent & event ) :
		m_name (),
		m_hEvent (NULL),
		m_attached(false)
	{ *this = event; }

	virtual ~SysEvent ()
	{ release(); }

	// create/release
	bool create ()
	{
		// release if needed
		release();

		// init name
		m_name = "";

		// create event
		m_attached = false;
		m_hEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL );	

		if ( m_hEvent == NULL )
			return false;
		else
			return true;
	}

	bool create ( LPTSTR name )
	{
		if ( !name )
			return false;

		// release if needed
		release();

		// create named event
		m_attached = false;
		m_hEvent = ::CreateEvent( NULL, TRUE, FALSE, (LPCTSTR) name );	

		if ( m_hEvent == NULL )
			return false;
		else
			return true;
	}

	void release ()
	{
		// if only attached just detach
		// else destroy event and name
		if ( m_attached )
			detach();
		else
		{
			if ( m_hEvent )
			{
				::CloseHandle(m_hEvent);
				m_hEvent = NULL;
			}

			if ( !m_name.empty() )
				m_name.erase();
		}
	}


	// attach/detach from event
	bool attached ()
	{ return m_attached; }

	bool attach	( HANDLE hEvent )
	{
		if ( !hEvent )
			return false;

		// get handle show attached
		m_hEvent = hEvent;
		m_attached = true;
		
		return true;
	}

	void detach		()
	{
		// if not attached stop
		if ( !m_attached )
			return;

		m_hEvent = NULL;
		if ( !m_name.empty() )
			m_name.erase();
		m_attached = false;
	}

	// set/reset event
	void set ()
	{
		::SetEvent( m_hEvent );
	}

	void reset ()
	{
		::ResetEvent( m_hEvent );
	}

	// get event handle
	HANDLE	getEvent ()
	{ return m_hEvent; }


	// ref operators
	HANDLE operator & ()		
	{ return m_hEvent; }

	operator HANDLE () 
	{ 
		return m_hEvent; 
	}

	// sort operators
	bool operator < ( SysEvent & event )
	{
		if ( m_hEvent < event.m_hEvent )
			return true;
		else
			return false;
	}

	bool operator == ( SysEvent & event )
	{
		if ( m_hEvent == event.m_hEvent )
			return true;
		else
			return false;
	}

	bool operator = ( SysEvent & event )
	{
		m_name = event.m_name;
		m_hEvent = event.m_hEvent;
		m_attached = event.m_attached;
	}

};


#endif