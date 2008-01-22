#include "StdAfx.h"
#include "TimeStamp.h"

#include <math.h>


#if defined(_DEBUG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////////////////
// conversion methods

double DoubleFromDate(DATE date)
{
	// No problem if positive
	if (date >= 0)
		return date;

	try
	{
		// If negative, must convert since negative dates not continuous
		// (examples: -1.25 to -.75, -1.50 to -.50, -1.75 to -.25)
		double temp = ceil(date);
		return temp - (date - temp);
	}
	catch(...)
	{
		_ASSERT(FALSE);
	}
}

DATE DateFromDouble(double date)
{
	// No problem if positive
	if (date >= 0)
		return date;
	try
	{
		// If negative, must convert since negative dates not continuous
		// (examples: -.75 to -1.25, -.50 to -1.50, -.25 to -1.75)
		double temp = floor(date); // dbl is now whole part
		return temp + (temp - date);
	}
	catch(...)
	{
		_ASSERT(FALSE);
	}
}



/////////////////////////////////////////////////////////////////////////////
// TimeStamp class

//////////////////////////////////////////////////////////////////////////////////////
// constructors

TimeStamp::TimeStamp()
{ 
	m_timestamp = 0; 
	m_valid = valid; 

	setDefaultFormat();
}

TimeStamp::TimeStamp(TimeStamp& dateSrc)
{ 
	m_timestamp = dateSrc.m_timestamp; 
	m_valid = dateSrc.m_valid; 

	setDefaultFormat();
}

TimeStamp::TimeStamp(VARIANT& varSrc)
{ 
	*this = varSrc; 

	setDefaultFormat();
}

TimeStamp::TimeStamp(DATE dtSrc)
{ 
	m_timestamp = dtSrc; 
	m_valid = valid; 

	setDefaultFormat();
}

TimeStamp::TimeStamp(SYSTEMTIME& systimeSrc)
{ 
	*this = systimeSrc; 

	setDefaultFormat();
}

TimeStamp::TimeStamp(FILETIME& filetimeSrc)
{ 
	*this = filetimeSrc; 

	setDefaultFormat();
}

TimeStamp::TimeStamp(long year, long month, long day,
	           long hour, long minute, long second )
{ 
	setDateTime(year, month, day, hour, minute, second); 

	setDefaultFormat();
}

//////////////////////////////////////////////////////////////////////////////////////
// get current time stamp

void TimeStamp::getCurTimeStamp	()
{
	// get local time
	SYSTEMTIME sysTime;
	::GetLocalTime(&sysTime);

	// convert to variant date
	*this = sysTime;
}

void TimeStamp::getCurTimeStamp ( DATE & timestamp )
{	
	// get local time
	SYSTEMTIME sysTime;
	::GetLocalTime(&sysTime);

	// convert to variant date
	SystemTimeToVariantTime(&sysTime,&timestamp);
}



void TimeStamp::getCurTimeStamp ( string & str )
{
	DATE timestamp;
	getCurTimeStamp(timestamp);

	getTimeStamp(timestamp,str);
}


//////////////////////////////////////////////////////////////////////////////////////
// access methods

long TimeStamp::getYear()
{
	if ( m_valid != valid )
		return -1;

	SYSTEMTIME sysTime;
	if ( VariantTimeToSystemTime(m_timestamp,&sysTime) )
		return (long) sysTime.wYear;
	else
		return -1;
}

long TimeStamp::getMonth()
{
	if ( m_valid != valid )
		return -1;

	SYSTEMTIME sysTime;
	if ( VariantTimeToSystemTime(m_timestamp,&sysTime) )
		return (long) sysTime.wMonth;
	else
		return -1;
}

long TimeStamp::getDay()
{
	if ( m_valid != valid )
		return -1;

	SYSTEMTIME sysTime;
	if ( VariantTimeToSystemTime(m_timestamp,&sysTime) )
		return (long) sysTime.wDay;
	else
		return -1;
}

long TimeStamp::getHour()
{
	if ( m_valid != valid )
		return -1;

	SYSTEMTIME sysTime;
	if ( VariantTimeToSystemTime(m_timestamp,&sysTime) )
		return (long) sysTime.wHour;
	else
		return -1;
}

long TimeStamp::getMinute()
{
	if ( m_valid != valid )
		return -1;

	SYSTEMTIME sysTime;
	if ( VariantTimeToSystemTime(m_timestamp,&sysTime) )
		return (long) sysTime.wMinute;
	else
		return -1;
}

