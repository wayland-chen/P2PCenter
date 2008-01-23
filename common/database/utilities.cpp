/*
 *				FILENAME	: *
 *				AUTHOR		: Kevin(fuwenke@gmail.com)
 *				DATE		: 01/23/2008
 *				PURPOSE		: A series of utilities,such as convert string type to date type,
 *							  convert stringtype to time type and so on
 */


#include "utilities.h"
#include <stdlib.h>
#include <string>
using namespace std;

namespace DB_TOOLKIT
{

	//the format of szDate should like yyyy-mm-dd
	DBDATE  StrToDate(const char* szDate)
	{
		DBDATE date;
		char num[5];
		unsigned short nTemp = 0;
		while(*szDate==' ') 
		{
			szDate++;
		}
		num[0] =*szDate++; 
		num[1] =*szDate++; 
		num[2] =*szDate++; 
		num[3] =*szDate++; 
		num[4] = 0;
		nTemp = (unsigned short)strtol(num,0,10);
		date.year = (nTemp<0)?0:nTemp;
		if (*szDate == '-') 
		{
			szDate++;
		}
		num[0] = *szDate++; 
		num[1] = *szDate++; 
		num[2] = 0;
		nTemp = (unsigned short)strtol(num,0,10);
		date.month = (nTemp>12 || nTemp<0)?1:nTemp;
		if (*szDate == '-') 
		{
			szDate++;
		}
		num[0] = *szDate++; 
		num[1] = *szDate++; 
		num[2] = 0;
		nTemp = (unsigned short)strtol(num,0,10);
		date.day = (nTemp>31 || nTemp<0)?1:nTemp;
		return date;
	}

	//the format of szTime should like hh:mm:ss
	DBTIME  StrToTime(const char* szTime)
	{
		DBTIME time;
		char num[5];
		unsigned char nTemp = 0;
		while(*szTime==' ')
		{
			szTime++;
		}
		num[0] = *szTime++; 
		num[1] = *szTime++; 
		num[2] = 0;
		nTemp = (unsigned char)strtol(num,0,10);
		time.hour = (nTemp>23 || nTemp<0)?0:nTemp;
		if (*szTime == ':') 
		{
			szTime++;
		}
		num[0] = *szTime++; 
		num[1] = *szTime++; 
		num[2] = 0;
		nTemp = (unsigned char)strtol(num,0,10);
		time.minute = (nTemp>59 || nTemp<0)?0:nTemp;
		if (*szTime == ':') 
		{
			szTime++;
		}
		num[0] = *szTime++; 
		num[1] = *szTime++; 
		num[2] = 0;
		nTemp = (unsigned char)strtol(num,0,10);
		time.second = (nTemp>59 || nTemp<0)?0:nTemp;
		return time;
	}


	//the format of szDateTime should like yyyy-mm-dd hh:mm:ss
	DBDATETIME StrToDateTime(const char* szDateTime)
	{
		DBDATETIME dt;
		char num[5];
		unsigned char nTemp = 0;
		while(*szDateTime==' ')
		{
			szDateTime++;
		}
		num[0] =*szDateTime++; 
		num[1] =*szDateTime++; 
		num[2] =*szDateTime++; 
		num[3] =*szDateTime++; 
		num[4] = 0;
		nTemp = (unsigned char)strtol(num,0,10);
		dt.year = (nTemp<0)?0:nTemp;
		if (*szDateTime == '-') 
		{
			szDateTime++;
		}
		num[0] = *szDateTime++; 
		num[1] = *szDateTime++; 
		num[2] = 0;
		nTemp = (unsigned char)strtol(num,0,10);
		dt.month = (nTemp>12 || nTemp<0)?1:nTemp;
		if (*szDateTime == '-') 
		{
			szDateTime++;
		}
		num[0] = *szDateTime++; 
		num[1] = *szDateTime++; 
		num[2] = 0;
		nTemp = (unsigned char)strtol(num,0,10);
		dt.day = (nTemp>31 || nTemp<0)?1:nTemp;

		while(*szDateTime==' ')
		{
			szDateTime++;
		}
		num[0] = *szDateTime++; 
		num[1] = *szDateTime++; 
		num[2] = 0;
		nTemp = (unsigned char)strtol(num,0,10);
		dt.hour = (nTemp>23 || nTemp<0)?0:nTemp;
		if (*szDateTime == ':') 
		{
			szDateTime++;
		}
		num[0] = *szDateTime++; 
		num[1] = *szDateTime++; 
		num[2] = 0;
		nTemp = (unsigned char)strtol(num,0,10);
		dt.minute = (nTemp>59 || nTemp<0)?0:nTemp;
		if (*szDateTime == ':') 
		{
			szDateTime++;
		}
		num[0] = *szDateTime++; 
		num[1] = *szDateTime++; 
		num[2] = 0;
		nTemp = (unsigned char)strtol(num,0,10);
		dt.second = (nTemp>59 || nTemp<0)?0:nTemp;
	    
		return dt;
	}

