#if !defined(ServerSocketHandler_H)
#define ServerSocketHandler_H



#include "ServerSocket.h"
#include "SocketHandler.h"

#include <list>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// ServerSocketHandler
//
// Purpose: handles basics of server socket connections

typedef list<SocketHandler *>				SocketHandlers;
typedef SocketHandlers::iterator			itrSocketHandlers;
typedef SocketHandlers::reverse_iterator	revItrSocketHandlers;



class ServerSocketHandler :
	public SocketHandler
{
	CreateSocketHandler &		_createSocket;	// creates socket handler
	SocketHandlers				_handlers;		// active socket handlers


public:

	ServerSocketHandler ( ServerSocket & sock, CreateSocketHandler & createSocket ) :
	    SocketHandler( (Socket &) sock ),
		_createSocket(createSocket),
		_handlers()
	{}


	// get server socket
	ServerSocket & getServerSocket ()
	{
		return (ServerSocket &) SocketHandler::getSocket();
	}

	// create/release
	bool create ()
	{
		if ( !SocketHandler::create() )
			return false;

		return true;
	}

	void release ()
	{
		if ( _handlers.empty() )
			return;

		// release any socket handlers
		for ( itrSocketHandlers itr = _handlers.begin(); 
			  itr != _handlers.end();
			  itr++ )
		{
			// get handler
			SocketHandler * handler = *itr;

			// do cleanup and get rid of it
			if ( handler )
			{
				handler->release();
				delete handler;
			}
		}

		// cleanup list
		_handlers.clear();
	}

	// check handlers to see if any are not active
	void cleanup ()
	{
		if ( _handlers.empty() )
			return;

		// release any socket handlers
		itrSocketHandlers itr = _handlers.begin(); 
		while ( itr != _handlers.end() )
		{
			// get handler
			SocketHandler * handler = *itr;

			// do cleanup if needed
			if ( handler && !handler->isConnected() )
			{
				// get rid of it
				handler->release();
				delete handler;

				// remove this one goto next
				itr = _handlers.erase(itr);
			}
			else
			{
			  itr++;

			}
		}


	}

	// socket event methods
	virtual bool onConnect ( int error )
	{
		// get server socket
		ServerSocket & server = getServerSocket();

		// get socket, accept and create
		SocketHandler * handler = _createSocket.createHandler();
		if ( !handler )
			return false;

		// add to handlers
		_handlers.push_back(handler);

		// do accept on socket
		Socket & sock = (*handler).getSocket();
		if ( !server.accept( sock ) )
		{
			return false;
		}

		// finish create
		handler->create();

		return true;
	}

	// run time processing
	long run ()
	{
		// get server socket
		ServerSocket & server = getServerSocket();

		// if listen fails
		if ( !server.listen() )
			return -1;

		// set socket to non blocking
		if ( !server.setSocketNonBlocking() )
			return -1;

		return process(server);
	}

	// idle processing
	void idle ()
	{
		// do cleanup during idle processing
		cleanup();
	}
	

};


/*
// set buffer
stringstream strm;
strm << "<Contact>" << endl;
strm << "<name>Joe Blow</name>" << endl;
strm << "<id>" << id << "</id>" << endl;
strm << "</Contact>" << endl;

id++;

// not implemented at this time
*this << strm.str();

char buffer[1024];
long noRead = recieve( buffer, 1024 );

long idIn = (long) atoi(buffer);


*/




#endif