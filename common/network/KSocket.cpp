/****************************************************************************
 *
 *  Copyright (C) 2001-2006 Kevin(fuwenke@gmail.com), Inc. All rights reserved.
 *
 *  This program is free software.  It may be distributed under the terms
 *  in the file LICENSE, found in the top level of the source distribution.
 *
 */

#ifndef WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#endif

#include "../misc/KString.h"
#include "../define/kwin32types.h"
#include "../misc/KDebug.h"
#include "KSocket.h"
#include "../thread/KThread.h"

#ifndef WIN32
#include <sys/ioctl.h>
#define SOCK_LAST_ERROR() errno
//#define SOCKET_ERROR -1
#define closesocket close
#define ioctlsocket ioctl
typedef int sioctl_t;
#endif
#if defined(_WIN32)
#define SOCK_LAST_ERROR() ::WSAGetLastError()
typedef unsigned long sioctl_t;
#pragma warning(disable:4267)
#endif

#if defined(_WIN32) || defined(_SOLARIS)
int inet_aton( const char* cp, struct in_addr* inp )
{
    UINT32 addr = inet_addr( cp );
    if( addr == INADDR_NONE ) return 0;
    inp->s_addr = addr;
    return 1;
}
#endif

#ifndef WIN32
static void socket_nonblock( sockobj_t sock )
{
    int tmp;
    fcntl( sock, F_GETFL, &tmp );
    tmp |= O_NONBLOCK;
    fcntl( sock, F_SETFL, &tmp );
}
#endif
#if defined(_WIN32)
static void socket_nonblock( sockobj_t sock )
{
    sioctl_t tmp = 1;
    ioctlsocket( sock, FIONBIO, &tmp );
}
#endif

static void socket_reuseaddr( sockobj_t sock )
{
    int tmp = 1;
    setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&tmp, sizeof(tmp) );
}

/**************************************
 *
 * CKInetAddr
 *
 **************************************/

CKInetAddr::CKInetAddr( void )
{
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = INADDR_NONE;
    m_addr.sin_port = 0;
}

CKInetAddr::CKInetAddr( const in_addr& host )
{
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr = host;
    m_addr.sin_port = 0;
}

CKInetAddr::CKInetAddr( CPCHAR szHost )
{
    SetHost( szHost );
}

bool CKInetAddr::IsValid( void ) const
{
    return ( m_addr.sin_addr.s_addr != INADDR_NONE );
}

in_addr CKInetAddr::GetHost( void ) const
{
    return m_addr.sin_addr;
}

void CKInetAddr::SetHost( const in_addr& host )
{
    m_addr.sin_addr = host;
}

void CKInetAddr::SetHost( CPCHAR szHost )
{
    m_addr.sin_addr.s_addr = INADDR_NONE;

    Resolve( szHost, &m_addr.sin_addr );
}

CKInetAddr CKInetAddr::Any( void )
{
    CKInetAddr addr;
    addr.m_addr.sin_addr.s_addr = INADDR_ANY;
    return addr;
}

CKInetAddr CKInetAddr::None( void )
{
    CKInetAddr addr;
    addr.m_addr.sin_addr.s_addr = INADDR_NONE;
    return addr;
}

bool CKInetAddr::Resolve( CPCHAR szHost, in_addr* phost )
{
    bool bRet = false;
    bool bIsName = false;

    if( NULL == szHost ) return false;

    // Determine if this is a number or name
    CPCHAR p = szHost;
    while( *p )
    {
        if( (*p < '0' || *p > '9') && *p != '.' )
        {
            bIsName = true;
            break;
        }
        p++;
    }

    if( bIsName )
    {
        hostent* phent;
        phent = gethostbyname( szHost );
        if( phent )
        {
            memcpy( phost, phent->h_addr, sizeof(in_addr) );
            bRet = true;
        }
    }
    else
    {
        bRet = ( 0 != inet_aton( szHost, phost ) );
    }

    return bRet;
}

/**************************************
 *
 * CKSockAddr
 *
 **************************************/

CKSockAddr::CKSockAddr( void ) :
    CKInetAddr()
{
    // Empty
}

CKSockAddr::CKSockAddr( const sockaddr_in& addr ) :
    CKInetAddr()
{
    SetAddr( addr );
}

CKSockAddr::CKSockAddr( const in_addr& host, UINT16 port /* = 0 */ ) :
    CKInetAddr(host)
{
    SetAddr( host, port );
}

