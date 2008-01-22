#if !defined(SocketHandler_H)
#define SocketHandler_H

#include "Socket.h"
#include "SocketNotify.h"




////////////////////////////////////////////////////////////////////////////////
// SocketHandler
//
// Purpose: handles basics of socket. provides for notification of 
//			socket events. event notification can be either passed in
//			or used as is. also provides a socket if one is not
//			passed in


class SocketHandler :
	public Thread,
	public SocketNotify
{
	bool				_ownSocket;			// shows if delete socket on cleanup
	Socket *			_socket;			// ptr to socket associated w/handler
	SocketNotify *		_subscriber;		// subscriber to socket events
	unsigned long		_fdEventMask;			// shows which events to fire

public:

	SocketHandler () :
		Thread(),
		SocketNotify(),

		_ownSocket(false),
		_socket(NULL),
		_subscriber( (SocketNotify *) this ),
		_fdEventMask(idFdAll)

	{
		// create the socket
		createSocket();
	}

	SocketHandler ( SocketNotify & subscriber ) :
		Thread(),
		SocketNotify(),

		_ownSocket(false),
		_socket(NULL),
		_subscriber( &subscriber ),
		_fdEventMask(idFdAll)

	{
		// create the socket
		createSocket();
	}


	SocketHandler ( Socket & sock ) :

		Thread(),
		SocketNotify(),

		_ownSocket(false),
		_socket( &sock ),
		_subscriber( (SocketNotify *) this ),
		_fdEventMask(idFdAll)

	{}

	SocketHandler ( Socket & sock, SocketNotify & subscriber ) :

		Thread(),
		SocketNotify(),

		_ownSocket(false),
		_socket(&sock),
		_subscriber(&subscriber),
		_fdEventMask(idFdAll)

	{}


	virtual ~SocketHandler ()
	{
		release();
	}

	// create/release
	bool create ()
	{
		if ( !Thread::create() )
			return false;

		return true;
	}

	void release ()
	{
		Thread::release();

		if ( _ownSocket && _socket )
			delete _socket;

		_socket = 0;
	}

	// socket methods
	bool createSocket ()
	{
		// create the socket
		_socket = new Socket;

		// show we own the socket
		// if created okay
		if ( _socket )
		{
			_ownSocket = true;
			return true;
		}
		else
			return false;
	}

	Socket & getSocket ()
	{
		return *_socket;
	}

	void setSocket ( Socket & socket )
	{
		_socket = &socket;
	}

	bool isValid ()
	{ 
		if ( _socket )
			return true;
		else
			return false;
	}

	bool isConnected ()
	{
		if ( _socket->isValid() )
			return true;
		else
			return false;
	}



	// subscriber methods

	long getFdEvents ()
	{
		return _fdEventMask;
	}

	void asyncSelet ( long set )
	{
		_fdEventMask = set;
	}


	SocketNotify & getSubscriber ()
	{
		return *_subscriber;
	}

	void setSubscriber ( SocketNotify & subscriber )
	{
		_subscriber = &subscriber;
	}

	// handle socket processing
	virtual
	long process ( Socket & sock )
	{
		// if no socket stop
		if ( !isValid() )
			return -1;

		// The fd sets should be zeroed out before using them to prevent errors.
		fd_set readfds, writefds, exceptfds;
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_ZERO(&exceptfds);

		// read from socket
		while ( true )
		{
			while ( true )
			{
				// show no error
				long error = 0;

				// set time to wait
				struct timeval timeout;
				timeout.tv_sec  = 1;
				timeout.tv_usec = 0;

				// setup to wait for read event
				FD_SET(sock,&readfds);
				FD_SET(sock,&writefds);
				FD_SET(sock,&exceptfds);

				int i = select(0,&readfds,&writefds,&exceptfds,&timeout);
				//int i = select( (SOCKET)sock + 1,&readfds,&writefds,&exceptfds,&timeout);

				if ( i == SOCKET_ERROR ) 
				{
					long error = sock.getLastError();
					//fprintf(stderr,"select failed %d\n",error);
					return -1;
				}


				if ( FD_ISSET(sock,&readfds) && 
					 _fdEventMask | (idFdRead | idFdConnect) )
				{
					// got a recieve so read data
					if ( _subscriber )
					{
						long length = sock.getRecieveSize();
						if ( length == 0 )
							_subscriber->onConnect(error);
						else
							_subscriber->onReceive(error);
					}
				}
				else if ( FD_ISSET(sock,&writefds) && _fdEventMask | idFdWrite )
				{
					// got a recieve so read data
					if ( _subscriber )
						_subscriber->onSend(error);
				}
				else if ( FD_ISSET(sock,&exceptfds)  && _fdEventMask | idFdClose )
				{
					// got a recieve so read data
					if ( _subscriber )
						_subscriber->onClose(error);
				}


				// test end event
				if ( WaitForSingleObject( getEndEvent(), 0 ) != WAIT_TIMEOUT )
					return -1;

				// do idle processing
				idle();

			}

			
		}

		return 1;

	}


	// run method implementation
	long run ()
	{
		// if no socket stop
		if ( !isValid() )
			return -1;

		// get socket
		Socket & sock = getSocket();

		// set socket to non-blocking
		if ( !sock.setSocketNonBlocking() )
			return 0;


		return process(sock);
	}

};





////////////////////////////////////////////////////////////////////////////////////
// CreateSocketHandler
// 
// Purpose:		create socket handler is a class 
//				used only to create socket handlers or their derived classes. 
//				you must implement you own


class CreateSocketHandler
{
	long	i;
public:

	CreateSocketHandler () 
	{
		i = -1;
	}

	virtual ~CreateSocketHandler ()
	{}

	// create/release
	virtual bool create ()
	{
		return true;
	}

	virtual void release ()
	{}

	// responsible for create a connection
	virtual 
	SocketHandler * createHandler () = 0;

};




#endif