long TimeStamp::getSecond()
{
	if ( m_valid != valid )
		return -1;

	SYSTEMTIME sysTime;
	if ( VariantTimeToSystemTime(m_timestamp,&sysTime) )
		return (long) sysTime.wSecond;
	else
		return -1;
}

long TimeStamp::getDayOfWeek()
{
	if ( m_valid != valid )
		return -1;

	SYSTEMTIME sysTime;
	if ( VariantTimeToSystemTime(m_timestamp,&sysTime) )
		return (long) sysTime.wDay;
	else
		return -1;
}

void TimeStamp::operator=( VARIANT& varSrc )
{
	if (varSrc.vt == VT_DATE)
	{
		m_timestamp = varSrc.date;
		m_valid = valid;
	}
}

void TimeStamp::operator=(DATE dtSrc)
{
	m_timestamp = dtSrc;
	m_valid = valid;
}

void TimeStamp::operator=( SYSTEMTIME & sysTime )
{
	if ( SystemTimeToVariantTime(&sysTime,&m_timestamp) )		
		m_valid = valid;
	else
		m_valid = invalid;
}

void TimeStamp::operator=( FILETIME & fileTime )
{
	m_valid = invalid;

	// convert to sys time then to ole time
	SYSTEMTIME sysTime;
	if ( FileTimeToSystemTime(&fileTime, &sysTime) )
	{
		if ( SystemTimeToVariantTime(&sysTime,&m_timestamp) )		
			m_valid = valid;
	}
}

bool TimeStamp::operator<( const TimeStamp& ts)
{
	// Handle negative dates
	return DoubleFromDate(m_timestamp) < DoubleFromDate(ts.m_timestamp);
}

bool TimeStamp::operator>( const TimeStamp& ts)
{
	// Handle negative dates
	return DoubleFromDate(m_timestamp) > DoubleFromDate(ts.m_timestamp);
}

bool TimeStamp::operator<=( const TimeStamp& ts)
{
	// Handle negative dates
	return DoubleFromDate(m_timestamp) <= DoubleFromDate(ts.m_timestamp);
}

bool TimeStamp::operator>=( const TimeStamp& ts)
{
	// Handle negative dates
	return DoubleFromDate(m_timestamp) >= DoubleFromDate(ts.m_timestamp);
}

TimeStamp TimeStamp::operator+( TimeStamp & ts )
{
	TimeStamp result;

	// Compute the actual date difference by adding underlying dates
	result = DateFromDouble(DoubleFromDate(m_timestamp) + ts.m_timestamp);

	result.isValid();

	return result;
}

TimeStamp TimeStamp::operator-( TimeStamp & ts )
{
	TimeStamp result;


	// Return result (span can't be invalid, so don't check range)
	return DoubleFromDate(m_timestamp) - DoubleFromDate(ts.m_timestamp);
}

void TimeStamp::setDateTime(long year, long month, long day,
	                    long hour, long minute, long second )
{
	// convert to sys time
	SYSTEMTIME sysTime;
	sysTime.wYear = year; 
	sysTime.wMonth = minute; 
	sysTime.wDay = day; 
	sysTime.wHour = hour; 
	sysTime.wMinute = minute; 
	sysTime.wSecond = second; 

	// convert to ole date
	if ( SystemTimeToVariantTime(&sysTime,&m_timestamp) )		
		m_valid = valid;
	else
		m_valid = invalid;
}


bool TimeStamp::isValid ()
{
	// about year 100 to about 9999
	if ( m_timestamp > MAX_DATE || m_timestamp < MIN_DATE ) 
	{
		m_valid = invalid;
		return false;
	}

	else
	{
		m_valid = valid;
		return true;
	}
}


////////////////////////////////////////////////////////////////////
// format methods

void TimeStamp::setDefaultFormat ()
{
	// "h:mm:ss tt"
	string str = "h:mm:ss tt";
	setFormat(str);
}

string TimeStamp::format ( LPTSTR strFormat )
{
	// #DGH
	// need to modify 
	// and check 

	// init sys time struct
	SYSTEMTIME sysTime;
	memset(&sysTime,0,sizeof(sysTime));
	VariantTimeToSystemTime(m_timestamp,&sysTime);

	// set the flags
	DWORD flags = 0;

	// get time
	char str[128];
	DWORD count = 
	GetTimeFormat( LOCALE_USER_DEFAULT, flags, &sysTime, 
		           strFormat, str, 128 );

	if ( count == 0 )
	{
		DWORD error = GetLastError();
		return string("");
	}
	else
	{
		return string( str );
	}
}


