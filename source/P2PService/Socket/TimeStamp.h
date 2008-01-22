#if !defined(TimeStamp_H)
#define TimeStamp_H


#include <string>
#include <sstream>
using namespace std;


#define MIN_DATE                (-657434L)  // about year 100
#define MAX_DATE                2958465L    // about year 9999

double	DoubleFromDate				( DATE dt );
DATE	DateFromDouble				( double dbl );


/*

// Verifies will fail if the needed buffer size is too large
#define MAX_TIME_BUFFER_SIZE    128         // matches that in timecore.cpp


// Half a second, expressed in days
#define HALF_SECOND  (1.0/172800.0)
BOOL	OleDateFromTm				( WORD wYear, WORD wMonth, WORD wDay,
									  WORD wHour, WORD wMinute, WORD wSecond, 
									  DATE& dtDest );
BOOL	TmFromOleDate				( DATE dtSrc, struct tm& tmDest );
void	TmConvertToStandardFormat	( struct tm& tmSrc );
double	DoubleFromDate				( DATE dt );
DATE	DateFromDouble				( double dbl );
*/

//////////////////////////////////////////////////////////////////////////////////
// TimeStamp

class TimeStamp
{
	DATE			m_timestamp;
	long			m_valid;
	bool			m_standard;
	string			m_format;

public:

	TimeStamp	();

	TimeStamp	( TimeStamp& dateSrc );
	TimeStamp	( VARIANT& varSrc );
	TimeStamp	( DATE dtSrc );

	TimeStamp	( SYSTEMTIME& systimeSrc );
	TimeStamp	( FILETIME& filetimeSrc);

	TimeStamp	( long nYear, long nMonth, long nDay,
				  long nHour, long nMin, long nSec );


	// enum to show if timestamp valid
	enum
	{
		valid = 0,
		invalid = 1,    // Invalid date (out of range, etc.)
		null = 2,       // Literally has no value
	};


	// format methods
	string  format				( LPTSTR strFormat );
	void	setDefaultFormat	();
	void	setFormat			( string & strFormat );
	void	getTimeStr			( string & timeStr, bool showSeconds );
	void	getDateStr			( string & dateStr, bool useShortDateFormat );

	bool	getTimeStamp		( string & str, bool showSeconds = false );
	bool	getTimeStamp		( string & str,
		                          string & format, 
							      DWORD dwFlags = 0,
								  LCID lcid = LANG_USER_DEFAULT );

	static void getTimeStamp	(  DATE timestamp, string & str );


	// get current and store
			void	getCurTimeStamp	();
	static	void	getCurTimeStamp	( DATE & timestamp );
	static	void	getCurTimeStamp	( string & str );

	// access methods
	long	getStatus		();

	long	getYear			();
	long	getMonth		();		    // month of year (1 = Jan)
	long	getDay			();	        // day of month (0-31)
	long	getHour			();			// hour in day (0-23)
	long	getMinute		();			// minute in hour (0-59)
	long	getSecond		();			// second in minute (0-59)
	long	getDayOfWeek	();			// 0=Sun, 1=Mon, ..., 6=Sat

	double	getDays			();			// span in days
	double	getHours		();			// span in hours
	double	getMinutes		();			// span in minutes
	double	getSeconds		();			// span in seconds

	// copy methods
	void		operator =		( const TimeStamp& dateSrc );
	void		operator =		( VARIANT& varSrc );
	void		operator =		( DATE dtSrc );

	void		operator =		( SYSTEMTIME& systimeSrc );
	void		operator =		( FILETIME& filetimeSrc );

	// compare methods
	bool		operator ==		( const TimeStamp& date );
	bool		operator !=		( const TimeStamp& date );
	bool		operator <		( const TimeStamp& date );
	bool		operator >		( const TimeStamp& date );
	bool		operator <=		( const TimeStamp& date );
	bool		operator >=		( const TimeStamp& date );

	// math methods
	TimeStamp	operator  +		( TimeStamp & dateSpan );
	TimeStamp	operator  -		( TimeStamp & dateSpan );
	TimeStamp & operator +=		( TimeStamp dateSpan );
	TimeStamp & operator -=		( TimeStamp dateSpan );


	operator DATE();

