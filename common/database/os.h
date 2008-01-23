/*
 *				FILENAME	: *
 *				AUTHOR		: Kevin(fuwenke@gmail.com)
 *				DATE		: 01/23/2008
 */

#ifndef     __OS_INDEPENDENT_H__
#define     __OS_INDEPENDENT_H__

namespace DB_TOOLKIT
{


	#if (defined(WIN32) || defined(WIN64) || defined(WINDOWS))
	#define  OS_IS_MSWINDOWS
	#define     UInt64      unsigned __int64
	#define     Int64       __int64
	#else  //LINUX
	#define     UInt64      unsigned long long 
	#define     Int64       long long
	#endif

	#if defined(OS_IS_MSWINDOWS)
	#define strtoll(str,endptr,base) _atoi64((str))
	#endif
	  
	//使用异常是否的编译开关
	#ifdef       _THROW_EXCEPTION
	#define      RETURN_WITH_VALUE(exception,ret) \
				throw exception
	#define      RETURN_WITH_VOID(exception) \
				throw exception
	#else
	#define      RETURN_WITH_VALUE(exception,ret) \
				return ret
	#define      RETURN_WITH_VOID(exception) \
				return
	#endif
};

#endif
