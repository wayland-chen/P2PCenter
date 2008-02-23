// TextFile.cpp: implementation of the TextFile class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "TextFile.h"

#include <tchar.h>
#include <commdlg.h>

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

void operator << ( stringstream & strm, TextFile & file )
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

void operator << ( TextFile & file, stringstream & strm )
{
	// get buffer
	string & str  = strm.str();
	char * buffer = (char *) str.c_str();
	if ( !buffer )
		return;


	// write buffer to file
	long sz = str.size();
	file.write(buffer,sz);
}


/////////////////////////////////////////////////////////////
// TextFile



TextFile::TextFile () :
	Lock(),
	_hFile(0),						// handle to file
	_openFlags(0),					// flags used to open file
	_name("")						// name of file
{}

TextFile::~TextFile () 
{
	release();
}

bool TextFile::create ()
{
	// if already open then close it
	if ( isOpen() )
		close();

	// init
	_hFile = 0;
	_openFlags = 0;
	_name = "";

	return true;
}

void TextFile::release ()
{
	// release lock
	Lock::release();

	// close file
	close();

	// release string
	_name.erase();
}


void TextFile::showLastError ()
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



bool TextFile::invalidFile ()
{
	if ( _hFile == INVALID_HANDLE_VALUE )
		return true;
	else
		return false;
}

bool TextFile::create ( string & fileName )
{
	DWORD openFlags = CREATE_ALWAYS;
	return open(fileName,openFlags);
}

bool TextFile::open ( string & fileName )
{
	DWORD openFlags = OPEN_EXISTING;
	return open(fileName,openFlags);
}

bool TextFile::openFastRead ( string & fileName )
{
	if ( !lock() )
		return false;

	// open the file
	_hFile = 
	CreateFile( fileName.c_str(),
			    GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
			    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
				NULL ); 

	unlock();

	if ( _hFile == INVALID_HANDLE_VALUE )
	{
		showLastError();
		return false;
	}

	// store file info
	_openFlags = OPEN_EXISTING;
	_name = fileName;

	return true;
}

bool TextFile::openAlways ( string & fileName )
{
	DWORD openFlags = OPEN_ALWAYS;
	return open(fileName,openFlags);
}

bool TextFile::open ( string & fileName, DWORD openFlags )
{
	if ( !lock() )
		return false;

	// open the file
	_hFile = 
	CreateFile( fileName.c_str(),
				GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				openFlags,
				FILE_ATTRIBUTE_NORMAL, 0 );

	unlock();

	if ( _hFile == INVALID_HANDLE_VALUE )
	{
		showLastError();
		return false;
	}

	// store file info
	_openFlags = openFlags;
	_name = fileName;

	return true;
}

void TextFile::close ()
{
	if ( !lock() )
		return;

	if ( _hFile == INVALID_HANDLE_VALUE )
		return;

	::CloseHandle(_hFile);
	_hFile = INVALID_HANDLE_VALUE;

	unlock();
}

bool TextFile::setPosition ( DWORD position )
{
	if ( !lock() )
		return false;

	if ( _hFile == INVALID_HANDLE_VALUE )
		return false;

	LONG moveHigh = 0;

	DWORD dwPos = 
	::SetFilePointer( _hFile, position, &moveHigh, FILE_BEGIN );

	if ( moveHigh == 0xffffffff )
		return false;

	unlock();
	return true;
}

long TextFile::size ()
{
	if ( !lock() )
		return 0;

	if ( _hFile == INVALID_HANDLE_VALUE )
		return 0;

	DWORD dwSize =
	::GetFileSize(_hFile, NULL);

	unlock();
	return dwSize;
}

void TextFile::clear ()
{
	if ( !lock() )
		return;

	if ( _hFile == INVALID_HANDLE_VALUE )
		return;

	::CloseHandle(_hFile);
	DeleteFile( _name.c_str() );

	bool success = 
	open(_name,_openFlags);

	unlock();
}



bool TextFile::offsetPosition ( DWORD offset )
{
	if ( !lock() )
		return false;

	if ( _hFile == INVALID_HANDLE_VALUE )
		return false;

	LONG moveHigh = 0;

	DWORD dwPos = 
	::SetFilePointer( _hFile, offset, &moveHigh, FILE_CURRENT );

	if ( moveHigh == 0xffffffff )
		return false;

	unlock();
	return true;
}

bool TextFile::write ( LPTSTR pBuffer, DWORD noToWrite )
{
	if ( !lock() )
	return false;


	bool success = false;
	try
	{
		DWORD noWritten = 0;
		if ( WriteFile(_hFile,(LPVOID)pBuffer,noToWrite,&noWritten,NULL) != 0 )
			success = true;
	}
	catch(...)
	{}

	unlock();

	return success;
}

