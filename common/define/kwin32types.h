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

#ifndef _WIN32_TYPES_H
#define _WIN32_TYPES_H

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
#include <time.h>
#include <winsock2.h>
#include <sys/stat.h>
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#else
#define _UNIX
#endif

#ifdef _UNIX
#ifdef SOLARIS_GCC_HACK
#include "/usr/include/sys/types.h"
#else
#include <sys/types.h>
#endif
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/stat.h>
#endif

// add new
typedef void* PVOID;
typedef const void* CPVOID;

typedef char* PCHAR;
typedef const char* CPCHAR;

typedef unsigned char* PBYTE;
typedef const unsigned char* CPBYTE;

// add end

#ifndef WIN32

#include <sys/types.h>
#include <sys/socket.h>

typedef unsigned short      UINT16;

//--from windef.h----------------------------------------
typedef unsigned long	ULONG;
typedef ULONG			*PULONG;
typedef unsigned short	USHORT;
typedef USHORT			*PUSHORT;
typedef unsigned char	UCHAR;
typedef UCHAR			*PUCHAR;
typedef char			*PSZ;
typedef const char		*LPCSTR;
typedef char			*LPSTR;

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

#ifndef CONST
#define CONST               const
#endif

typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef FLOAT               *PFLOAT;
typedef BOOL	            *PBOOL;
typedef BOOL	            *LPBOOL;
typedef BYTE	            *PBYTE;
typedef BYTE	            *LPBYTE;
typedef int	                *PINT;
typedef int		            *LPINT;
typedef WORD	            *PWORD;
typedef WORD	            *LPWORD;
typedef long	            *LPLONG;
typedef DWORD	            *PDWORD;
typedef DWORD	            *LPDWORD;
typedef void	            *LPVOID;
typedef CONST void          *LPCVOID;

typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int		UINT32;
typedef unsigned int        *PUINT;

#define TRUE	1
#define FALSE	0

#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define LOWORD(l)           ((WORD)(l))
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))


//--from wtypes.h----------------------------------------
#define VOID            void

typedef long LONG;
typedef unsigned char UCHAR;
typedef short SHORT;
typedef unsigned short USHORT;
typedef DWORD ULONG;
typedef double DOUBLE;
typedef long long LONGLONG;
typedef long long __int64;
typedef unsigned long long ULONGLONG;


//--custom types ----------------------------------------
typedef LONGLONG __int64;
typedef LONG	 __int32;
typedef WORD	 __int16;

//--from basetsd.h-----------------------------------------------------
//
// The following types are guaranteed to be signed and 32 bits wide.
//

typedef int LONG32, *PLONG32;
typedef int INT32, *PINT32;

//
// The following types are guaranteed to be unsigned and 32 bits wide.
//

typedef unsigned int ULONG32, *PULONG32;
typedef unsigned int DWORD32, *PDWORD32;
typedef unsigned int UINT32, *PUINT32;


typedef __int64 LONG64, *PLONG64;
typedef __int64 INT64, *PINT64;

//
// The following types are guaranteed to be unsigned and 64 bits wide.
//

#ifndef WIN32
#define MAKEWPARAM(l, h)      (WPARAM)MAKELONG(l, h)
#define MAKELPARAM(l, h)      (LPARAM)MAKELONG(l, h)
#define MAKELRESULT(l, h)     (LRESULT)MAKELONG(l, h)
#endif

typedef ULONGLONG ULONG64, *PULONG64;
typedef ULONGLONG DWORD64, *PDWORD64;
typedef ULONGLONG UINT64, *PUINT64;

#define UNREFERENCED_PARAMETER(P)      {   (P) = (P);   }

#define _snprintf snprintf
#define stricmp	  strcasecmp
#define strnicmp  strncasecmp
#define	_atoi64	  atoll

#define __max(a,b)	(a>b?a:b)
#define __min(a,b)  (a<b?a:b)

#define Sleep(ms) usleep(ms*1000)

typedef struct sockaddr SOCKADDR;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr_in *PSOCKADDR_IN;

typedef int SOCKET;

#define INVALID_SOCKET (int)(~0)
#define SOCKET_ERROR            (-1)

//#define AF_INET 2
#ifndef INADDR_ANY
#define INADDR_ANY              (u_long)0x00000000
#define INADDR_LOOPBACK         0x7f000001
#define INADDR_BROADCAST        (u_long)0xffffffff
#define INADDR_NONE             0xffffffff
#endif

#define MAX_PATH				(NAME_MAX+1)

#define E_OUTOFMEMORY			0x8007000E
#define E_INVALIDARG            0x80070057

	#else

	#ifdef WIN32
	#pragma warning( disable : 4786 )
	#endif

	//#include <WINSOCK2.H>
	//#include <windows.h>

	#endif //WIN32

	#ifdef WIN32
	#define INT64_FORMAT	"%I64d"
	#define UINT64_FORMAT	"%I64u"
	#else
	#define INT64_FORMAT	"%lld"
	#define UINT64_FORMAT	"%llu"
	#endif

#endif //_WIN32_TYPES_H
