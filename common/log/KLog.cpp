
#include "KLog.h"

LOG_TYPE CKLog::m_type = LOG_TYPE_DEBUG;
CKFile CKLog::m_file;

CKLog::CKLog()
{
}

void CKLog::SetDebugType( LOG_TYPE type )
{
	m_type = type;
}

#if _MSC_VER >= 1300    // for VC 7.0
// from ATL 7.0 sources
#ifndef _delayimp_h
extern "C" IMAGE_DOS_HEADER __ImageBase;
#endif
#endif


HMODULE GetCurrentModule()
{
#if _MSC_VER < 1300    // earlier than .NET compiler (VC 6.0)

	MEMORY_BASIC_INFORMATION mbi;
	static int dummy;
	VirtualQuery( &dummy, &mbi, sizeof(mbi) );

	return reinterpret_cast<HMODULE>(mbi.AllocationBase);

#else    // VC 7.0
	return reinterpret_cast<HMODULE>(&__ImageBase);
#endif
}


bool CKLog::WriteLog( LOG_TYPE type, char* pArrFmt, ... )
{
	if ( type >= m_type )
	{
		struct tm *local;
		time_t t =time(NULL);
		local=localtime(&t);

		va_list argList = {0};
		char szBuf[4096] = {0};
		sprintf( szBuf, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d ", 
			local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec );

		va_start(argList, pArrFmt);
		vsprintf(&szBuf[strlen(szBuf)], pArrFmt, argList);
		va_end(argList);
		printf(szBuf);
		printf("\n");

#ifdef _UNIX
		fputs( szBuf, stderr );
		fputs( "\n", stderr );
#endif
#ifdef _WIN32
		::OutputDebugString( szBuf );
		::OutputDebugString( "\n" );
#endif
		if ( !m_file.IsOpen())
		{
			char szFolderName[MAX_PATH];
			GetModuleFileName(GetCurrentModule(), szFolderName, sizeof(szFolderName));
			(strrchr(szFolderName,'\\'))[1] = 0;

			strcat( szFolderName, "log");

			CreateDirectory( szFolderName, NULL);
			char szFilename[260];
			sprintf( szFilename, "%s\\%.4d-%.2d-%.2d %.2d.%.2d.%.2d.log", szFolderName,
				local->tm_year + 1900, local->tm_mon, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);
			m_file.Create( CKString(szFilename));
		}

		if ( !m_file.IsOpen())	return false;
		m_file.Write( szBuf, strlen(szBuf));
		m_file.Write( "\r\n", 2);
	}

	return true;
};