bool TextFile::read ( LPTSTR pBuffer, DWORD noToRead )
{
	if ( !lock() )
		return false;

	bool success = false;
	try
	{
		DWORD noRead;
		if ( ReadFile(_hFile,(LPVOID)pBuffer,noToRead,&noRead,NULL) != 0 )
			success = true;
	}
	catch(...)
	{}

	unlock();

	return success;
}



bool TextFile::getStatus(LPCTSTR lpszFileName, TextFileStatus& rStatus)
{
	// attempt to fully qualify path first
	if (!getFullPath(rStatus._szFullName, lpszFileName))
	{
		rStatus._szFullName[0] = '\0';
		return false;
	}

	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile((LPTSTR)lpszFileName, &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return false;
	FindClose(hFind);

	// strip attribute of NORMAL bit, our API doesn't have a "normal" bit.
	rStatus._attribute = (BYTE)
		(findFileData.dwFileAttributes & ~FILE_ATTRIBUTE_NORMAL);

	// get just the low DWORD of the file size
	_ASSERT(findFileData.nFileSizeHigh == 0);
	rStatus._size = (LONG)findFileData.nFileSizeLow;

	// convert times as appropriate
	rStatus._ctime = TimeStamp(findFileData.ftCreationTime);
	rStatus._atime = TimeStamp(findFileData.ftLastAccessTime);
	rStatus._mtime = TimeStamp(findFileData.ftLastWriteTime);

	if (rStatus._ctime.empty() == 0)
		rStatus._ctime = rStatus._mtime;

	if (rStatus._atime.empty() == 0)
		rStatus._atime = rStatus._mtime;

	return TRUE;
}

bool TimeToFileTime(TimeStamp& time, LPFILETIME pFileTime)
{
	SYSTEMTIME sysTime;
	time.getTime(sysTime);

	// convert system time to local file time
	FILETIME localTime;
	if (!SystemTimeToFileTime((LPSYSTEMTIME)&sysTime, &localTime))
		return false;

	// convert local file time to UTC file time
	if (!LocalFileTimeToFileTime(&localTime, pFileTime))
		return false;

	return true;
}

bool TextFile::setStatus ( LPCTSTR lpszFileName, TextFileStatus & status )
{
	DWORD wAttr;
	FILETIME creationTime;
	FILETIME lastAccessTime;
	FILETIME lastWriteTime;
	LPFILETIME lpCreationTime = NULL;
	LPFILETIME lpLastAccessTime = NULL;
	LPFILETIME lpLastWriteTime = NULL;

	if ((wAttr = GetFileAttributes((LPTSTR)lpszFileName)) == (DWORD)-1L)
		return false;

	if ((DWORD)status._attribute != wAttr && (wAttr & FILE_ATTRIBUTE_READONLY))
	{
		// Set file attribute, only if currently readonly.
		// This way we will be able to modify the time assuming the
		// caller changed the file from readonly.

		if (!SetFileAttributes((LPTSTR)lpszFileName, (DWORD)status._attribute))
			return false;
	}

	// last modification time
	if (status._mtime.empty() != 0)
	{
		TimeToFileTime(status._mtime, &lastWriteTime);
		lpLastWriteTime = &lastWriteTime;

		// last access time
		if (status._atime.empty() != 0)
		{
			TimeToFileTime(status._atime, &lastAccessTime);
			lpLastAccessTime = &lastAccessTime;
		}

		// create time
		if (status._ctime.empty() != 0)
		{
			TimeToFileTime(status._ctime, &creationTime);
			lpCreationTime = &creationTime;
		}

		HANDLE hFile = ::CreateFile(lpszFileName, GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
			NULL);

		if (hFile == INVALID_HANDLE_VALUE)
			return false;

		if (!SetFileTime((HANDLE)hFile, lpCreationTime, lpLastAccessTime, lpLastWriteTime))
			return false;

		if (!::CloseHandle(hFile))
			return false;
	}

	if ((DWORD)status._attribute != wAttr && !(wAttr & FILE_ATTRIBUTE_READONLY))
	{
		if (!SetFileAttributes((LPTSTR)lpszFileName, (DWORD)status._attribute))
			return false;
	}

	return true;
}



// turn a file, relative path or other into an absolute path
// lpszPathOut = buffer of _MAX_PATH
// lpszFileIn = file, relative path or absolute path
// (both in ANSI character set)

bool TextFile::getFullPath ( LPTSTR lpszPathOut, LPCTSTR lpszFileIn )
{
	// first, fully qualify the path name
	LPTSTR lpszFilePart;
	if (!GetFullPathName(lpszFileIn, _MAX_PATH, lpszPathOut, &lpszFilePart))
	{
		
		if (lpszFileIn[0] != '\0')
			return false;
		lstrcpyn(lpszPathOut, lpszFileIn, _MAX_PATH); // take it literally
		return false;
	}
	
	// determine the root name of the volume
	string strRoot;
	getRoot(lpszPathOut, strRoot);
	
	// get file system information for the volume
	DWORD dwFlags, dwDummy;
	if ( !GetVolumeInformation(strRoot.c_str(), NULL, 0, NULL, &dwDummy, &dwFlags,
		                       NULL, 0))
	{
		return false;
	}
	
	// not all characters have complete uppercase/lowercase
	if (!(dwFlags & FS_CASE_IS_PRESERVED))
		CharUpper(lpszPathOut);
	
	// assume non-UNICODE file systems, use OEM character set
	if (!(dwFlags & FS_UNICODE_STORED_ON_DISK))
	{
		WIN32_FIND_DATA data;
		HANDLE h = FindFirstFile(lpszFileIn, &data);
		if (h != INVALID_HANDLE_VALUE)
		{
			FindClose(h);
			lstrcpy(lpszFilePart, data.cFileName);
		}
	}
	return TRUE;
}

bool TextFile::getRoot ( LPCTSTR lpszPath, string & strRoot )
{
	if ( lpszPath == NULL )
		return false;

	// determine the root name of the volume
	LPTSTR lpszRoot = (LPTSTR) strRoot.c_str();
	memset(lpszRoot, 0, _MAX_PATH);
	lstrcpyn(lpszRoot, lpszPath, _MAX_PATH);
	LPTSTR lpsz = 0;
	for (lpsz = lpszRoot; *lpsz != '\0'; lpsz = _tcsinc(lpsz))
	{
		// find first double slash and stop
		if ( isDirSep(lpsz[0]) && isDirSep(lpsz[1]) )
			break;
	}

	if ( *lpsz != '\0' )
	{
		// it is a UNC name, find second slash past '\\'
		_ASSERT(isDirSep(lpsz[0]));
		_ASSERT(isDirSep(lpsz[1]));

		lpsz += 2;
		while (*lpsz != '\0' && (!isDirSep(*lpsz)))
			lpsz = _tcsinc(lpsz);
		if (*lpsz != '\0')
			lpsz = _tcsinc(lpsz);
		while (*lpsz != '\0' && (!isDirSep(*lpsz)))
			lpsz = _tcsinc(lpsz);
		// terminate it just after the UNC root (ie. '\\server\share\')
		if (*lpsz != '\0')
			lpsz[1] = '\0';
	}
	else
	{
		// not a UNC, look for just the first slash
		lpsz = lpszRoot;
		while (*lpsz != '\0' && (!isDirSep(*lpsz)))
			lpsz = _tcsinc(lpsz);
		// terminate it just after root (ie. 'x:\')
		if (*lpsz != '\0')
			lpsz[1] = '\0';
	}

	

	return true;
}

bool TextFile::comparePath(LPCTSTR lpszPath1, LPCTSTR lpszPath2)
{
	// use case insensitive compare as a starter
	if (lstrcmpi(lpszPath1, lpszPath2) != 0)
		return false;
	else
		return true;
}

long TextFile::getFileTitle(LPCTSTR lpszPathName, LPTSTR lpszTitle, UINT nMax)
{
	if ( lpszTitle == NULL )
		return 0;
	if ( lpszPathName == NULL )
		return NULL;
	
	// use a temporary to avoid bugs in ::GetFileTitle when lpszTitle is NULL
	TCHAR szTemp[_MAX_PATH];
	LPTSTR lpszTemp = lpszTitle;
	if ( lpszTemp == NULL )
	{
		lpszTemp = szTemp;
		nMax = lstrlen(szTemp);
	}

	if ( ::GetFileTitle(lpszPathName, lpszTemp, (WORD)nMax) != 0 )
		return 0;


	return lpszTitle == NULL ? lstrlen(lpszTemp)+1 : 0;
}



bool TextFile::rename ( LPCTSTR lpszOldName, LPCTSTR lpszNewName )
{
	if ( !::MoveFile((LPTSTR)lpszOldName, (LPTSTR)lpszNewName) )
		return false;
	else
		return true;
}

bool TextFile::remove ( LPCTSTR lpszFileName )
{
	if ( !::DeleteFile((LPTSTR)lpszFileName) )
		return false;
	else
		return true;
}
