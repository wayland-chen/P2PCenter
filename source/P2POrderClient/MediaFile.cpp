#include "StdAfx.h"
#include ".\mediafile.h"
#include "misc/MD5.h"

extern HWND g_hMain;

string GetModuleFolderName()
{
	char szFolderName[260];
	GetModuleFileName(NULL, szFolderName, sizeof(szFolderName));
	(_tcsrchr(szFolderName,'\\'))[1] = 0;
	return szFolderName;
}

string CMediaFile::m_sCacheFileName = GetModuleFolderName() + "FilesInfoCache.ini";

bool OutputStringNoMove( const char* pstr, size_t len=0 ) // 输出字符串
{
	HANDLE hStdOutput = GetStdHandle( STD_OUTPUT_HANDLE );
	CONSOLE_SCREEN_BUFFER_INFO bInfo; 
	GetConsoleScreenBufferInfo( hStdOutput, &bInfo ); 
	DWORD n = 0;
	return TRUE==WriteConsoleOutputCharacter( hStdOutput, pstr, len?len:strlen(pstr), bInfo.dwCursorPosition, &n );
}

CMediaFile::CMediaFile(void) : m_llFileSize(0), m_nModifyDate(0)
{
}

CMediaFile::~CMediaFile(void)
{
}

bool CMediaFile::InitFile( string sFilePath, string sFileName )
{
	if ( sFilePath.length() == 0 ) return false;
	m_sFilePath = sFilePath;
	m_sFileName = sFileName;

	HANDLE hFile = ::CreateFile( m_sFilePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( INVALID_HANDLE_VALUE == hFile )	return false;
	CMD5 md5;
	DWORD dwRead = 0;	
	LONGLONG llAlreadyRead = 0;
	LARGE_INTEGER uliSize;
	if ( !GetFileSizeEx( hFile, &uliSize ) ) 
	{
		CloseHandle( hFile );
		return false;
	}

	m_nModifyDate = GetModifyDate( hFile);

	if ( !ReadFromCache() || m_llFileSize != uliSize.QuadPart )
	{
		m_llFileSize = uliSize.QuadPart;
		char szBuffer[81920];
		char szText[256];
		printf( "\nRead file %s progress: ", m_sFilePath.c_str());
		while( ::ReadFile( hFile, szBuffer, sizeof(szBuffer), &dwRead, NULL ) && dwRead > 0 )
		{
			md5.Add( szBuffer, dwRead );
			llAlreadyRead += dwRead;
			sprintf( szText, "%.2d%%", llAlreadyRead * 100 / (DWORD)m_llFileSize);
			OutputStringNoMove( szText);
		}

		if ( m_llFileSize != llAlreadyRead ){
			CKLog::WriteLog( LOG_TYPE_DEBUG, "文件没有读取完整,文件长度: %I64u, 实际读取: %I64u", m_llFileSize, llAlreadyRead );
			CloseHandle( hFile );
			return false;
		}

		MD5 hash;
		md5.Finish();
		md5.GetHash( &hash );
		m_sFileHash = md5.HashToString( &hash ).GetBuffer(0);
		SaveToCache();
	}

	CloseHandle( hFile );

	return true;
}

bool CMediaFile::InitFileForDB( string sFilePath, string sFileName )
{
	if ( sFilePath.length() == 0 ) return false;
	m_sFilePath = sFilePath;
	m_sFileName = sFileName;

	HANDLE hFile = ::CreateFile( m_sFilePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( INVALID_HANDLE_VALUE == hFile )	return false;
	CMD5 md5;
	DWORD dwRead = 0;	
	LONGLONG llAlreadyRead = 0;
	LARGE_INTEGER uliSize;
	if ( !GetFileSizeEx( hFile, &uliSize ) ) 
	{
		CloseHandle( hFile );
		return false;
	}

	m_nModifyDate = GetModifyDate( hFile);
	m_llFileSize = uliSize.QuadPart;

	char szBuffer[81920];
	char szText[256];
	printf( "\nRead file %s progress: ", m_sFilePath.c_str());
	while( ::ReadFile( hFile, szBuffer, sizeof(szBuffer), &dwRead, NULL ) && dwRead > 0 )
	{
		md5.Add( szBuffer, dwRead );
		llAlreadyRead += dwRead;
		sprintf( szText, "%.2d%%", llAlreadyRead * 100 / (DWORD)m_llFileSize);
		OutputStringNoMove( szText);
	}

	if ( m_llFileSize != llAlreadyRead ){
		CKLog::WriteLog( LOG_TYPE_DEBUG, "文件没有读取完整,文件长度: %I64u, 实际读取: %I64u", m_llFileSize, llAlreadyRead );
		CloseHandle( hFile );
		return false;
	}

	MD5 hash;
	md5.Finish();
	md5.GetHash( &hash );
	m_sFileHash = md5.HashToString( &hash ).GetBuffer(0);
	SaveToCache();

	CloseHandle( hFile );

	return true;
}

bool CMediaFile::ReadFromCache()
{
	char szText[256];
	if ( ::GetPrivateProfileString( CreateMD5Hash(m_sFilePath.c_str()), "FileHash", "", szText, sizeof(szText), m_sCacheFileName.c_str() ) == 0 )	return false;
	m_sFileHash = szText;
	MD5 md5;
	if ( !CMD5::HashFromString( m_sFileHash.c_str(), &md5 ) ) return false;

	if ( ::GetPrivateProfileString( CreateMD5Hash(m_sFilePath.c_str()), "FileSize", "", szText, sizeof(szText), m_sCacheFileName.c_str() ) == 0 )	return false;
	m_llFileSize = _atoi64(szText);
	if ( 0 == m_llFileSize )	return false;

	m_nModifyDate = ::GetPrivateProfileInt( CreateMD5Hash(m_sFilePath.c_str()), "ModifyDate", 0, m_sCacheFileName.c_str() );
	if ( 0 == m_nModifyDate )	return false;

	return true;
}

bool CMediaFile::SaveToCache()
{
	if ( !::WritePrivateProfileString( CreateMD5Hash(m_sFilePath.c_str()), "FileHash", 
		m_sFileHash.c_str(), m_sCacheFileName.c_str() ) )	return false;
	char szText[256];
	sprintf(szText, "%I64u", m_llFileSize);
	if ( !::WritePrivateProfileString( CreateMD5Hash(m_sFilePath.c_str()), "FileSize", 
		szText, m_sCacheFileName.c_str() ) )	return false;
	sprintf(szText, "%u", m_nModifyDate);
	if ( !::WritePrivateProfileString( CreateMD5Hash(m_sFilePath.c_str()), "ModifyDate", 
		szText, m_sCacheFileName.c_str() ) )	return false;
	if ( !::WritePrivateProfileString( CreateMD5Hash(m_sFilePath.c_str()), "FilePath", 
		m_sFilePath.c_str(), m_sCacheFileName.c_str() ) )	return false;

	return true;
}

LONGLONG CMediaFile::GetFileSize( string sFilePath)
{
	if ( sFilePath.length() == 0 ) return -1;

	HANDLE hFile = ::CreateFile( sFilePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if ( INVALID_HANDLE_VALUE == hFile )	return -1;

	LARGE_INTEGER uliSize;
	if ( !GetFileSizeEx( hFile, &uliSize ) ) 
	{
		CloseHandle( hFile );
		return -1;
	}

	CloseHandle( hFile );

	return uliSize.QuadPart;
}

unsigned int CMediaFile::GetModifyDate( string sFilePath)
{
	if ( sFilePath.length() == 0 ) return -1;

	HANDLE hFile = ::CreateFile( sFilePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if ( INVALID_HANDLE_VALUE == hFile )	return -1;

	unsigned int nRet = GetModifyDate( hFile);

	CloseHandle( hFile );

	return nRet;
}

unsigned int CMediaFile::GetModifyDate( HANDLE hFile)
{
	if ( INVALID_HANDLE_VALUE == hFile )	return -1;

	FILETIME ft[3];
	if ( !GetFileTime( hFile, &ft[0], &ft[1], &ft[2] ) ) 
	{
		return -1;
	}

	union {
		DWORD dwDate;          // Obsolete field...do not use
		struct {
			WORD w1;
			WORD w2;
		};
	} ModifyDate;

	if ( !FileTimeToDosDateTime( &ft[2], &ModifyDate.w1, &ModifyDate.w2))
		return -1;

	return ModifyDate.dwDate;
}
