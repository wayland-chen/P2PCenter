#pragma once

#include "..\p2pnetwork\ip2pnetwork.h"
#include "MediaFileMgr.h"
#include "io/KFile.h"
#include "thread/KThread.h"
#include "MSSqlDB.h"

#define WM_SHOW_LOG WM_USER + 100

class CClientApp :
	public IP2PNetworkNotify, public CKThread
{
public:
	CClientApp(void);
	virtual ~CClientApp(void);

	virtual void OnConnect();
	virtual void OnDisConnect();

	bool Connect();
	void Release();

	bool EnumAllFiles(string sFolder);
	bool PublishFilesFromDB( CMSSqlDB& db);

protected:
	virtual void ThreadProcMain(void);

private:
	DWORD PublishFiles();
	static void WriteLog( LOG_TYPE type, char* pArrFmt, ... );

public:
	IP2PNetwork* m_pP2PNetwork;

private:
	CMediaFileMgr m_MediaFileMgr;
	map<string, DWORD> m_AlreadyPublishs;
	bool m_bWantStop;
	CKEvent m_evThreadWait;
};