void TimeStamp::setFormat ( string & strFormat )
{

	// "h:mm:ss tt"
	if ( strFormat == "h:mm:ss tt" )
	{
		m_standard = true;
	}

	// "hh:mm:ss tt"
	else if ( strFormat == "hh:mm:ss tt" )
	{
		m_standard = true;
	}

	// "H:mm:ss"
	else if ( strFormat == "H:mm:ss" )
	{
		m_standard = false;
	}

	// "HH:mm:ss"
	else if ( strFormat == "HH:mm:ss" )
	{
		m_standard = false;
	}
	else 
		return;

	m_format = strFormat;
}

void TimeStamp::getTimeStamp ( DATE timestamp, string & timestampStr )
{
	TCHAR str[128];

	// init sys time struct
	SYSTEMTIME sysTime;
	VariantTimeToSystemTime(timestamp,&sysTime);

	// get date
	DWORD count =
	GetDateFormat( LOCALE_USER_DEFAULT, 
	               DATE_SHORTDATE, 
				   &sysTime, 
		           NULL, str, 128 );

	if ( count != 0 )
		timestampStr = str;

	// get time
	count = 
	GetTimeFormat( LOCALE_USER_DEFAULT, 
	               0, 
	               &sysTime, 
		           "hh::mm::ss tt", str, 128 );

	if ( count != 0 )
	{
		timestampStr += " ";
		timestampStr += str;
	}
}




bool TimeStamp::getTimeStamp ( string & timestamp, string & format, DWORD flags, LCID lcid)
{
	TCHAR str[128];
	DWORD error;

	bool success = true;

	// init sys time struct
	SYSTEMTIME sysTime;
	VariantTimeToSystemTime(m_timestamp,&sysTime);

	// get time
	DWORD count = 
	GetTimeFormat( lcid, flags, &sysTime, 
		           format.c_str(), str, 128 );

	if ( count == 0 )
	{
		error   = GetLastError();
		success = false;
	}
	else
		timestamp = str;

	// get date
	count =
	GetDateFormat( lcid, flags, &sysTime, 
		           format.c_str(), str, 128 );

	if ( count == 0 )
	{
		error   = GetLastError();
		success = false;
	}
	else
	{
		timestamp += " ";
		timestamp += str;
	}

	return success;
}



bool TimeStamp::getTimeStamp ( string & str, bool showSeconds )
{
	string timeStr;
	getTimeStr( timeStr, false );

	string dateStr;
	getDateStr( dateStr, true );

	stringstream strm;
	strm << dateStr;
	strm << " " << timeStr;

	str = strm.str();
	return true;
}

void TimeStamp::getTimeStr ( string & timeStr, bool showSeconds )
{
	// init sys time struct
	SYSTEMTIME sysTime;
	memset(&sysTime,0,sizeof(sysTime));
	VariantTimeToSystemTime(m_timestamp,&sysTime);

	// get the seconds
	DWORD flags = 0;
	if ( !showSeconds )
	{
		flags |= TIME_NOSECONDS;
		sysTime.wSecond = 0;
	}

	// set military flags
	if ( !m_standard )
		flags |= TIME_FORCE24HOURFORMAT | TIME_NOTIMEMARKER;

	// get time
	char str[128];
	DWORD count = 
	GetTimeFormat( LOCALE_USER_DEFAULT, flags, &sysTime, 
		           m_format.c_str(), str, 128 );

	if ( count == 0 )
	{
		DWORD error = GetLastError();
	}
	else
		timeStr = str;
}

void TimeStamp::getDateStr ( string & dateStr, bool useShortDateFormat )
{
	// init sys time struct
	SYSTEMTIME time;
	memset(&time,0,sizeof(time));

	// get the time
	time.wMonth = getMonth();
	time.wDay = getDay();
	time.wYear = getYear();


	// set flags
	DWORD flags = 0;
	if ( useShortDateFormat )
		flags |= DATE_SHORTDATE;
	else
		flags |= DATE_LONGDATE;

	// get time
	char str[128];

	DWORD count =
	GetDateFormat( LOCALE_USER_DEFAULT, flags, &time, 
		           NULL, str, 128 );

	if ( count == 0 )
	{
		DWORD error = GetLastError();
	}
	else
		dateStr = str;
}


