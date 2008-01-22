#if !defined(ServerSocket_H)
#define ServerSocket_H

#include "Socket.h"
//#include "ConnectSocket.h"



////////////////////////////////////////////////////////////////////////////////
// ServerSocket
//
// Purpose: class for reading/writing a socket


class ServerSocket :
	public Socket
{
protected:

	long			_noQued;

public:


	ServerSocket ( long noQued ) :
		Socket(),
		_noQued(noQued)
	{
		
	}

	ServerSocket ( ServerSocket & sock ) :
		Socket( (Socket &) sock ),
		_noQued(0)
	{
		_noQued = sock._noQued;
	}




	virtual ~ServerSocket ()
	{
		close();
	}

	// open methods
	bool open ( long port )
	{
		// show using local host
		string host = "127.0.0.1";//"localhost";

		// get socket
		_socket = socket(PF_INET,SOCK_STREAM,0);

		if ( _socket == INVALID_SOCKET )
			return false;

		// init address in
		initSockAddrIn( host, port, _addrIn );

		// bind socket
		if ( bind( _socket, (const SOCKADDR *) &_addrIn, sizeof(_addrIn) ) == SOCKET_ERROR )
		{
			int error = getLastError();
			return false;
		}

		return true;
	}


	bool listen ()
	{
		// listen for connection
		if ( ::listen( _socket, _noQued ) != 0 )
		{
			long error = getLastError();

			return false;
		}
		else
			return true;
	}

	bool accept ( Socket & socket )
	{

		// get new socket
		SOCKADDR addr;
		int		 size = sizeof(addr);
		memset( &addr, 0, sizeof(addr) );

		SOCKET sock =
		::accept( _socket, &addr, &size );

		if ( sock == INVALID_SOCKET )
		{
			int err = getLastError();

			return false;
		}
		else
		{
			socket.create( sock, addr );
			return true;
		}
	}

};



#endif