CKSockAddr::CKSockAddr( CPCHAR szHost, UINT16 port /* = 0 */ ) :
    CKInetAddr()
{
    SetAddr( szHost, port );
}

sockaddr_in CKSockAddr::GetAddr( void ) const
{
    return m_addr;
}

void CKSockAddr::SetAddr( const sockaddr_in& addr )
{
    m_addr = addr;
}

void CKSockAddr::SetAddr( const in_addr& host, UINT16 port )
{
    SetHost( host );
    m_addr.sin_port = htons( port );
}

void CKSockAddr::SetAddr( CPCHAR szHost, UINT16 port )
{
    SetHost( szHost );
    m_addr.sin_port = htons( port );
}

UINT16 CKSockAddr::GetPort( void ) const
{
    return ntohs( m_addr.sin_port );
}

void CKSockAddr::SetPort( UINT16 port )
{
    m_addr.sin_port = htons( port );
}

CKSockAddr CKSockAddr::Any( void )
{
    CKSockAddr addr;
    addr.m_addr.sin_addr.s_addr = INADDR_ANY;
    addr.m_addr.sin_port = 0;
    return addr;
}

CKSockAddr CKSockAddr::None( void )
{
    CKSockAddr addr;
    addr.m_addr.sin_addr.s_addr = INADDR_NONE;
    addr.m_addr.sin_port = 0xFFFF;
    return addr;
}

/**************************************
 *
 * CKSocket
 *
 **************************************/

CKSocket::CKSocket( void ) :
    CKStream(),
    m_sock(INVALID_SOCKET),
    m_uSelectFlags(SF_NONE),
    m_err(SOCKERR_NONE)
{
    // Empty
}

CKSocket::CKSocket( CKStreamNotify* pNotify ) :
    CKStream(pNotify),
    m_sock(INVALID_SOCKET),
    m_uSelectFlags(SF_NONE),
    m_err(SOCKERR_NONE)
{
    // Empty
}

CKSocket::~CKSocket( void )
{
    Close();
}

bool CKSocket::IsOpen( void )
{
    return ( INVALID_SOCKET != m_sock );
}

void CKSocket::Close( void )
{
    if( IsOpen() )
    {
        Select( SF_NONE );
        closesocket( m_sock );
        m_sock = INVALID_SOCKET;
        if( m_pNotify ) m_pNotify->OnClosed();
    }
}

size_t CKSocket::Read( PVOID pbuf, size_t nLen )
{
    assert_or_retv( SOCKERR_EOF, (pbuf != NULL && IsOpen()) );

    m_err = SOCKERR_NONE;
    ssize_t n = recv( m_sock, (char*)pbuf, nLen, 0 );
    // For TCP sockets...
    // If recv() returns zero, the remote end closed gracefully
    // If we get EPIPE/WSAECONNRESET, the remote end has closed, but there
    // may be more data left to read
    if( n == 0 )
    {
        n = SOCKERR_EOF;
    }
    else if( n == SOCKET_ERROR )
    {
        n = 0;
        m_err = SOCK_LAST_ERROR();
        if( m_err != SOCKERR_WOULDBLOCK )
        {
            n = SOCKERR_EOF;
        }
    }

    return n;
}

size_t CKSocket::Write( CPVOID pbuf, size_t nLen )
{
    assert_or_retv( 0, (pbuf != NULL && IsOpen()) );

    m_err = SOCKERR_NONE;
    ssize_t n = send( m_sock, (const char*)pbuf, nLen, 0 );
    if( n == SOCKET_ERROR )
    {
        n = 0;
        m_err = SOCK_LAST_ERROR();
        if( m_err != SOCKERR_WOULDBLOCK )
        {
            n = SOCKERR_EOF;
        }
    }

    return n;
}

CKSockAddr CKSocket::GetLocalAddr( void )
{
    CKSockAddr   addr;
    sockaddr_in sa;
    socklen_t   salen;

    salen = sizeof(sa);
    if( getsockname( m_sock, (sockaddr*)&sa, &salen ) == 0 )
    {
        addr.SetAddr( sa );
    }

    return addr;
}

CKSockAddr CKSocket::GetPeerAddr( void )
{
    CKSockAddr   addr;
    sockaddr_in sa;
    socklen_t   salen;

    salen = sizeof(sa);
    if( getpeername( m_sock, (sockaddr*)&sa, &salen ) == 0 )
    {
        addr.SetAddr( sa );
    }

    return addr;
}

