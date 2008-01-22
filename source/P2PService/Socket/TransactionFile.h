#if !defined(TransactionFile_H)
#define TransactionFile_H



#include <string>
using namespace std;


#include "Lock.h"
#include "SysEvent.h"

////////////////////////////////////////////////////////////////////////////////
// TransactionFile
//
// Purpose: text file class

class TransactionFile :
	public Lock,
	public SysEvent
{
	long	m_readOffset;			// current read offset
	long	m_writeOffset;			// current write offset
	DWORD	m_readWait;				// time (ms) to wait after read
	DWORD	m_writeWait;			// time (ms) to wait after write
	DWORD	m_openFlags;			// flags used to open file
	string	m_name;					// name of file

public:
	HANDLE	m_hFile;


	TransactionFile ();
	~TransactionFile (); 

	// show error
	void	showLastError	();
	bool	invalidFile		();

	// open methods
	bool	create			( string & fileName );
	bool	open			( string & fileName );
	bool	openAlways		( string & fileName );
	bool	open			( string & fileName, DWORD openFlags );
	void	close			();

	// file info/position methods
	bool	setPosition		( DWORD position, bool read = true );
	long	size			();
	bool	offsetPosition	( DWORD offset, bool read = true  );

	// clear the file
	void	clear			();

	// read/write methods
	bool	write			( LPTSTR pBuffer, DWORD noToWrite, bool wait = true );
	bool	read			( LPTSTR pBuffer, DWORD noToRead, bool wait = true );

	void    operator <<		( string & buffer )
	{
		write( (LPTSTR) buffer.c_str(), buffer.size() );
	}

	operator HANDLE () 
	{ 
		return m_hFile; 
	}

	void setReadWait ( DWORD wait )
	{ m_readWait = wait; }

	void setWriteWait ( DWORD wait )
	{ m_writeWait = wait; }

};


void operator << ( stringstream & strm, TransactionFile & file );
void operator << ( TransactionFile & file, stringstream & strm );


#endif