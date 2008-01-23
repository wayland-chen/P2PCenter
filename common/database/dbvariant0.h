/*
 *				FILENAME	: *
 *				AUTHOR		: Kevin(fuwenke@gmail.com)
 *				DATE		: 01/23/2008
 */

#ifndef     __DB_VARIANT_0_H__
#define     __DB_VARIANT_0_H__

namespace DB_TOOLKIT
{

	#define     VT_DBI1           0x00
	#define     VT_DBUI1          0x01
	#define     VT_DBI2           0x02
	#define     VT_DBUI2          0x03
	#define     VT_DBI4           0x04
	#define     VT_DBUI4          0x05
	#define     VT_DBI8           0x06
	#define     VT_DBUI8          0x07
	#define     VT_DBSINGLE       0x08
	#define     VT_DBDOUBLE       0x09
	#define     VT_DBGUID         0x0A
	#define     VT_DBDATE         0x0B
	#define     VT_DBTIME         0x0C
	#define     VT_DBDATETIME     0x0D
	#define     VT_DBBOOLEAN      0x0E
	#define     VT_DBBSTR         0x0F
	#define     VT_DBBLOB         0x10
	#define     VT_DBEMPTY        0xFE
	#define     VT_DBNULL         0xFF




	//------------------DBBSTR(DBBLOB)-----------------
	/* 
	-----------------------------------------------
	| length(sizeof(int))  |data................. |
	-----------------------------------------------
	*/
	#define BSTR_LENGTH(bstr)           *(int*)(bstr)
	#define BLOB_LENGTH                 BSTR_LENGTH
	#define BSTR_TO_CCHAR(bstr)         static_cast<const char*>((bstr)+sizeof(int))
	#define BSTR_TO_CHAR(bstr)          static_cast<char*>((bstr)+sizeof(int))
	#define BLOB_TO_CREALDATA(blob)     static_cast<const unsigned char*>((blob)+sizeof(int))
	#define BLOB_TO_REALDATA(blob)      static_cast<unsigned char*>((blob)+sizeof(int))

};

#endif