	// set/get datestamp methods

	void	setDateTime		( long year, long month, long day,
							  long hour, long minute, long second );

	void	setDate			( long year, long month, long day );
	void	setTime			( long hour, long minute, long second );
	void	setTime			( SYSTEMTIME & sysTime );
	void	setDate			( SYSTEMTIME & sysTime );

	void	getDate			( long & year, long & month, long & day );
	void	getTime			( long & hour, long & minute, long & second );
	void	getTime			( SYSTEMTIME & sysTime );
	void	getDate			( SYSTEMTIME & sysTime );

	// check if valid
	bool	isValid			();

	// shows if time is empty
	bool	empty			();
	

};


inline
bool TimeStamp::empty ()
{

	if ( !isValid() || m_timestamp == 0 )
		return false;
	else
		return true;
}


inline
void TimeStamp::getDate ( long & year, long & month, long & day )
{
	SYSTEMTIME sysTime;
	VariantTimeToSystemTime( m_timestamp, &sysTime );

	year  = sysTime.wYear;
	month = sysTime.wMonth;
	day   = sysTime.wDay;
}

inline
void TimeStamp::getTime	( long & hour, long & minute, long & second )
{
	SYSTEMTIME sysTime;
	VariantTimeToSystemTime( m_timestamp, &sysTime );

	hour    = sysTime.wHour;
	minute  = sysTime.wMinute;
	second  = sysTime.wSecond;
}



inline
void TimeStamp::getTime ( SYSTEMTIME & sysTime )
{
	VariantTimeToSystemTime( m_timestamp, &sysTime );
}

inline
void TimeStamp::getDate ( SYSTEMTIME & sysTime )
{
	VariantTimeToSystemTime( m_timestamp, &sysTime );
}

inline
void TimeStamp::setDate(long year, long month, long day )
{ 
	setDateTime(year, month, day, 0, 0, 0); 
}

inline
void TimeStamp::setTime(long hour, long minute, long second)
{	
	// Set date to zero date - 12/30/1899
	setDateTime(1899, 12, 30, hour, minute, second); 
}

inline
void TimeStamp::setTime ( SYSTEMTIME & sysTime )
{
	// Set date to zero date - 12/30/1899
	setDateTime(1899, 12, 30, sysTime.wHour, sysTime.wMinute, sysTime.wSecond); 
}

inline
void TimeStamp::setDate ( SYSTEMTIME & sysTime )
{ 
	setDateTime(sysTime.wYear, sysTime.wMonth, sysTime.wDay, 0, 0, 0); 
}




inline
double TimeStamp::getDays()
{ 
	_ASSERT(m_valid == valid); 
	return m_timestamp; 
}

inline
double TimeStamp::getHours()
{ 
	_ASSERT(m_valid == valid); 
	return m_timestamp * 24; 
}

inline
double TimeStamp::getMinutes()
{ 
	_ASSERT(m_valid == valid); 
	return m_timestamp * 24 * 60; 
}

inline
double TimeStamp::getSeconds()
{ 
	_ASSERT(m_valid == valid); 
	return m_timestamp * 24 * 60 * 60; 
}


/*
inline
long TimeStamp::getDays()
{ 
	_ASSERT(m_valid == valid); 
	return (long)m_timestamp; 
}
*/

inline
void TimeStamp::operator=( const TimeStamp& dateSrc)
{ m_timestamp = dateSrc.m_timestamp; m_valid = dateSrc.m_valid; }

inline
bool TimeStamp::operator==(const TimeStamp& date)
{ 
	return (m_valid == date.m_valid && m_timestamp == date.m_timestamp); 
}

inline
bool TimeStamp::operator!=(const TimeStamp& date)
{ 
	return (m_valid != date.m_valid || m_timestamp != date.m_timestamp); 
}

inline
TimeStamp& TimeStamp::operator+=(TimeStamp dateSpan)
{ 
	*this = *this + dateSpan; 
	return *this; 
}

inline
TimeStamp& TimeStamp::operator-=(TimeStamp dateSpan)
{ 
	*this = *this - dateSpan; 
	return *this; 
}

inline
TimeStamp::operator DATE()
{ 
	return m_timestamp; 
}



#endif

