/****************************************************************************
 *
 *  Copyright (C) 2001-2006 Kevin(fuwenke@gmail.com), Inc. All rights reserved.
 *
 *  This program is free software.  It may be distributed under the terms
 *  in the file LICENSE, found in the top level of the source distribution.
 *
 */

#include "KFile.h"

#include <fcntl.h>

#include "../misc/KDebug.h"

CKFile::CKFile( void ) :
    CKStream(),
    m_file(INVALID_FILE)
{
    // Empty
}

CKFile::CKFile( CKStreamNotify* pNotify ) :
    CKStream(pNotify),
    m_file(INVALID_FILE)
{
    // Empty
}

CKFile::~CKFile( void )
{
    if( IsOpen() ) Close();
}

bool CKFile::IsOpen( void )
{
    return ( INVALID_FILE != m_file );
}

void CKFile::Close( void )
{
    assert( IsOpen() );

#ifdef _UNIX
    close( m_file );
#endif
#ifdef _WIN32
    ::CloseHandle( m_file );
#endif

    m_file = INVALID_FILE;
    if( m_pNotify ) m_pNotify->OnClosed();
}

bool CKFile::Create( const CKString& strFile )
{
	assert( !IsOpen() );

#ifdef _UNIX
	m_file = open( strFile, O_NONBLOCK );
#endif
#ifdef _WIN32
	m_file = ::CreateFile( strFile, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL );
#endif

	return IsOpen();
}

bool CKFile::Open( const CKString& strFile )
{
	assert( !IsOpen() );

#ifdef _UNIX
	m_file = open( strFile, O_NONBLOCK );
#endif
#ifdef _WIN32
	m_file = ::CreateFile( strFile, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
#endif

	return IsOpen();
}

bool CKFile::Stat( struct stat* pst )
{
    assert( IsOpen() );

    //XXXFIXME: this does NOT work for Win32 but it will compile
    return ( 0 == fstat( (int)m_file, pst ) );
}

size_t CKFile::Read( PVOID pbuf, size_t len )
{
    assert( pbuf );
    assert( IsOpen() );

#ifdef _UNIX
    ssize_t nRead = read( m_file, pbuf, len );
    if( nRead < 0 )
    {
        Close();
        nRead = 0;
    }
#endif
#ifdef _WIN32
    DWORD nRead = 0;
    if( ! ::ReadFile( m_file, pbuf, len, &nRead, NULL ) )
    {
        Close();
        nRead = 0;
    }
#endif

    return nRead;
}

size_t CKFile::Write( CPVOID pbuf, size_t len )
{
    assert( pbuf );
    assert( IsOpen() );

#ifdef _UNIX
    ssize_t nWritten = write( m_file, pbuf, len );
    if( nWritten < 0 )
    {
        Close();
        nWritten = 0;
    }
#endif
#ifdef _WIN32
    DWORD nWritten = 0;
    if( ! ::WriteFile( m_file, pbuf, len, &nWritten, NULL ) )
    {
        Close();
        nWritten = 0;
    }
#endif

    return nWritten;
}

bool CKFile::Seek( size_t pos, unsigned long ulFlag )
{
	assert( IsOpen() );

#ifdef _UNIX
	return false;
#endif

#ifdef _WIN32
	LARGE_INTEGER uliSize;
	uliSize.QuadPart = pos;
	return SetFilePointerEx( m_file, uliSize, 0, FILE_BEGIN);
#endif
}

LONGLONG CKFile::Size()
{
	assert( IsOpen() );
#ifdef _WIN32
	LARGE_INTEGER uliSize;
	if ( GetFileSizeEx(m_file, &uliSize) )
		return uliSize.QuadPart;
	else
		return 0;
#else
	return ( 0 == fstat( (int)m_file, pst ) );
#endif
}

bool CKFile::Stat( const CKString& strFile, struct stat* pst )
{
	return ( 0 == stat( strFile, pst ) );
}

bool CKFile::DeleteFile( const CKString& strFile )
{
#ifdef _WIN32
	return ::DeleteFile( strFile );
#else
	return false;
#endif
}
