/****************************************************************************
 *
 *  Copyright (C) 2001-2006 Kevin(fuwenke@gmail.com), Inc. All rights reserved.
 *
 *  This program is free software.  It may be distributed under the terms
 *  in the file LICENSE, found in the top level of the source distribution.
 *
 */

#include <ctype.h>
#include <string.h>

#include "../define/kwin32types.h"
#include "../misc/KString.h"
#include "../misc/KDebug.h"
#include "../misc/KPtrList.h"

/**************************************
 *
 * CKBuffer
 *
 **************************************/

CKBuffer::CKBuffer( void ) :
    m_nAlloc(0),
    m_nLen(0),
    m_buf(NULL)
{
    // Empty
}

CKBuffer::CKBuffer( const CKBuffer& other ) :
    m_nAlloc(0),
    m_nLen(0),
    m_buf(NULL)
{
    assert( other.m_buf || (!other.m_nAlloc && !other.m_nLen) );

    if( other.m_nLen )
    {
        m_nAlloc = m_nLen = other.m_nLen;
        m_buf = new BYTE[ other.m_nLen ];
        memcpy( m_buf, other.m_buf, m_nAlloc );
    }
}

CKBuffer::CKBuffer( size_t len )
{
    m_buf = new BYTE[ len ];
    m_nAlloc = m_nLen = len;
}

CKBuffer::CKBuffer( CPBYTE pbuf, size_t len )
{
    assert( pbuf );

    m_buf = new BYTE[ len ];
    m_nAlloc = m_nLen = len;
    memcpy( m_buf, pbuf, len );
}

CKBuffer::~CKBuffer( void )
{
    if( m_buf ) memset( m_buf, 0xDD, m_nAlloc );
    delete[] m_buf;
}

CKBuffer& CKBuffer::operator=( const CKBuffer& other )
{
    assert( other.m_buf || (!other.m_nAlloc && !other.m_nLen) );

    m_nAlloc = m_nLen = 0;
    delete[] m_buf; m_buf = NULL;
    if( other.m_nLen )
    {
        m_nAlloc = m_nLen = other.m_nLen;
        m_buf = new BYTE[ other.m_nLen ];
        memcpy( m_buf, other.m_buf, m_nAlloc );
    }

    return *this;
}

BYTE CKBuffer::operator[]( size_t n ) const
{
    assert( m_buf );
    assert( n < m_nLen );

    return m_buf[n];
}

BYTE& CKBuffer::operator[]( size_t n )
{
    assert( m_buf );
    assert( n < m_nLen );

    return m_buf[n];
}

void CKBuffer::Clear( void )
{
    m_nLen = m_nAlloc = 0;
    delete[] m_buf; m_buf = NULL;
}

void CKBuffer::Set( CPBYTE pbuf, size_t len )
{
    assert( pbuf || !len );
    assert( m_buf || (!m_nAlloc && !m_nLen) );

    if( len )
    {
        if( len > m_nAlloc )
        {
            delete[] m_buf;
            m_buf = new BYTE[ len ];
            m_nAlloc = len;
        }
        memcpy( m_buf, pbuf, len );
        m_nLen = len;
    }
    else
    {
        m_nAlloc = m_nLen = 0;
        delete[] m_buf; m_buf = NULL;
    }
}

size_t CKBuffer::GetSize( void ) const
{
    return m_nLen;
}

void CKBuffer::SetSize( size_t len )
{
    assert( m_buf || (!m_nAlloc && !m_nLen) );

    if( len )
    {
        if( len > m_nAlloc )
        {
            BYTE* pbuf = new BYTE[ len ];
            memcpy( pbuf, m_buf, m_nAlloc );
            delete[] m_buf;
            m_nAlloc = len;
            m_buf = pbuf;
        }
        m_nLen = len;
    }
    else
    {
        m_nAlloc = m_nLen = 0;
        delete[] m_buf; m_buf = NULL;
    }
}

PBYTE CKBuffer::GetBuffer( void )
{
    return m_buf;
}

CPBYTE CKBuffer::GetBuffer( void ) const
{
    return m_buf;
}

void CKBuffer::SetBuffer( CPBYTE pbuf )
{
    assert( pbuf );
    assert( m_buf && m_nAlloc && m_nLen );

    memcpy( m_buf, pbuf, m_nLen );
}

/**************************************
 *
 * CKString
 *
 **************************************/

