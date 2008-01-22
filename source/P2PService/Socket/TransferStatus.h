#if !defined(TransferStatus_H)
#define TransferStatus_H



/////////////////////////////////////////////////////////////////////////////////////
// TransferStatus
//
// Purpose:	used to keep track of a transfer

class TransferStatus
{
	long			_blockSize;		// no to send at one time
	long			_noToSend;		// number of bytes left to send 
	long			_noSent;		// number of bytes sent in response

public:

	TransferStatus () :
		_blockSize(0),
		_noToSend(0),
		_noSent(0)
	{}

	// setup status
	void reset ()
	{
		_noToSend = 0;
		_noSent = 0;
	}

	void begin ( long noToSend )
	{
		_blockSize = noToSend;
		_noToSend = noToSend;
		_noSent = 0;
	}

	void setSizeToSend ( long noToSend )
	{
		begin(noToSend);
	}

	// get/set block size
	long getBlockSize ()
	{
		return _blockSize;
	}

	void setBlockSize ( long set )
	{
		_blockSize = set;
	}

	// status methods
	long size()
	{
		return _blockSize;
	}

	long getNoToSend ()
	{
		return _noToSend;
	}

	long getNoSent ()
	{
		return _noSent;
	}

	void incNoSent ( long inc )
	{
		// if not valid stop
		if ( inc <= 0 )
			return;

		// update no sent
		_noSent += inc;
		if ( _noSent > size() )
			_noSent = size();
	}

	void decNoToSend ( long dec )
	{
		// if not valid stop
		if ( dec <= 0 )
			return;

		// update no to send
		_noToSend -= dec;
		if ( _noToSend < 0 )
			_noToSend = 0;
	}

	bool finished ()
	{
		// if all sent show it
		//if ( _noSent == _noToSend )
		if ( 0 == _noToSend )
			return true;
		else
			return false;
	}




};

#endif