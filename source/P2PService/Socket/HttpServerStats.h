#if !defined(HttpServerStats_H)
#define HttpServerStats_H

#include "TextFile.h"
#include "HttpRequest.h"

#include <vector>
using namespace std;

#pragma warning( disable:4800 )


// types of status counters
enum 
{ 
	STATUS_INFORMATIONAL = 0,
	STATUS_SUCCESS, 
	STATUS_REDIRECTION,
	STATUS_CLIENTERR, 
	STATUS_SERVERERR,
	STATUS_NO_COUNTERS
};

///////////////////////////////////////////////////////////////////////////////////////
// HttpServerStats
//
// Purpose:		http server statistics



class HttpServerStats
{
protected:

	bool			_logging;			// shows if logging results
	vector<long>	_stats;				// stats for server by status group
	int				_numberOfHits;		// total hits
	TextFile		_logFile;			// log file for server stats

public:

	HttpServerStats ();
	virtual ~HttpServerStats ();

	// create release
	bool create  ();
	void release ();

	// status methods
	void    reset			();
	void    hit				( HttpRequest & request );
	void    getStats		( HttpRequest & request );

	// logging methods
	bool getLogging ()
	{
		return _logging;
	}

	void setLogging ( bool set )
	{
		_logging = set;
	}


};


#endif