CKString::CKString( void )
{
    m_sz = new char[1];
    m_sz[0] = '\0';
}

CKString::CKString( const CKString& other )
{
    assert( other.m_sz );
    m_sz = new char[ strlen(other.m_sz) + 1 ];
    strcpy( m_sz, other.m_sz );
}

CKString::CKString( CPCHAR sz )
{
    m_sz = new char[ strlen(sz) + 1 ];
    strcpy( m_sz, sz );
}

CKString::CKString( CPCHAR buf, size_t len )
{
    m_sz = new char[ len + 1 ];
    memcpy( m_sz, buf, len );
    m_sz[len] = '\0';
}

CKString::CKString( char c, UINT nrep /* = 1 */ )
{
    m_sz = new char[ nrep + 1 ];
    memset( m_sz, c, nrep );
    m_sz[nrep] = '\0';
}

CKString::~CKString( void )
{
    if( m_sz ) memset( m_sz, 0xDD, strlen(m_sz)+1 );
    delete[] m_sz;
}

CKString& CKString::operator=( const CKString& other )
{
    assert( other.m_sz );
    delete[] m_sz;
    m_sz = new char[ strlen(other.m_sz) + 1 ];
    strcpy( m_sz, other.m_sz );

    return *this;
}

CKString& CKString::operator=( CPCHAR sz )
{
    assert( sz );

    delete[] m_sz;
    m_sz = new char[ strlen(sz) + 1 ];
    strcpy( m_sz, sz );

    return *this;
}

void CKString::Set( CPCHAR buf, size_t len )
{
    assert( buf );

    delete[] m_sz;
    m_sz = new char[ len + 1 ];
    memcpy( m_sz, buf, len );
    m_sz[len] = '\0';
}

UINT CKString::GetLength( void ) const
{
    assert( m_sz );

    return strlen( m_sz );
}

bool CKString::IsEmpty( void ) const
{
    assert( m_sz );

    return ( m_sz[0] == '\0' );
}

int CKString::Compare( const CKString& other ) const
{
    assert( m_sz && other.m_sz );

    return strcmp( m_sz, other.m_sz );
}

int CKString::CompareNoCase( const CKString& other ) const
{
    assert( m_sz && other.m_sz );

    return strcasecmp( m_sz, other.m_sz );
}

char CKString::GetAt( UINT pos ) const
{
    assert( m_sz );
    assert( pos < strlen(m_sz) );

    return m_sz[pos];
}

char& CKString::GetAt( UINT pos )
{
    assert( m_sz );
    assert( pos < strlen(m_sz) );

    return m_sz[pos];
}

void CKString::SetAt( UINT pos, char c )
{
    assert( m_sz );
    assert( pos < strlen(m_sz) );

    m_sz[pos] = c;
}

void CKString::Append( CPCHAR sz )
{
    assert( sz && m_sz );

    if( *sz )
    {
        PCHAR sznew = new char[ strlen(m_sz) + strlen(sz) + 1 ];
        strcpy( sznew, m_sz );
        strcat( sznew, sz );
        delete[] m_sz;
        m_sz = sznew;
    }
}

void CKString::ToLower( void )
{
    assert_or_ret( m_sz );
    char* p = m_sz;
    while( *p )
    {
        *p = tolower( *p );
        p++;
    }
}

void CKString::ToUpper( void )
{
    assert_or_ret( m_sz );
    char* p = m_sz;
    while( *p )
    {
        *p = toupper( *p );
        p++;
    }
}

void CKString::DeQuote( void )
{
    assert_or_ret( m_sz );

    PCHAR p = m_sz;
    PCHAR q = m_sz;
    while( *p )
    {
        if( *p == '\\' && *(p+1) )
        {
            switch( *(p+1) )
            {
            case 'n':   p++; *p = '\n'; break;
            case 'r':   p++; *p = '\r'; break;
            case 't':   p++; *p = '\t'; break;
            case '"':   p++; *p = '"';  break;
            case '\\':  p++;            break;
            default:
                break;
            }
        }
        *q++ = *p++;
    }
    *q = '\0';
}

CPCHAR CKString::Find( char c, UINT pos /* = 0 */ ) const
{
    assert_or_retv( NULL, m_sz );
    assert_or_retv( NULL, pos < strlen(m_sz) );

    return strchr( m_sz+pos, c );
}
