#pragma once

#include "misc/MD5.h"

class CMediaFile
{
public:
	CMediaFile(void);
	virtual ~CMediaFile(void);

	bool InitFile( string sFilePath, string sFileName );
	bool InitFileForDB( string sFilePath, string sFileName );
	bool ReadFromCache();
	bool SaveToCache();

	//return -1ÎªÊ§°Ü;
	static LONGLONG GetFileSize( string sFilePath);
	static unsigned int GetModifyDate( HANDLE hFile);
	static unsigned int GetModifyDate( string sFilePath);

	string				m_sFilePath;
	string				m_sFileName;
	string				m_sFileHash;
	string				m_sNodeName;
	LONGLONG			m_llFileSize;
	unsigned int		m_nModifyDate;
	static string		m_sCacheFileName;
};
