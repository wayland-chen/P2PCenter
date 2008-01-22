#if !defined(SocketNotify_H)
#define SocketNotify_H

#include "Socket.h"

////////////////////////////////////////////////////////////////////////////////
// SocketEvent
//
// Purpose: bitmask for socket events

enum
{
	idFdRead		= 0x00000001,
	idFdWrite		= 0x00000002,
	idFdOobd		= 0x00000004,
	idFdAccept		= 0x00000008,
	idFdConnect		= 0x00000010,
	idFdClose		= 0x00000020,
	idFdAll			= 0xffffffff
};


////////////////////////////////////////////////////////////////////////////////
// SocketNotify
//
// Purpose: notification handler for socket


class SocketNotify
{
	Socket *	_socket;
public:

	SocketNotify () :
		_socket(0)
	{}

	SocketNotify ( Socket & socket ) :
		_socket(&socket)
	{}

	// notification methods
	virtual bool onReceive ( int error )
	{
		return true;
	}

	virtual bool onSend	( int error )
	{
		return true;
	}

	virtual bool onOutOfBandData ( int error )
	{
		return true;
	}

	virtual bool onAccept ( int error )
	{
		return true;
	}

	virtual bool onConnect ( int error )
	{
		return true;
	}

	virtual bool onClose ( int error )
	{
		return true;
	}


};



#endif