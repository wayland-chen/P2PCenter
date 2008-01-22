
#ifdef _UNIX
#include <sys/types.h>
#include <sys/socket.h>
#endif

#include "KThread.h"
#include "../misc/KDebug.h"

#ifdef _WIN32
DWORD g_tlsself;
#else
pthread_key_t g_keyself;
#endif

CKThread* CKThread::This( void )
{
#ifdef _UNIX
	return (CKThread*)pthread_getspecific( g_keyself );
#endif
#ifdef _WIN32
	return (CKThread*)g_tlsself;
#endif
}

/****************************************************************************
*
* CKEventThread
*
****************************************************************************/

#define MIN_TIMER_ALLOC     16
#define MIN_STREAM_ALLOC     4

#define LEFT(n)     (2*n)
#define RIGHT(n)    (2*n+1)
#define PARENT(n)   (n/2)
#define SWAP(t,a,b) { t=a; a=b; b=t; }

CKEventThread::CKEventThread( void ) :
m_nSocks(0),
m_nSockAlloc(0),
m_ppSocks(NULL),
m_pWaitObjs(NULL),
m_nTimers(0),
m_nTimerAlloc(0),
m_ppTimers(NULL),
m_bWantStop(false)
{
	m_nSockAlloc = MIN_STREAM_ALLOC;
	m_ppSocks = new CKSocket*[ MIN_STREAM_ALLOC ];
	memset( m_ppSocks, 0, MIN_STREAM_ALLOC * sizeof(CKStream*) );
	m_pWaitObjs = new waitobj_t[ MIN_STREAM_ALLOC ];
	memset( m_pWaitObjs, 0, MIN_STREAM_ALLOC * sizeof(waitobj_t) );
	m_nTimerAlloc = MIN_TIMER_ALLOC;
	m_ppTimers = new CKTimer*[ MIN_TIMER_ALLOC ];
	memset( m_ppTimers, 0, MIN_TIMER_ALLOC * sizeof(CKTimer*) );
}

CKEventThread::~CKEventThread( void )
{
	delete[] m_ppTimers;
	delete[] m_pWaitObjs;
	delete[] m_ppSocks;
}

bool CKEventThread::Init( void )
{
	return false;
}

int CKEventThread::Exit( void )
{
	return 0;
}

bool CKEventThread::AddStream( CKSocket* pSock )
{
	assert_or_retv( false, pSock );

	if( m_nSocks >= WAITOBJ_MAX )
	{
		return false;
	}

	if( m_nSocks == m_nSockAlloc )
	{
		UINT nNewAlloc = m_nSocks*2;
		CKSocket** ppSocks = new CKSocket*[ nNewAlloc ];
		memset( ppSocks, 0xDD, nNewAlloc * sizeof(CKSocket*) );
		memcpy( ppSocks, m_ppSocks, m_nSocks * sizeof(CKSocket*) );
		delete[] m_ppSocks;
		m_ppSocks = ppSocks;
		waitobj_t* pWaitObjs = new waitobj_t[ nNewAlloc ];
		memset( pWaitObjs, 0xDD, nNewAlloc * sizeof(waitobj_t) );
		memcpy( pWaitObjs, m_pWaitObjs, m_nSocks * sizeof(waitobj_t) );
		delete[] m_pWaitObjs;
		m_pWaitObjs = pWaitObjs;
		m_nSockAlloc = nNewAlloc;
	}

	m_ppSocks[m_nSocks] = pSock;
#ifdef _UNIX
	m_pWaitObjs[m_nSocks].fd = pSock->GetHandle();
	m_pWaitObjs[m_nSocks].events = 0;
	m_pWaitObjs[m_nSocks].revents = 0;
#endif
#ifdef _WIN32
	m_pWaitObjs[m_nSocks] = ::CreateEvent( NULL, FALSE, FALSE, NULL );
#endif
	m_nSocks++;
	return true;
}

void CKEventThread::DelStream( CKSocket* pSock )
{
	assert_or_ret( pSock );

	UINT n;
	for( n = 0; n < m_nSocks; n++ )
	{
		if( m_ppSocks[n] == pSock )
		{
#ifdef _WIN32
			::CloseHandle( m_pWaitObjs[n] );
#endif
			m_nSocks--;
			m_ppSocks[n] = m_ppSocks[m_nSocks];
			m_pWaitObjs[n] = m_pWaitObjs[m_nSocks];
			break;
		}
	}

	if( m_nSocks <= m_nSockAlloc/4 && m_nSockAlloc > MIN_STREAM_ALLOC )
	{
		UINT nNewAlloc = m_nSockAlloc/2;
		CKSocket** ppSocks = new CKSocket*[ nNewAlloc ];
		memset( ppSocks, 0xDD, nNewAlloc * sizeof(CKSocket*) );
		memcpy( ppSocks, m_ppSocks, m_nSocks * sizeof(CKSocket*) );
		delete[] m_ppSocks;
		m_ppSocks = ppSocks;
		waitobj_t* pWaitObjs = new waitobj_t[ nNewAlloc ];
		memset( pWaitObjs, 0xDD, nNewAlloc * sizeof(waitobj_t) );
		memcpy( pWaitObjs, m_pWaitObjs, m_nSocks * sizeof(waitobj_t) );
		delete[] m_pWaitObjs;
		m_pWaitObjs = pWaitObjs;
		m_nSockAlloc = nNewAlloc;
	}
}

