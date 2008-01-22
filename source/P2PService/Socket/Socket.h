#if !defined(Socket_H)
#define Socket_H

#include "Lock.h"
#include "Thread.h"

#include <string>
#include <sstream>
using namespace std;


// define mapping of error methods
#if defined( _WIN32 )

#if !defined(errno)
#define errno	WSAGetLastError
#endif

#else

// win32 define ioctl for driver control
// so redefine ioctlsocket for normal bsd

#define ioctlsocket ioctl

#endif

////////////////////////////////////////////////////////////////////////////////
// Socket
//
// Purpose: class for reading/writing a socket


class Socket :
	private Lock
{
protected:

	SOCKET			_socket;
	SOCKADDR_IN		_addrIn;
	long			_port;
public:


	Socket () :
		_socket(INVALID_SOCKET),
		_port()
	{}


	Socket ( Socket & sock ) :
		_socket(INVALID_SOCKET),
		_port()
	{
		create( sock._socket, (SOCKADDR &) sock._addrIn );	
	}

	virtual ~Socket ()
	{
		close();
	}

	// create/release
	bool create ( SOCKET & sock, SOCKADDR & addr )
	{
		if ( sock == INVALID_SOCKET )
			return false;

		// get new socket
		memcpy( &_addrIn, (SOCKADDR *) &addr, sizeof(addr) );
		memcpy( &_socket, &sock, sizeof(_socket) );

		return true;
	}

	void release ()
	{
		// close socket if open
		if ( _socket != INVALID_SOCKET )
		{
			closesocket(_socket);
			_socket = INVALID_SOCKET;
		}
	}

	bool isValid ()
	{
		if ( _socket != INVALID_SOCKET )
			return true;
		else
			return false;
	}

	long getPort ()
	{
		return _port;
	}

	// open methods
	static bool initSockAddrIn ( string & host, long port, SOCKADDR_IN & addrIn )
	{
		// init socket in struct
		memset(&addrIn,0,sizeof(addrIn));

		// set family
		addrIn.sin_family = AF_INET;


		if ( !host.empty() )
		{
			// retrieve the host data corresponding to the host name
			LPHOSTENT phost;
			if ((phost = gethostbyname(host.c_str())) != NULL) 
			{
				// assign the socket IP address. 
				memcpy ( (char FAR *) &(addrIn.sin_addr), 
						  phost->h_addr, 
						  phost->h_length );
			}
			else
			{
				// get inet address
				addrIn.sin_addr.s_addr = inet_addr(host.c_str());
			}
		}
		else
		{
			// set inet address
			addrIn.sin_addr.s_addr = htonl(INADDR_ANY);
		}

		// set port
		addrIn.sin_port = htons(port);

		return true;
	}

	bool open ( string host, long port )
	{
		// get socket
		_socket = socket(PF_INET,SOCK_STREAM,0);

		if ( _socket == INVALID_SOCKET )
			return false;

		// init address in
		_port = port;
		initSockAddrIn( host, port, _addrIn );

		// connect to socket
		if ( connect( _socket, (const SOCKADDR *) &_addrIn, sizeof(_addrIn) ) == SOCKET_ERROR )
		{
			int error = getLastError();
			return false;
		}
		
		
		return true;
	}

	// close
	void close ()
	{
		// close socket if open
		if ( _socket != INVALID_SOCKET )
		{
			closesocket(_socket);
			_socket = INVALID_SOCKET;
		}
	}

	static int getLastError ()
	{
		return WSAGetLastError();
	}

	static bool init ()
	{
		#if defined( _WIN32 )

		// init startup
		WSADATA WSAData;
		WSAStartup( MAKEWORD(2,0), &WSAData );

		#endif

		return true;
	}

	long getRecieveSize ()
	{
		// return not of bytes waiting
		// if fail show none
		long noBytes;
		if ( ioControl(FIONREAD, (unsigned long *) &noBytes) )
			return noBytes;
		else
			return 0;
	}

	bool setSocketNonBlocking ()
	{
		// set socket to non-blocking mode
		// probably need to add more ioctl later
		unsigned long ioctl_opt = -1;
		return ioControl( FIONBIO, &ioctl_opt);

		/*
		unsigned long ioctl_opt = -1;
		if ( ioctlsocket(_socket,FIONBIO,&ioctl_opt) == SOCKET_ERROR ) 
		{
			fprintf(stderr,"ioctlsocket failed %d\n",getLastError());
			
			return false;
		}
		else
			return true;
		*/
	}
		


	// recieve/send methods
	long send ( string & buffer )
	{
		return send( (LPTSTR) buffer.c_str(), buffer.size() );
	}

	long send ( LPCTSTR buffer, DWORD noToWrite )
	{
		return send( (LPTSTR) buffer, noToWrite );
	}

	long send ( LPTSTR pBuffer, DWORD noToWrite )
	{
		if ( !lock() )
			return 0;

		// send to socket
		int noWritten =
		::send( _socket, pBuffer, noToWrite, 0 );

		unlock();

		// if error show none sent
		if ( noWritten == SOCKET_ERROR )
			return 0;
		else
			return noWritten;
	}

	long recieve ( LPTSTR pBuffer, DWORD noToRead )
	{
		if ( !lock() )
			return false;

		// recieve from socket
		int noRead =
		recv( _socket, pBuffer, noToRead, 0 );

		// if no recieve error
		if ( noRead == SOCKET_ERROR )
		{
			noRead = -1;
		}
		else
		{
			// else terminate buffer
			pBuffer[noRead] = '\0';
		}

		unlock();

		return noRead;
	}

	bool ioControl ( long cmd, u_long * argp )
	{
		// perfom action
		long ret =
		ioctlsocket(_socket,cmd,argp);

		// if success show it
		// else get error show failed
		if ( ret == 0 )
			return true;
		else
		{
			// show error
			fprintf(stderr,"ioctlsocket failed %d\n",getLastError());

			return false;
		}

	}


	// operators
	operator SOCKET ()
	{
		return _socket;
	}

	Socket &  operator << ( string & buffer )
	{
		send( (LPTSTR) buffer.c_str(), buffer.size() );
		return *this;
	}

	Socket &  operator >> ( stringstream & strm )
	{
		send( (LPTSTR) strm.str().c_str(), strm.str().size() );
		return *this;
	}

	Socket &  operator << ( LPCTSTR buffer )
	{
		send( (LPTSTR) buffer, lstrlen(buffer) );
		return *this;
	}

	Socket &  operator << ( LPTSTR buffer )
	{
		send( buffer, lstrlen(buffer) );
		return *this;
	}

	// test if file open
	bool isOpen ()
	{
		if ( _socket == INVALID_SOCKET )
			return false;
		else
			return true;
	}
};





#endif