#pragma once

class CConfig
{
public:
	CConfig(void);
	virtual ~CConfig(void);

	bool ReadAll( const char* szFilename );

public:
	string m_sServerIP;
	unsigned short m_usServerTcpPort;
	unsigned short m_usServerUdpPort;
};