void CKEventThread::SetStreamSelect( CKSocket* pSock, UINT nWhich )
{
	assert_or_ret( pSock && pSock->IsOpen() );

	UINT n;
	for( n = 0; n < m_nSocks; n++ )
	{
		if( m_ppSocks[n] == pSock )
		{
#ifdef _UNIX
			m_pWaitObjs[n].events = nWhich;
#endif
#ifdef _WIN32
			nWhich |= FD_CLOSE;
			::WSAEventSelect( pSock->GetHandle(), m_pWaitObjs[n], nWhich );
#endif
			break;
		}
	}
}

// O( lg(n) )
void CKEventThread::AddTimer( CKTimer* pTimer )
{
	assert_or_ret( pTimer );

	if( 1+m_nTimers == m_nTimerAlloc )
	{
		UINT nNewAlloc = m_nTimerAlloc*2;
		CKTimer** ppTimers = new CKTimer*[ nNewAlloc ];
		memset( ppTimers, 0xDD, nNewAlloc * sizeof(CKTimer*) );
		memcpy( ppTimers, m_ppTimers, (1+m_nTimers) * sizeof(CKTimer*) );
		delete[] m_ppTimers;
		m_nTimerAlloc = nNewAlloc;
		m_ppTimers = ppTimers;
	}

	m_nTimers++;
	UINT n = m_nTimers;
	UINT32 t = pTimer->GetTimeout();
	while( n > 1 && t < m_ppTimers[ PARENT(n) ]->GetTimeout() )
	{
		m_ppTimers[n] = m_ppTimers[ PARENT(n) ];
		n = PARENT(n);
	}

	m_ppTimers[n] = pTimer;
}

// O( n )
void CKEventThread::DelTimer( CKTimer* pTimer )
{
	assert_or_ret( pTimer );

	UINT n;
	for( n = 1; n <= m_nTimers; n++ )
	{
		if( m_ppTimers[n] == pTimer )
		{
			m_ppTimers[n] = m_ppTimers[m_nTimers];
			m_nTimers--;
			Heapify( CKTimer::CurrentTime(), n );
			break;
		}
	}

	// Shrink heap if we are using less than 1/4 and more than minimum
	if( m_nTimers <= m_nTimerAlloc/4 && m_nTimerAlloc > MIN_TIMER_ALLOC )
	{
		UINT nNewAlloc = m_nTimerAlloc/2;
		CKTimer** ppTimers = new CKTimer*[ nNewAlloc ];
		memset( ppTimers, 0xDD, nNewAlloc * sizeof(CKTimer*) );
		memcpy( ppTimers, m_ppTimers, (1+m_nTimers) * sizeof(CKTimer*) );
		delete[] m_ppTimers;
		m_nTimerAlloc = nNewAlloc;
		m_ppTimers = ppTimers;
	}
}

void CKEventThread::Heapify( UINT32 now, UINT n )
{

	UINT lnode = LEFT(n);
	UINT rnode = RIGHT(n);
	UINT low = n;

	UINT32 dn, dl, dr;

	dn = m_ppTimers[n]->GetTimeout() - now;
	if( dn > 0x7FFFFFFF ) dn = 0;

	if( lnode <= m_nTimers )
	{
		dl = m_ppTimers[lnode]->GetTimeout() - now;
		if( dl > 0x7FFFFFFF ) dl = 0;
		if( dl < dn )
		{
			low = lnode;
			dn = dl;
		}
	}

	if( rnode <= m_nTimers )
	{
		dr = m_ppTimers[rnode]->GetTimeout() - now;
		if( dr > 0x7FFFFFFF ) dr = 0;
		if( dr < dn )
		{
			low = rnode;
			dn = dr;
		}
	}

	if( low != n )
	{
		CKTimer* tmp;
		SWAP( tmp, m_ppTimers[n], m_ppTimers[low] );
		Heapify( now, low );
	}
}