bool CKSocket::Select( UINT32 nWhich )
{
    assert( IsOpen() || SF_NONE == nWhich );
    assert( m_pNotify || SF_NONE == nWhich || SF_ACCEPT == nWhich );

    if( nWhich != m_uSelectFlags )
    {
		CKThread* pThread = CKThread::This();

		CKEventThread* pSelf = ( CKEventThread* )pThread;

        assert_or_retv( false, pSelf );

        if( SF_NONE == m_uSelectFlags )
        {
            if( !pSelf->AddStream( this ) ) return false;
        }
        m_uSelectFlags = nWhich;
        pSelf->SetStreamSelect( this, nWhich );
        if( SF_NONE == nWhich )
        {
            pSelf->DelStream( this );
        }
    }
    return true;
}

sockerr_t CKSocket::LastError( void )
{
    return m_err;
}

unsigned long CKSocket::Size()
{
	assert_or_retv( 0, IsOpen() );

	unsigned long n  = 0;

	if ( ioctlsocket( m_sock, FIONREAD, &n) != 0 )
	{
		m_err = SOCK_LAST_ERROR();
		if( m_err != SOCKERR_WOULDBLOCK )
		{
			n = 0;
		}
	}

	return n;
}

/**************************************
 *
 * CKListenSocket
 *
 **************************************/

CKListenSocket::CKListenSocket( void ) :
    CKSocket(),
    m_pAcceptNotify(NULL)
{
    // Empty
}

CKListenSocket::CKListenSocket( CKListenSocketNotify* pNotify ) :
    CKSocket(),
    m_pAcceptNotify(pNotify)
{
    // Empty
}

CKListenSocket::~CKListenSocket( void )
{
    // Empty
}

void CKListenSocket::SetNotify( CKListenSocketNotify* pNotify )
{
    assert( pNotify || !IsOpen() );

    m_pAcceptNotify = pNotify;
}

bool CKListenSocket::Listen( const CKSockAddr& addr )
{
    assert_or_retv( false, (m_pAcceptNotify && !IsOpen()) );

    m_err = SOCKERR_NONE;
    m_sock = socket( AF_INET, SOCK_STREAM, IPPROTO_IP );
    if( m_sock == INVALID_SOCKET )
    {
        m_err = SOCK_LAST_ERROR();
        return false;
    }

    socket_nonblock( m_sock );
    socket_reuseaddr( m_sock );

    sockaddr_in bindaddr = addr.GetAddr();
    if( 0 != bind( m_sock, (sockaddr*)&bindaddr, sizeof(bindaddr) ) )
    {
        m_err = SOCK_LAST_ERROR();
        closesocket( m_sock );
        m_sock = INVALID_SOCKET;
        return false;
    }

    //XXX: Is there a performance penalty for SOMAXCONN?
    if( 0 != listen( m_sock, SOMAXCONN ) )
    {
        m_err = SOCK_LAST_ERROR();
        closesocket( m_sock );
        m_sock = INVALID_SOCKET;
        return false;
    }

    Select( SF_ACCEPT );

    return true;
}

/**************************************
 *
 * CKTcpSocket
 *
 **************************************/

CKTcpSocket::CKTcpSocket( void ) :
    CKSocket()
{
    // Empty
}

CKTcpSocket::CKTcpSocket( CKStreamNotify* pNotify ) :
    CKSocket(pNotify)
{
    // Empty
}

CKTcpSocket::~CKTcpSocket( void )
{
    // Empty
}

bool CKTcpSocket::Connect( const CKSockAddr& addr )
{
    assert_or_retv( false, (m_pNotify && !IsOpen()) );

    m_err = SOCKERR_NONE;
    m_sock = socket( AF_INET, SOCK_STREAM, IPPROTO_IP );
    if( m_sock == INVALID_SOCKET )
    {
        m_err = SOCK_LAST_ERROR();
        return false;
    }

    socket_nonblock( m_sock );
    socket_reuseaddr( m_sock );

    sockaddr_in cnxaddr = addr.GetAddr();
    if( 0 == connect( m_sock, (sockaddr*)&cnxaddr, sizeof(cnxaddr) ) )
    {
        m_pNotify->OnConnectDone( m_err );
        return true;
    }

    int cnxerr = SOCK_LAST_ERROR();
    if( cnxerr != SOCKERR_INPROGRESS && cnxerr != SOCKERR_WOULDBLOCK )
    {
        m_err = cnxerr;
        closesocket( m_sock );
        m_sock = INVALID_SOCKET;
        return false;
    }

    Select( SF_CONNECT );

    return true;
}

