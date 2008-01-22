#if !defined(Pipe_H)
#define Pipe_H


#include "SecurityAttributes.h"



#include <string>
#include <iostream>
using namespace std;


///////////////////////////////////////////////////////////////////////////////////
// Pipe

class Pipe
{
public:

	bool   _ownWrite;			// shows if close read on destroy
    HANDLE _hWrite;				// write handle
	bool   _ownRead;			// shows if close read on destroy
	HANDLE _hRead;				// read handle

	Pipe () :

		_hWrite(0),
		_hRead(0),
		_ownWrite(true),
		_ownRead(true)

	{}

	virtual ~Pipe ()
	{
		release(); 
	}

	// create/release
	bool create ( bool inherit = true )
	{

		// Set up the security attributes struct.
		SECURITY_ATTRIBUTES sa;
		sa.nLength              = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle       = (BOOL) inherit;

		// create the pipe
		if (!CreatePipe(&_hRead,&_hWrite,&sa,0))
			return false;
		else 
			return true;

	}

	bool create ( HANDLE hRead, HANDLE hWrite )
	{
		if ( hRead == 0 || hWrite == 0 )
			return false;

		// get read handle
		_hRead   = hRead;
		_ownRead = true;

		// get write handle
		_hWrite   = hWrite;
		_ownWrite = true;

		return true;
	}

	void release ()
	{
		close();
	}

	// dupicate handle
	HANDLE duplicateRead ( bool inherit, bool sameAccess  )
	{
		HANDLE hProcess = GetCurrentProcess();
		return duplicate( _hRead, hProcess, inherit, sameAccess );
	}

	HANDLE duplicateWrite ( bool inherit, bool sameAccess  )
	{
		HANDLE hProcess = GetCurrentProcess();
		return duplicate( _hWrite, hProcess, inherit, sameAccess );
	}

	static
	HANDLE duplicate ( HANDLE hPipe, 
		               HANDLE hProcess,
					   bool inherit, bool sameAccess )
	{
		// handle to duplicated pipe
		HANDLE hDupPipe;

		// get options
		DWORD dwOptions = DUPLICATE_CLOSE_SOURCE;
		if ( sameAccess )
			dwOptions = DUPLICATE_SAME_ACCESS;

		// get process
		if ( hProcess == 0 )
			hProcess = GetCurrentProcess();

		// Create a duplicate of the output write handle for the std error
		// write handle. This is necessary in case the child application
		// closes one of its std output handles.
		BOOL success = 

		DuplicateHandle( hProcess, hPipe,
						 hProcess, &hDupPipe, 
						 0, (BOOL) inherit, dwOptions );

		if ( success )
			return hDupPipe;
		else
			return 0;
	}

	// attach to handle
	void attachRead ( HANDLE hRead )
	{
		// if already exists stop
		if ( _hRead == 0 )
			return;

		// get read handle
		_hRead   = hRead;
		_ownRead = false;
	}

	// attach to handle
	void attachWrite ( HANDLE hWrite )
	{
		// if already exists stop
		if ( _hWrite == 0 )
			return;

		// get write handle
		_hWrite   = hWrite;
		_ownWrite = false;
	}

	// get handles
	HANDLE getWrite ()
	{
		return _hWrite;
	}

	HANDLE getRead ()
	{
		return _hRead;
	}

	// get handles
	void close ()
	{
		closeRead();
		closeWrite();
	}

	void closeWrite ()
	{
		if ( _hWrite = 0 )
			return;

		CloseHandle(_hWrite);
		_hWrite = 0;
	}

	void closeRead ()
	{
		if ( _hRead = 0 )
			return;

		CloseHandle(_hRead);
		_hRead = 0;
	}




	// read/write pipe
	long read ( char * readBuf, long readSz )
	{
		// if no read handle stop
		if ( _hRead == 0 )
			return 0;

		// read from pipe
		DWORD nBytesRead;
	
		BOOL success = 
		ReadFile( _hRead,readBuf,readSz,&nBytesRead,NULL );
	
		if ( success )
			return nBytesRead;
		else
		{
			DWORD error = GetLastError();
			return 0;
		}
	}

	long write ( char * writeBuf, long writeSz )
	{
		// if no write handle stop
		if ( _hWrite == 0 )
			return 0;

		// read from pipe
		DWORD nBytesWritten;
	
		BOOL success = 
		WriteFile( _hWrite,writeBuf,writeSz,&nBytesWritten,NULL );
	
		if ( success )
		{
			return nBytesWritten;
		}
		else
		{
			DWORD error = GetLastError();

			return 0;
		}
	}

	
	// write
	Pipe & operator << ( string & str )
	{
		// get length
		long bytesToWrite = str.size() + 1;

		// write buffer
		long bytesWritten = 
		write( (LPTSTR) str.c_str(), bytesToWrite );
		return *this;
	}

	Pipe & operator << ( LPCTSTR str )
	{
		// get length
		long bytesToWrite = strlen(str);

		// write buffer
		long bytesWritten = 
		write( (LPTSTR) str, bytesToWrite );
		return *this;
	}

	Pipe & operator << ( LPTSTR str )
	{
		// get length
		long bytesToWrite = strlen(str);

		// write buffer
		long bytesWritten = 
		write( str, bytesToWrite );
		return *this;
	}

	// read
	Pipe & operator >> ( string & str )
	{
		// read from pipe
		char buffer[1024];

		long bytesRead =
		read( buffer, 1024 );
		buffer[bytesRead] = '\0';

		// if got something read it
		if ( bytesRead > 0 )
			str = buffer;

		return *this;
	}
};



#endif