/*
 *				FILENAME	: *
 *				AUTHOR		: Kevin(fuwenke@gmail.com)
 *				DATE		: 01/23/2008
 */

#ifndef     __DATA_TYPE_H__
#define     __DATA_TYPE_H__

namespace DB_TOOLKIT
{
	typedef unsigned char* DBBLOB;
	typedef char* DBBSTR;
	typedef unsigned int DBVARTYPE;


	//definition of GUID type
	typedef struct _DBGUID
	{
		unsigned long   Data1;
		unsigned short  Data2;
		unsigned short  Data3;
		unsigned short  Data4;
		unsigned char   Data[6];
	}DBGUID;

	//definition of DATE type
	union DBDATE
	{
		unsigned long _dummy;
		struct
		{
			unsigned short year;
			unsigned char  month;
			unsigned char  day;
		};
	    
		bool operator > (const DBDATE& other)
		{ return _dummy > other._dummy; };
		bool operator == (const DBDATE& other)
		{ return _dummy == other._dummy; };
		bool operator >= (const DBDATE& other)
		{ return _dummy >= other._dummy; };
		bool operator < (const DBDATE& other)
		{ return _dummy < other._dummy; };
		bool operator <= (const DBDATE& other)
		{ return _dummy <= other._dummy; };
		bool operator != (const DBDATE& other)
		{ return _dummy != other._dummy; };
	    
	};

	//definition of TIME type
	union DBTIME
	{
		unsigned long _dummy;
		struct
		{
			unsigned char hour;
			unsigned char minute;
			unsigned char second;
			unsigned char pad;
		};
	    
		bool operator > (const DBTIME& other)
		{ return _dummy > other._dummy; };
		bool operator == (const DBTIME& other)
		{ return _dummy == other._dummy; };
		bool operator >= (const DBTIME& other)
		{ return _dummy >= other._dummy; };
		bool operator < (const DBTIME& other)
		{ return _dummy < other._dummy; };
		bool operator <= (const DBTIME& other)
		{ return _dummy <= other._dummy; };
		bool operator != (const DBTIME& other)
		{ return _dummy != other._dummy; };
	};

	//definition of DATETIME type
	union DBDATETIME
	{
		double  _dummy;
		struct 
		{
			unsigned short  year;
			unsigned char   month;
			unsigned char   day;
			unsigned char   hour;
			unsigned char   minute;
			unsigned char   second;
			unsigned char   pad;
		};

	    
		bool operator > (const DBDATETIME& other)
		{ return _dummy > other._dummy; };
		bool operator == (const DBDATETIME& other)
		{ return _dummy == other._dummy; };
		bool operator >= (const DBDATETIME& other)
		{ return _dummy >= other._dummy; };
		bool operator < (const DBDATETIME& other)
		{ return _dummy < other._dummy; };
		bool operator <= (const DBDATETIME& other)
		{ return _dummy <= other._dummy; };
		bool operator != (const DBDATETIME& other)
		{ return _dummy != other._dummy; };
	};
};
#endif
