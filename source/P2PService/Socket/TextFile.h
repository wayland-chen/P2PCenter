#if !defined(TextFile_H)
#define TextFile_H

#include <sstream>
#include <string>
using namespace std;


#include "Lock.h"
#include "TimeStamp.h"




/////////////////////////////////////////////////////////////////////////////
// File status

struct TextFileStatus
{
	TimeStamp	_ctime;					// creation date/time of file
	TimeStamp	_mtime;					// last modification date/time of file
	TimeStamp	_atime;					// last access date/time of file
	LONG		_size;					// logical size of file in bytes
	DWORD		_attribute;				// logical OR of CFile::Attribute enum values
	TCHAR		_szFullName[_MAX_PATH];	// absolute path name

};

////////////////////////////////////////////////////////////////////////////////
// TextFile
//
// Purpose: text file class


class TextFile :
	private Lock
{
	DWORD	_openFlags;			// flags used to open file
	string	_name;				// name of file

public:
	HANDLE	_hFile;


	TextFile ();
	~TextFile (); 

	enum OpenFlags {
		modeRead =          0x0000,
		modeWrite =         0x0001,
		modeReadWrite =     0x0002,
		shareCompat =       0x0000,
		shareExclusive =    0x0010,
		shareDenyWrite =    0x0020,
		shareDenyRead =     0x0030,
		shareDenyNone =     0x0040,
		modeNoInherit =     0x0080,
		modeCreate =        0x1000,
		modeNoTruncate =    0x2000,
		typeText =          0x4000, // typeText and typeBinary are used in
		typeBinary =   (int)0x8000 // derived classes only
		};

	enum Attribute {
		normal =    0x00,
		readOnly =  0x01,
		hidden =    0x02,
		system =    0x04,
		volume =    0x08,
		directory = 0x10,
		archive =   0x20
		};

	enum SeekPosition { begin = 0x0, current = 0x1, end = 0x2 };

	enum { hFileNull = -1 };

	bool isValid ()
	{
		if ( !invalidFile() )
			return true;
		else
			return false;
	}


	// show error
	void	showLastError	();
	bool	invalidFile		();

	// open methods
	bool	create			( string & fileName );
	bool	open			( string & fileName );
	bool	openFastRead	( string & fileName );

	bool	openAlways		( string & fileName );
	bool	open			( string & fileName, DWORD openFlags );
	void	close			();
	bool	create			();
	void	release			();

	// file info/position methods
	bool	setPosition		( DWORD position );
	long	size			();
	bool	offsetPosition	( DWORD offset );

	// clear the file
	void	clear			();

	// read/write methods
	bool	write			( LPTSTR pBuffer, DWORD noToWrite );
	bool	read			( LPTSTR pBuffer, DWORD noToRead );

	// file status
		   bool getStatus	( TextFileStatus& rStatus );
	static bool getStatus	( LPCTSTR lpszFileName, TextFileStatus& rStatus );
	static bool setStatus	( LPCTSTR lpszFileName, TextFileStatus& status );

	// file manipulation
	static bool rename		( LPCTSTR lpszOldName, LPCTSTR lpszNewName );
	static bool remove		( LPCTSTR lpszFileName );

	// file path
	static bool getFullPath		( LPTSTR lpszPathOut, LPCTSTR lpszFileIn );
	static bool getRoot			( LPCTSTR lpszPath, string & strRoot );
	static bool comparePath		( LPCTSTR lpszPath1, LPCTSTR lpszPath2 );
	static long getFileTitle	( LPCTSTR lpszPathName, LPTSTR lpszTitle, UINT nMax );

	// operators
	TextFile &  operator << ( string & buffer )
	{
		write( (LPTSTR) buffer.c_str(), buffer.size() );
		return *this;
	}

	TextFile &  operator >> ( stringstream & strm )
	{
		write( (LPTSTR) strm.str().c_str(), strm.str().size() );
		return *this;
	}

	TextFile &  operator << ( LPCTSTR buffer )
	{
		write( (LPTSTR) buffer, lstrlen(buffer) );
		return *this;
	}

	TextFile &  operator << ( LPTSTR buffer )
	{
		write( buffer, lstrlen(buffer) );
		return *this;
	}

	operator HANDLE ()
	{
		return _hFile;
	}

	// test if file open
	bool isOpen ()
	{
		if ( _hFile == 0 || _hFile == INVALID_HANDLE_VALUE )
			return false;
		else
			return true;
	}
};

void operator << ( stringstream & strm, TextFile & file );
void operator << ( TextFile & file, stringstream & strm );


inline bool isDirSep(TCHAR ch)
{
	return (ch == '\\' || ch == '/');
}

inline
bool TextFile::getStatus ( TextFileStatus& rStatus )
{
	return TextFile::getStatus( _name.c_str(), rStatus );
}



#endif