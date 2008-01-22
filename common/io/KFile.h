/********************************************************************
	created:	2008/01/11
	created:	11:1:2008   15:49
	author:		Kevin
	
	purpose:	

	email:		kan8888.com@163.com
	msn:		fuwenke@gmail.com
	QQ:		799383417
	site:		opensource.kan8888.com
*********************************************************************/

#ifndef _KFILE_H
#define _KFILE_H

#include "../define/kwin32types.h"
#include "../misc/KString.h"
#include "KStream.h"

#ifdef _WIN32
typedef HANDLE fileobj_t;
//#define INVALID_FILE NULL      
#define INVALID_FILE INVALID_HANDLE_VALUE      
#else
typedef int fileobj_t;
#define INVALID_FILE -1
#endif

class CKFile : public CKStream
{
public:
    CKFile( void );
    CKFile( CKStreamNotify* pNotify );
    virtual ~CKFile( void );

    virtual bool IsOpen( void );
    virtual void Close( void );

	bool Open( const CKString& strFile );
	bool Create( const CKString& strFile );

    bool Stat( struct stat* pst );
    size_t Read( PVOID buf, size_t len );
    size_t Write( CPVOID buf, size_t len );
	
	bool Seek( size_t pos, unsigned long ulFlag = FILE_BEGIN  );
	LONGLONG Size();

    static bool Stat( const CKString& strFile, struct stat* pst );
	static bool DeleteFile( const CKString& strFile );

protected:
    fileobj_t   m_file;
};

#endif //ndef _KFILE_H
