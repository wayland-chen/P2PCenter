#ifndef _TCPSERVER_H
#define _TCPSERVER_H

#ifdef TCPSERVER_EXPORTS
#define TCPSERVER_API __declspec(dllexport)
#else
#define TCPSERVER_API __declspec(dllimport)
#endif


class ITcpServerNotify
{
public:
	virtual void OnAccept(unsigned int nLinkNo) = 0;
	virtual void OnClosed(unsigned int nLinkNo) = 0;
	virtual void OnRecvData(const char* pData, int nLen, unsigned int nLinkNo) = 0;
};

class ITcpServer
{
public:
	virtual bool Create(unsigned short usPort) = 0;
	virtual void Destroy(void) = 0;
	virtual bool SendData(const char *pData, int nLen, unsigned int nLinkNo) = 0;
	virtual bool CloseLink(unsigned int nLinkNo) = 0;
	virtual bool SetExtData(unsigned int nLinkNo, unsigned int nExtData) = 0;
	virtual unsigned int GetExtData(unsigned int nLinkNo) = 0;
};

TCPSERVER_API bool InitSocket();
TCPSERVER_API void UninitSocket();
TCPSERVER_API ITcpServer* CreateTcpServer(ITcpServerNotify* pNotify, unsigned short usMaxLinkCount);

#endif