/**************************************
 *
 * CKUdpSocket
 *
 **************************************/

CKUdpSocket::CKUdpSocket( void ) :
    CKSocket()
{
    // Empty
}

CKUdpSocket::CKUdpSocket( CKStreamNotify* pNotify ) :
    CKSocket(pNotify)
{
    // Empty
}

CKUdpSocket::~CKUdpSocket( void )
{
    // Empty
}

bool CKUdpSocket::Bind( const CKSockAddr& addr )
{
    assert_or_retv( false, !IsOpen() );

    m_err = SOCKERR_NONE;
    m_sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_IP );
    if( m_sock == INVALID_SOCKET )
    {
        m_err = SOCK_LAST_ERROR();
        return false;
    }

    //socket_nonblock( m_sock );

    sockaddr_in bindaddr = addr.GetAddr();
    if( 0 != bind( m_sock, (sockaddr*)&bindaddr, sizeof(bindaddr) ) )
    {
        m_err = SOCK_LAST_ERROR();
        closesocket( m_sock );
        m_sock = INVALID_SOCKET;
        return false;
    }

    return true;
}

bool CKUdpSocket::Connect( const CKSockAddr& addr )
{
    assert_or_retv( false, IsOpen() );

    m_err = SOCKERR_NONE;
    sockaddr_in cnxaddr = addr.GetAddr();
    if( 0 != connect( m_sock, (sockaddr*)&cnxaddr, sizeof(cnxaddr) ) )
    {
        m_err = SOCK_LAST_ERROR();
        return false;
    }

    return true;
}

size_t CKUdpSocket::RecvFrom( CKSockAddr* paddr, PVOID pbuf, size_t nLen )
{
    assert_or_retv( 0, (paddr != NULL && pbuf != NULL && IsOpen()) );

    m_err = SOCKERR_NONE;
    sockaddr_in recvaddr;
    socklen_t salen = sizeof(recvaddr);
    ssize_t n = recvfrom( m_sock, (char*)pbuf, nLen, 0, (sockaddr*)&recvaddr, &salen );
    if( n > 0 )
    {
        paddr->SetAddr( recvaddr );
    }
    else if( n == 0 )
    {
        dbgout( "*** recvfrom() returned zero ***" );
        n = SOCKERR_EOF;
    }
    else if( n == SOCKET_ERROR )
    {
        n = 0;
        m_err = SOCK_LAST_ERROR();
        if( m_err != SOCKERR_WOULDBLOCK )
        {
            n = SOCKERR_EOF;
        }
    }

    return n;
}

size_t CKUdpSocket::RecvFrom( CKSockAddr* paddr, PVOID pbuf, size_t nLen, size_t nWaitSec, size_t nWaitUsec )
{
	fd_set rfds;
	struct timeval tv;

	int fd_max = m_sock;
	FD_ZERO(&rfds);
	FD_SET(m_sock, &rfds);
	tv.tv_sec = nWaitSec;
	tv.tv_usec = nWaitUsec;
	int nRet = select(fd_max + 1, &rfds, NULL, NULL, &tv);
	if (nRet > 0 && FD_ISSET(m_sock, &rfds)) {

		return RecvFrom( paddr, pbuf, nLen );
	}

	return 0;
}

size_t CKUdpSocket::SendTo( const CKSockAddr& addr, CPVOID pbuf, size_t nLen )
{
    assert_or_retv( 0, (pbuf != NULL && IsOpen()) );

    m_err = SOCKERR_NONE;
    sockaddr_in sendaddr = addr.GetAddr();
    ssize_t n = sendto( m_sock, (const char*)pbuf, nLen, 0, (sockaddr*)&sendaddr, sizeof(sendaddr) );
    if( n == SOCKET_ERROR )
    {
        n = 0;
        m_err = SOCK_LAST_ERROR();
        if( m_err != SOCKERR_WOULDBLOCK )
        {
            n = SOCKERR_EOF;
        }
    }

    return n;
}