	DBDATE BlobToDate(DBBLOB blob)
	{

		DBDATE date;
		date.year = 0;
		date.month = 1;
		date.day = 1;
		int num[4];
		unsigned char year=0,month=0,day=0;
		if(BLOB_LENGTH(blob)>4)
		{
			num[0] = ((unsigned char*)BLOB_TO_REALDATA(blob))[0];
			num[1] = ((unsigned char*)BLOB_TO_REALDATA(blob))[1];
			num[2] = ((unsigned char*)BLOB_TO_REALDATA(blob))[2];
			num[3] = ((unsigned char*)BLOB_TO_REALDATA(blob))[3];
			year = num[0]*1000+num[1]*100+num[2]*10+num[3];
			date.year = (year>9999)?0:year;
		}
		else
		{
			for(int i=0;i<BLOB_LENGTH(blob);i++)
			{
				date.year *= 10;
				date.year += ((unsigned char*)BLOB_TO_REALDATA(blob))[i];
			}
			return date;
		}
	    
		if(BLOB_LENGTH(blob)==5)
		{
			month = ((unsigned char*)BLOB_TO_REALDATA(blob))[4];
			date.month = (month>12)?1:month;
			return date;
		}
		else
		{
			month = ((unsigned char*)BLOB_TO_REALDATA(blob))[4]*10+
					((unsigned char*)BLOB_TO_REALDATA(blob))[5];
			date.month = (month>12)?1:month;
		}
	    
		if(BLOB_LENGTH(blob)==7)
		{
			day = ((unsigned char*)BLOB_TO_REALDATA(blob))[6];
			date.day = (day>31)?1:day;
			return date;
		}
		else
		{
			day = ((unsigned char*)BLOB_TO_REALDATA(blob))[6]*10+
				((unsigned char*)BLOB_TO_REALDATA(blob))[7];
			date.day = (day>31)?1:day;
		}

		return date;
	}

	DBTIME BlobToTime(DBBLOB blob)
	{
		DBTIME time;
		memset(&time,0,sizeof(DBTIME));
		int num[4];
		unsigned char hour=0,minute=0,second=0;
		if(BLOB_LENGTH(blob)>2)
		{
			num[0] = ((unsigned char*)BLOB_TO_REALDATA(blob))[0];
			num[1] = ((unsigned char*)BLOB_TO_REALDATA(blob))[1];
			hour = num[0]*10+num[1];
			time.hour = (hour>23)?0:hour;
		}
		else
		{
			for(int i=0;i<BLOB_LENGTH(blob);i++)
			{
				time.hour *= 10;
				time.hour += ((unsigned char*)BLOB_TO_REALDATA(blob))[i];
			}
			return time;
		}
	    
		if(BLOB_LENGTH(blob)==3)
		{
			minute = ((unsigned char*)BLOB_TO_REALDATA(blob))[2];
			time.minute = (minute>59)?0:minute;
			return time;
		}
		else
		{
			minute = ((unsigned char*)BLOB_TO_REALDATA(blob))[2]*10+
					((unsigned char*)BLOB_TO_REALDATA(blob))[3];
			time.minute = (minute>59)?0:minute;
		}
	    
		if(BLOB_LENGTH(blob)==5)
		{
			second = ((unsigned char*)BLOB_TO_REALDATA(blob))[4];
			time.second = (second>59)?0:second;
			return time;
		}
		else
		{
			second = ((unsigned char*)BLOB_TO_REALDATA(blob))[4]*10+
					((unsigned char*)BLOB_TO_REALDATA(blob))[5];
			time.second = (second>59)?1:second;
		}

		return time;
	}

	DBDATETIME BlobToDateTime(DBBLOB blob)
	{
		DBDATE date = BlobToDate(blob);
		DBTIME time;
		memset(&time,0,sizeof(DBTIME));
	    
		if(BLOB_LENGTH(blob) > 8)
		{
			DBBLOB temp = new unsigned char[BLOB_LENGTH(blob)-8+sizeof(int)];
			BLOB_LENGTH(temp) = BLOB_LENGTH(blob)-8;
			memcpy(BLOB_TO_REALDATA(temp),BLOB_TO_REALDATA(blob)+8,BLOB_LENGTH(blob)-8);
			time = BlobToTime(temp);
			delete [] temp;
		}
	    
		DBDATETIME dt;
		dt.year = date.year;
		dt.month = date.month;
		dt.day = date.day;
		dt.hour = time.hour;
		dt.minute = time.minute;
		dt.second = time.second;

		return dt;
	}

	
	DBGUID  StrToGUID(const char* szGuid)
	{
		DBGUID guid;
		memset(&guid,0,sizeof(DBGUID));
	    
		char s[36];
		while(*szGuid==' ')
		{
			szGuid++;
		}
		if(strlen(szGuid)<36)//64DA6213-9E99-437a-B77D-D1E41A3E4E85³¤¶ÈÎª36
		{
			return guid;
		}

		int i=0;
		while(*szGuid!='-')
		{
			s[i++] = *szGuid++;
		}
		s[i] = 0;
		guid.Data1 = strtol(s,NULL,16);
		szGuid++;
	    
		i=0;
		while(*szGuid!='-')
		{
			s[i++] = *szGuid++;
		}
		s[i] = 0;
		guid.Data2 = (unsigned short)strtol(s,NULL,16);
		szGuid++;

		i=0;
		while(*szGuid!='-')
		{
			s[i++] = *szGuid++;
		}
		s[i] = 0;
		guid.Data3 = (unsigned short)strtol(s,NULL,16);
		szGuid++;

		i=0;
		while(*szGuid!='-')
		{
			s[i++] = *szGuid++;
		}
		s[i] = 0;
		guid.Data4 = (unsigned short)strtol(s,NULL,16);
		szGuid++;

		for(i=0;i<6;i++)
		{
			s[0] = *szGuid++;
			s[1] = *szGuid++;
			s[2] = 0;
			guid.Data[i] = (unsigned char)strtol(s,NULL,16);
		}
	    
		return guid;
	}

};