void CKEventThread::ThreadProcMain(void)
{
	if( Init() )
	{
		while( !m_bWantStop )
		{
			// Figure out how long we can remain in poll/WFMO, possibly forever
			waittimer_t nTimeout = INFTIM;
			if( m_nTimers )
			{
				assert( m_ppTimers[1] );
				nTimeout = m_ppTimers[1]->GetTimeout() - CKTimer::CurrentTime();
				if( nTimeout < 0 || nTimeout > 0x7FFFFFFF ) nTimeout = 0; // Wrap - it's late
			}

#ifdef _UNIX
			int rc = poll( m_pWaitObjs, m_nSocks, nTimeout );
			if( rc < 0 )
			{
				dbgout( "poll() failed: error = %i (%s)", errno, strerror(errno) );
				break;
			}
#endif
#ifdef _WIN32
			DWORD rc = ::WaitForMultipleObjects( m_nSocks, m_pWaitObjs, FALSE, nTimeout );
			if( rc == WAIT_FAILED )
			{
				dbgout( "WFMO failed: error = %u", ::GetLastError() );
				break;
			}
			rc = (rc == WAIT_TIMEOUT) ? 0 : 1;
#endif

			if( rc == 0 && m_nTimers )
			{
				assert( m_nTimers && m_nTimerAlloc >= 2 && m_ppTimers[1] );
				CKTimer* pTimer = m_ppTimers[1];
				if( pTimer->GetMode() == CKTimer::Repeating )
				{
					pTimer->m_next += pTimer->m_interval;
				}
				else
				{
					pTimer->m_mode = CKTimer::Disabled;
					m_ppTimers[1] = m_ppTimers[m_nTimers--];
				}
				Heapify( CKTimer::CurrentTime(), 1 );
				pTimer->GetResponse()->OnTimer();
			}
			if( rc > 0 )
			{
				UINT n;
				for( n = 0; n < m_nSocks; n++ )
				{
					assert( WAITOBJ_IS_VALID( m_pWaitObjs[n] ) && NULL != m_ppSocks[n] );

					int err = SOCKERR_NONE;
					waitevents_t wevt;
					CKSocket* pSock = m_ppSocks[n];

#ifdef _UNIX
					wevt = m_pWaitObjs[n].revents;
					if( (wevt & (POLLIN|POLLERR)) && (pSock->m_uSelectFlags & SF_ACCEPT) == SF_ACCEPT )
					{
						wevt = XPOLLACC;
					}
					if( (wevt & (POLLOUT|POLLERR)) && (pSock->m_uSelectFlags & SF_CONNECT) == SF_CONNECT )
					{
						socklen_t errlen = sizeof(err);
#if defined(_SOLARIS) && (_SOLARIS < 58)
						getsockopt( pSock->GetHandle(), SOL_SOCKET, SO_ERROR, (char*)&err, &errlen );
#else
						getsockopt( pSock->GetHandle(), SOL_SOCKET, SO_ERROR, &err, &errlen );
#endif
						wevt = XPOLLCNX;
					}
					if( (wevt & POLLERR) )
					{
						wevt = pSock->m_uSelectFlags;
					}
#endif
#ifdef _WIN32
					::WSAEnumNetworkEvents( pSock->GetHandle(), m_pWaitObjs[n], &wevt );
					if( wevt.lNetworkEvents & FD_CONNECT )
					{
						err = wevt.iErrorCode[FD_CONNECT_BIT];
					}
					if( wevt.lNetworkEvents & FD_CLOSE )
					{
						wevt.lNetworkEvents = FD_READ;
						DelStream( pSock );
						pSock->GetNotify()->OnClosed();
					}
#endif
					if( WAIT_EVENT_READ( wevt ) )
					{
						pSock->GetNotify()->OnReadReady();
					}
					else if( WAIT_EVENT_WRITE( wevt ) )
					{
						pSock->GetNotify()->OnWriteReady();
					}
					else if( WAIT_EVENT_ACCEPT( wevt ) )
					{
						CKListenSocket* pListen = (CKListenSocket*)pSock;
						sockaddr_in sa;
						socklen_t salen = sizeof(sa);
						sockobj_t sock = accept( pListen->GetHandle(), (sockaddr*)&sa, &salen );
						if( INVALID_SOCKET != sock )
						{
#ifdef _WIN32
							// Cancel selections (they are inherited in Win32)
							::WSAEventSelect( sock, 0, 0 );
#endif
							CKTcpSocket* pNew = new CKTcpSocket();
							pNew->m_sock = sock;
							pListen->m_pAcceptNotify->OnConnection( pNew );
						}
					}
					else if( WAIT_EVENT_CONNECT( wevt ) )
					{
						pSock->GetNotify()->OnConnectDone( err );
					}
					else if( WAIT_EVENT_EXCEPT( wevt ) )
					{
						pSock->GetNotify()->OnExceptReady();
					}
				}
			}
		}
	}
	Exit();
}