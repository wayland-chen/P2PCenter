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

#ifndef __KLOG_H_
#define __KLOG_H_

#include <time.h>
#include "../io/KFile.h"

// 未作特别说明的，单位均为 毫秒

typedef enum LOG_TYPE
{
	LOG_TYPE_DEBUG = 0,
	LOG_TYPE_WARNING,
	LOG_TYPE_ERROR,
	LOG_TYPE_FORCE
};

class CKLog
{
public:
	CKLog();
	static void SetDebugType( LOG_TYPE type );
	static bool WriteLog( LOG_TYPE type, char* pArrFmt, ... );

public:
	static CKFile m_file;
	static LOG_TYPE m_type;
};

#endif // __KLOG_H_
