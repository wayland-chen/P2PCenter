// TransactionFile.cpp: implementation of the TransactionFile class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "TransactionFile.h"


#include <sstream>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////
// operator <<
//
// Purpose:		stream operators for file

void operator << ( stringstream & strm, TransactionFile & file )
{
	// get buffer
	long	sz     = file.size();
	TCHAR * buffer = new TCHAR[sz+1];
	if ( !buffer )
		return;

	// if read in, put buffer into stream
	if ( file.read(buffer,sz) )
	{
		buffer[sz] = '\0';
		strm << buffer;
	}

	// release buffer
	delete buffer;
}

void operator << ( TransactionFile & file, stringstream & strm )
{
	// get buffer
	long    sz     = strm.str().size();
	TCHAR * buffer = (TCHAR *) strm.str().c_str();
	if ( !buffer )
		return;

	// write buffer to file
	file.write(buffer,sz);
}


/////////////////////////////////////////////////////////////
// TransactionFile



TransactionFile::TransactionFile () :
	Lock(),
	SysEvent(),
	m_hFile(0),						// handle to file
	m_readOffset(0),				// current read offset
	m_writeOffset(0),				// current write offset
	m_readWait(INFINITE),			// time (ms) to wait after read
	m_writeWait(INFINITE),			// time (ms) to wait after write
	m_openFlags(0),					// flags used to open file
	m_name("")						// name of file

{
	SysEvent::create();
}

TransactionFile::~TransactionFile () 
{
	destroyLock();
	SysEvent::release();
	close();
}


void TransactionFile::showLastError ()
{
	LPVOID lpMsgBuf;
	DWORD  error = GetLastError();
	FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				   FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,    
				   NULL,
				   error,
				   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				   (LPTSTR) &lpMsgBuf,    0,    NULL );		  // Process any inserts in lpMsgBuf.

	// Display the string.
	AtlTrace("%s\n",(const char *) lpMsgBuf);

	// Free the buffer.
	LocalFree( lpMsgBuf );
}



bool TransactionFile::invalidFile ()
{
	if ( m_hFile == INVALID_HANDLE_VALUE )
		return true;
	else
		return false;
}

bool TransactionFile::create ( string & fileName )
{
	DWORD openFlags = CREATE_ALWAYS;
	return open(fileName,openFlags);
}

bool TransactionFile::open ( string & fileName )
{
	DWORD openFlags = OPEN_ALWAYS;
	return open(fileName,openFlags);
}


bool TransactionFile::openAlways ( string & fileName )
{
	DWORD openFlags = OPEN_ALWAYS;
	return open(fileName,openFlags);
}

bool TransactionFile::open ( string & fileName, DWORD openFlags )
{
	if ( !lock() )
		return false;

	// open the file
	m_hFile = 
	CreateFile( fileName.c_str(),
				GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				openFlags,
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 
				0 );

	unlock();

	if ( m_hFile == INVALID_HANDLE_VALUE )
	{
		showLastError();
		return false;
	}

	// store file info
	m_openFlags = openFlags;
	m_name = fileName;
	m_readOffset = 0;
	m_writeOffset = 0;

	return true;
}

void TransactionFile::close ()
{
	if ( !lock() )
		return;

	if ( m_hFile == INVALID_HANDLE_VALUE )
		return;

	::CloseHandle(m_hFile);
	m_hFile = INVALID_HANDLE_VALUE;

	unlock();
}


bool TransactionFile::offsetPosition ( DWORD offset, bool read )
{
	if ( !lock() )
		return false;

	if ( m_hFile == INVALID_HANDLE_VALUE )
		return false;

	if ( read )
		m_readOffset  += offset;
	else
		m_writeOffset += offset;

	unlock();
	return true;
}


bool TransactionFile::setPosition ( DWORD position, bool read )
{
	if ( !lock() )
		return false;

	if ( m_hFile == INVALID_HANDLE_VALUE )
		return false;

	if ( read )
		m_readOffset  = position;
	else
		m_writeOffset = position;

	unlock();
	return true;
}


void TransactionFile::clear ()
{
	if ( !lock() )
		return;

	if ( m_hFile == INVALID_HANDLE_VALUE )
		return;

	::CloseHandle(m_hFile);
	DeleteFile( m_name.c_str() );

	bool success = 
	open(m_name,m_openFlags);

	unlock();
}

long TransactionFile::size ()
{
	if ( !lock() )
		return 0;

	if ( m_hFile == INVALID_HANDLE_VALUE )
		return 0;

	DWORD dwSize =
	::GetFileSize(m_hFile, NULL);

	unlock();
	return dwSize;
}

bool TransactionFile::write ( LPTSTR pBuffer, DWORD noToWrite, bool wait )
{
	if ( !lock() )
		return false;

	// set overlap for write
	OVERLAPPED overlap;

	HANDLE hEvent      = getEvent();
	overlap.hEvent     = hEvent;
	overlap.Offset     = m_writeOffset;
	overlap.OffsetHigh = 0;

	bool success = false;
	try
	{
		// write to file
		DWORD noWritten = 0;
		if ( WriteFile(m_hFile,(LPVOID)pBuffer,noToWrite,&noWritten,&overlap) != 0 )
			success = true;

		if ( wait && m_writeWait > 0 )
		{
			DWORD result = WaitForSingleObject( hEvent, m_writeWait );
			switch ( result )
			{
				case WAIT_OBJECT_0:
					break;

				case WAIT_ABANDONED:
				case WAIT_TIMEOUT:
				default:
					return false;

			}
		}

		// if write success update write offset
		if ( noWritten == noToWrite )
			m_writeOffset += noWritten;

	}
	catch(...)
	{}


	unlock();

	return success;
}

bool TransactionFile::read ( LPTSTR pBuffer, DWORD noToRead, bool wait )
{
	//if ( !lock() )
	//	return false;

	// set overlap for write
	OVERLAPPED overlap;

	HANDLE hEvent      = getEvent();
	overlap.hEvent     = hEvent;
	overlap.Offset     = m_readOffset;
	overlap.OffsetHigh = 0;

	bool success = false;
	try
	{
		// read from file
		DWORD noRead;
		DWORD result = ReadFile(m_hFile,(LPVOID)pBuffer,noToRead,&noRead,&overlap);
		if ( result != 0 )
			success = true;

		if ( wait && m_readWait > 0 )
		{
			DWORD result = WaitForSingleObject( hEvent, m_readWait );
			switch ( result )
			{
				case WAIT_OBJECT_0:
					break;

				case WAIT_ABANDONED:
				case WAIT_TIMEOUT:
				default:
					return false;

			}
		}

		// if read success update read offset 
		if ( noRead == noToRead )
			m_readOffset += noRead;

	}
	catch(...)
	{}

	unlock();

	return success;
}
