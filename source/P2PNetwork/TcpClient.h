#ifndef _TCPCLIENT_H
#define _TCPCLIENT_H

#ifdef TCPCLIENT_EXPORTS
#define TCPCLIENT_API __declspec(dllexport)
#else
#define TCPCLIENT_API __declspec(dllimport)
#endif

class ITcpClientNotify
{
public:
	virtual void OnReceivedData( const char* pData,int nLen) = 0;
	virtual void OnConnected(void) = 0;
	virtual void OnConnectFailed(void) = 0;
	virtual void OnDisconnected(void) = 0;
};

class ITcpClient
{
public:
	virtual bool ConnectServer( const char* pServerAddr, unsigned short usPort) = 0;
	virtual bool SendData( const char* pData, int nLen) = 0;
	virtual void Disconnect(void) = 0;
	virtual void ReleaseConnection() = 0;
};

TCPCLIENT_API ITcpClient* CreateTcpClient( ITcpClientNotify* pNotify);
TCPCLIENT_API bool InitSocket();
TCPCLIENT_API void UninitSocket();
#endif