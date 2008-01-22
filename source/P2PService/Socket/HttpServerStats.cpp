#include "stdafx.h"

#include "HttpServerStats.h"

/////////////////////////////////////////////////////////////////////////////
// HttpServerStats

HttpServerStats::HttpServerStats () :
	_stats(),
	_numberOfHits(0)
{
	create();
}

HttpServerStats::~HttpServerStats ()
{
	_stats.clear();
}


// create release
bool HttpServerStats::create ()
{
	// if stats not created create
	if ( _stats.size() == 0 )
	{
		// init stats
		for ( long i=0; i < STATUS_NO_COUNTERS; i++ )
		{
			_stats.push_back(0);
		}
	}

	// init stats
	reset();

	return true;
}

void HttpServerStats::release ()
{
	// clear stats
	_stats.clear();

	// show no hits
	_numberOfHits = 0;

	// close log file
	_logFile.close();
	_logFile.release();


}


void HttpServerStats::reset ()
{
	// init stats
	for ( long i=0; i < STATUS_NO_COUNTERS; i++ )
	{
		_stats[i] = 0;
	}

	// show no hits
	_numberOfHits = 0;
}

void HttpServerStats::hit ( HttpRequest & request )
{
	// increment the total hit count
	_numberOfHits++;
}

void HttpServerStats::getStats ( HttpRequest & request )
{
	// increment the status' group count
	long status = request.getStatus();
	if ( status >= 100 && status < 600 )
		_stats[ status/100 - 1 ]++;
	else
		_stats[ STATUS_SERVERERR ]++;

	// see if we want to write entry in log file
	if ( _logging )
	{
		stringstream log;

		// get host where request originated
		log << request.getHost();

		// request status
		log << request.getStatus();

		// elements to add
		// time
		// string args (ex: www.host.com?ar1=val1&ar2=val2
		// request time		

		_logFile << log;
	}
}

