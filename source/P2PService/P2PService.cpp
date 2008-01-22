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

// P2PService.cpp : WinMain 的实现

#include "stdafx.h"
#include "resource.h"
#include "P2PService.h"
#include "ServerSocket.h"
#include "ServerSocketHandler.h"
#include "HttpServer.h"
#include "HttpRequestProcess.h"
#include <conio.h>
#include <locale>
using namespace std;
#include "network/KSocket.h"
#include "SocketClientMgr.h"
#include "log/KLog.h"

CSocketClientMgr g_socketClientMgr;
HttpServer g_server;

#define CHECK_SERVER_TIMER 2400

class CP2PServiceModule : 
	public CAtlServiceModuleT< CP2PServiceModule, IDS_SERVICENAME >
{
public :
	DECLARE_LIBID(LIBID_P2PServiceLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_P2PSERVICE, "{13AFCFEF-6DE7-4C05-AAB6-8AE7543011A5}")
	CP2PServiceModule(){};
	HRESULT InitializeSecurity() throw()
	{
		// TODO : 调用 CoInitializeSecurity 并为服务提供适当的 
		// 安全设置
		// 建议 - PKT 级别的身份验证、
		// RPC_C_IMP_LEVEL_IDENTIFY 的模拟级别
		// 以及适当的非空安全说明符。

		return S_OK;
	}  

	HRESULT PreMessageLoop(int   nShowCmd);
	void RunMessageLoop();
	HRESULT PostMessageLoop();

	bool RegisterApp();

	void OnCheckTimer( UINT uID, UINT uTime );
};

CP2PServiceModule _AtlModule;

BOOL ChangeServiceStartType( const char* pServiceName, DWORD dwStartType)
{
	BOOL bRet = FALSE;

	SC_HANDLE hSCM = OpenSCManager( 0, 0, SC_MANAGER_ALL_ACCESS);

	if (hSCM)
	{
		SC_HANDLE hService = OpenService( hSCM, pServiceName, SERVICE_ALL_ACCESS);

		if (hService)
		{
			bRet = ChangeServiceConfig(hService,
				SERVICE_NO_CHANGE, dwStartType, SERVICE_NO_CHANGE, NULL, NULL, NULL, NULL, NULL, NULL, NULL );
		}

		CloseServiceHandle(hSCM);
	}

	return bRet;
}
//
extern "C" int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, 
                                LPTSTR lpCmdLine, int nShowCmd)
{
#ifdef _DEBUG
	_AtlModule.Run( SW_SHOW);
#else
	return _AtlModule.WinMain(nShowCmd);
#endif
}

HRESULT CP2PServiceModule::PreMessageLoop(int   nShowCmd)   
{    
	__super::PreMessageLoop( nShowCmd);
	return S_OK;
}  

void CP2PServiceModule::RunMessageLoop()   
{    
#if defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	CKLog::SetDebugType( LOG_TYPE_DEBUG);
#else
	CKLog::SetDebugType( LOG_TYPE_WARNING);
#endif

	char szServiceName[256];
	::LoadString( NULL, IDS_SERVICENAME, szServiceName, sizeof(szServiceName));
	ChangeServiceStartType( szServiceName, SERVICE_AUTO_START);

	TCHAR szClient_Mutex[] = _T("PPVOD_MUTEX_K_1.0");
	if ( ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, szClient_Mutex) ) return;
	::CreateMutex(0, FALSE, szClient_Mutex);

	g_socketClientMgr.Connect();

	bool bListen = false;
	UINT uPort = 81;

	for ( int i = 1; i < 20; i ++ )
	{
		g_server.setPort( uPort);
		if ( g_server.create( string("P2PLocalService"), string("C:\\") ) )
		{
			bListen = true;
			break;
		}
		uPort += i;
	}

	HttpResponse::setServer( g_server );

	if ( bListen )
	{
		// 设置共享内存监听端口为 uPort
		CRegKey reg;
		LPCTSTR lpszKey = "SOFTWARE\\P2PService_K";

		if ( reg.Open( HKEY_LOCAL_MACHINE, lpszKey ) == ERROR_SUCCESS )
		{
			TCHAR szAppPath[MAX_PATH];
			ULONG ulLen = sizeof( szAppPath );
			if ( reg.SetDWORDValue( "ListenPort", uPort ) == ERROR_SUCCESS )
			{
			}
			reg.Close();
		}

		SetTimer( NULL, CHECK_SERVER_TIMER, 1000, NULL);

		MSG msg;
		while( GetMessage(&msg, NULL, 0, 0 )) 
		{
			switch( msg.message) {
			case WM_TIMER:
				OnCheckTimer( 0, 0);
				break;
			default:
				break;
			}
			TranslateMessage(&msg); 
			DispatchMessage(&msg); 
		}

		KillTimer( NULL, CHECK_SERVER_TIMER);
	}

	g_server.stop();

	SetServiceStatus(SERVICE_STOPPED);
}  

HRESULT CP2PServiceModule::PostMessageLoop()   
{   
	return __super::PostMessageLoop();   
}

bool CP2PServiceModule::RegisterApp()
{
	TCHAR szFileName[MAX_PATH];
	GetModuleFileName(_AtlBaseModule.m_hInstResource, szFileName, sizeof(szFileName));

	CRegKey reg;
	LPCTSTR lpszKey = "SOFTWARE\\P2PService_K";
	if ( reg.Create( HKEY_LOCAL_MACHINE, lpszKey ) == ERROR_SUCCESS )
	{
		reg.SetStringValue( "AppPath", szFileName );
		reg.Close();
	}

	if ( reg.Open( HKEY_LOCAL_MACHINE, lpszKey ) == ERROR_SUCCESS )
	{
		TCHAR szAppPath[MAX_PATH];
		ULONG ulLen = sizeof( szAppPath );
		if ( reg.QueryStringValue( "AppPath", szAppPath, &ulLen ) == ERROR_SUCCESS )
		{
			reg.Close();
			return true;
		}
		reg.Close();
	}

	return false;
}

void CP2PServiceModule::OnCheckTimer( UINT uID, UINT uTime )
{
	if ( !g_socketClientMgr.IsConnected())
	{
		CKLog::WriteLog( LOG_TYPE_DEBUG, "reconnect server.%d", GetTickCount());
		g_server.stop();
		g_socketClientMgr.Release();
		if ( g_socketClientMgr.Connect())
			g_server.start();
	}
}



