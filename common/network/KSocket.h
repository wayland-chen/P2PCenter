/********************************************************************
	created:	2008/01/11
	created:	11:1:2008   15:49
	author:		Kevin
	
	purpose:	

	email:		kan8888.com@163.com
	msn:		fuwenke@gmail.com
	QQ:		799383417
	site:		opensource.kan8888.com
*********************************************************************/

#ifndef _KSOCKET_H
#define _KSOCKET_H

#include "../misc/KString.h"
#include "../define/kwin32types.h"
#include "../misc/KDebug.h"
#include "../io/KStream.h"

#ifdef WIN32
//#include <Winsock2.h>
#else
#include<sys/types.h>
#include<sys/socket.h>
#include <netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#endif

#ifndef WIN32
    typedef int sockobj_t;
    typedef int sockerr_t;
    //#define INVALID_SOCKET -1
    #define SF_NONE     0
    #define SF_ACCEPT   (XPOLLACC|POLLIN)
    #define SF_CONNECT  (XPOLLCNX|POLLOUT)
    #define SF_READ     POLLIN
    #define SF_WRITE    POLLOUT
    #define SF_EXCEPT   POLLPRI
    #define SF_ALL      (XPOLLACC|XPOLLCNX|POLLIN|POLLOUT|POLLPRI)
    #define SOCKERR_NONE        0
    #define SOCKERR_WOULDBLOCK  EAGAIN
    #define SOCKERR_INPROGRESS  EINPROGRESS
    #define SOCKERR_CONNRESET   EPIPE
    #define SOCKERR_EOF         0x7FFFFFFF

    #if (defined(_BSD) && (_BSD < 40)) || (defined(_SOLARIS) && (_SOLARIS < 57))
        typedef int socklen_t;
    #endif
#endif
#ifdef _WIN32
    int inet_aton( const char* cp, struct in_addr* inp );
    typedef SOCKET sockobj_t;
    typedef int sockerr_t;
    // INVALID_SOCKET defined in winsock.h
    typedef int socklen_t;
    typedef int ssize_t;
    #define SF_NONE     0
    #define SF_ACCEPT   FD_ACCEPT
    #define SF_CONNECT  FD_CONNECT
    #define SF_READ     FD_READ
    #define SF_WRITE    FD_WRITE
    #define SF_EXCEPT   FD_OOB
    #define SF_ALL      (FD_ACCEPT|FD_CONNECT|FD_READ|FD_WRITE|FD_OOB)
    #define SOCKERR_NONE        0
    #define SOCKERR_WOULDBLOCK  WSAEWOULDBLOCK
    #define SOCKERR_INPROGRESS  WSAEINPROGRESS
    #define SOCKERR_CONNRESET   WSAECONNRESET
    #define SOCKERR_EOF         0x7FFFFFFF
#endif

#define INVALID_PORT 0xffff
#define MAX_UDP_LEN 8192 

//TODO: add IPv6 support, look into multicast

class CKInetAddr
{
public:
    CKInetAddr( void );
    CKInetAddr( const in_addr& host );
    CKInetAddr( CPCHAR szHost );

    bool        IsValid( void ) const;
    in_addr     GetHost( void ) const;
    void        SetHost( const in_addr& host );
    void        SetHost( CPCHAR szHost );

    static CKInetAddr Any( void );
    static CKInetAddr None( void );

    static bool Resolve( CPCHAR szHost, in_addr* paddr );

    inline operator in_addr( void ) const { return m_addr.sin_addr; }

protected:
    sockaddr_in m_addr;
};

class CKSockAddr : public CKInetAddr
{
public:
    CKSockAddr( void );
    CKSockAddr( const sockaddr_in& addr );
    CKSockAddr( const in_addr& host, UINT16 port = 0 );
    CKSockAddr( CPCHAR szHost, UINT16 port = 0 );

    sockaddr_in GetAddr( void ) const;
    void        SetAddr( const sockaddr_in& addr );
    void        SetAddr( const in_addr& host, UINT16 port );
    void        SetAddr( CPCHAR szHost, UINT16 port );

    UINT16      GetPort( void ) const;
    void        SetPort( UINT16 port );

    static CKSockAddr Any( void );
    static CKSockAddr None( void );

    inline operator sockaddr_in( void ) const { return m_addr; }
};

class CKTcpSocket;
class CKListenSocketNotify
{
public:
	virtual void OnConnection( CKTcpSocket* psock ) = 0;
    virtual void OnClosed( void ) = 0;
};

class CKSocket : public CKStream
{
    friend class CKEventThread;

private: // Unimplemented
    CKSocket( const CKSocket& );
    CKSocket& operator=( const CKSocket& );

public:
    CKSocket( void );
    CKSocket( CKStreamNotify* pNotify );
    virtual ~CKSocket( void );

    virtual bool    IsOpen( void );
    virtual void    Close( void );
    virtual size_t  Read( PVOID pbuf, size_t nLen );
    virtual size_t  Write( CPVOID pbuf, size_t nLen );

    inline bool Read( CKBuffer* pbuf ) { return CKStream::Read( pbuf ); }

    CKSockAddr       GetLocalAddr( void );
    CKSockAddr       GetPeerAddr( void );

    bool        Select( UINT32 nWhich );
    sockerr_t   LastError( void );

	unsigned long Size();

protected:
    sockobj_t GetHandle( void ) { return m_sock; }

protected:
    sockobj_t   m_sock;
    UINT32      m_uSelectFlags;
    sockerr_t   m_err;
};

class CKListenSocket : public CKSocket
{
    friend class CKEventThread;

private: // Unimplemented
    CKListenSocket( const CKListenSocket& );
    CKListenSocket& operator=( const CKListenSocket& );

public:
    CKListenSocket( void );
    CKListenSocket( CKListenSocketNotify* pNotify );
    virtual ~CKListenSocket( void );

    void SetNotify( CKListenSocketNotify* pNotify );
    bool Listen( const CKSockAddr& addr );

protected:
    CKListenSocketNotify*  m_pAcceptNotify;
};

class CKTcpSocket : public CKSocket
{
private: // Unimplemented
    CKTcpSocket( const CKTcpSocket& );
    CKTcpSocket& operator=( const CKTcpSocket& );

public:
    CKTcpSocket( void );
    CKTcpSocket( CKStreamNotify* pNotify );
    virtual ~CKTcpSocket( void );

    bool Connect( const CKSockAddr& addr );
};

class CKUdpSocket : public CKSocket
{
private: // Unimplemented
    CKUdpSocket( const CKUdpSocket& );
    CKUdpSocket& operator=( const CKUdpSocket& );

public:
    CKUdpSocket( void );
    CKUdpSocket( CKStreamNotify* pNotify );
    virtual ~CKUdpSocket( void );

    bool   Bind( const CKSockAddr& addr );
    bool   Connect( const CKSockAddr& addr );

    size_t RecvFrom( CKSockAddr* paddr, PVOID pbuf, size_t nLen );
	 // nWaitSec(等待时间秒), nWaitUsec(等待时间微秒) 
	size_t RecvFrom( CKSockAddr* paddr, PVOID pbuf, size_t nLen,  size_t nWaitSec, size_t nWaitUsec );
    size_t SendTo( const CKSockAddr& addr, CPVOID pbuf, size_t nLen );

protected:
};

#endif //ndef _KSOCKET_H
