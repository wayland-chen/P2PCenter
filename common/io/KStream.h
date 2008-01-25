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

#ifndef _STREAM_H
#define _STREAM_H

#include "../misc/KString.h"
#include "../define/kwin32types.h"
#include "../misc/KDebug.h"
#include "../misc/KPtrList.h"

#ifndef WIN32
    // some custom bits (we rely on pollfd events being a 16-bit type)
    // note that these are combined with POLLIN/POLLOUT
    #define XPOLLACC    0x00010000  /* accept */
    #define XPOLLCNX    0x00020000  /* connect */

    #include <sys/poll.h>
    typedef pollfd              waitobj_t;
    typedef INT32               waitevents_t;
   #define WAITOBJ_MAX          0xffff

    #define WAITOBJ_IS_VALID(obj)       (obj.fd != -1)
    #define WAIT_EVENT_ACCEPT(wevt)     (wevt & XPOLLACC)
    #define WAIT_EVENT_CONNECT(wevt)    (wevt & XPOLLCNX)
    #define WAIT_EVENT_READ(wevt)       (wevt & POLLIN)
    #define WAIT_EVENT_WRITE(wevt)      (wevt & POLLOUT)
    #define WAIT_EVENT_EXCEPT(wevt)     (wevt & POLLPRI)
#endif
#ifdef _WIN32
    typedef HANDLE              waitobj_t;
    typedef WSANETWORKEVENTS    waitevents_t;
    #define WAITOBJ_MAX         MAXIMUM_WAIT_OBJECTS

    #define WAITOBJ_IS_VALID(obj)       (obj != NULL)
    #define WAIT_EVENT_ACCEPT(wevt)     (wevt.lNetworkEvents & FD_ACCEPT)
    #define WAIT_EVENT_CONNECT(wevt)    (wevt.lNetworkEvents & FD_CONNECT)
    #define WAIT_EVENT_READ(wevt)       (wevt.lNetworkEvents & FD_READ)
    #define WAIT_EVENT_WRITE(wevt)      (wevt.lNetworkEvents & FD_WRITE)
    #define WAIT_EVENT_EXCEPT(wevt)     (wevt.lNetworkEvents & FD_OOB)
#endif

class CKStreamNotify
{
public:
    virtual void OnConnectDone( int err ) = 0;
    virtual void OnReadReady  ( void ) = 0;
    virtual void OnWriteReady ( void ) = 0;
    virtual void OnExceptReady( void ) = 0;
    virtual void OnClosed     ( void ) = 0;
};

class CKStream
{
    friend class CKEventThread;

private: // Unimplemented
    CKStream( const CKStream& );
    CKStream& operator=( const CKStream& );

public:
    CKStream( void );
    CKStream( CKStreamNotify* pNotify );
    virtual ~CKStream( void );

    virtual bool    IsOpen( void ) = 0;
    virtual void    Close( void ) = 0;
    virtual size_t  Read( PVOID pbuf, size_t nLen ) = 0;
    virtual size_t  Write( CPVOID pbuf, size_t nLen ) = 0;

    void    SetNotify( CKStreamNotify* pNotify );
    bool    Read( CKBuffer* pbuf );
    bool    Write( const CKBuffer& buf );

protected:
    inline CKStreamNotify* GetNotify( void ) { return m_pNotify; }

protected:
    CKStreamNotify* m_pNotify;
};
typedef CKPtrList<CKStream> CKStreamList;

#endif //ndef _STREAM_H
