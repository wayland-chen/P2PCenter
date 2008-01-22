// UpnpMFC.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "UpnpApp.h"
#include "Upnp.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

void CommandProcess();

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	if ( argc < 6 )
	{
		printf("Example: *.exe mappingname tcpinternalport tcpexternalport udpinternalport udpexternalport\n");
		return -1;
	}

	char* mappingname = argv[1];
	WORD tcpinternalport = atoi(argv[2]);
	WORD tcpexternalport = atoi(argv[3]);
	WORD udpinternalport = atoi(argv[4]);
	WORD udpexternalport = atoi(argv[5]);
	//WORD tcpexternalport = 2345;
	//WORD tcpinternalport = 2345;
	//WORD udpexternalport = 2007;
	//WORD udppinternalport = 2007;

	// 初始化 MFC 并在失败时显示错误
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: 更改错误代码以符合您的需要
		_tprintf(_T("致命错误: MFC 初始化失败\n"));
		nRetCode = 1;
	}
	else
	{
		WSADATA wsaData;
		WSAStartup( MAKEWORD( 1, 1 ), &wsaData );

		{
			MyUPnP pnp;
			MyUPnP::UPNPNAT_MAPPING tcpmapping;
			tcpmapping.description = "TCP_" + CString(mappingname);
			tcpmapping.internalPort = tcpinternalport;
			tcpmapping.externalPort = tcpexternalport;
			tcpmapping.protocol = MyUPnP::UNAT_TCP;
			if ( MyUPnP::UNAT_OK == pnp.AddNATPortMapping(&tcpmapping, true) )
			{
				printf("mapping %s tcp port succeed. internalport = %d, externalport = %d\n", tcpmapping.description, tcpinternalport, tcpexternalport);
			}
			else
			{
				printf("mapping %s tcp port failed. internalport = %d, externalport = %d\n", tcpmapping.description, tcpinternalport, tcpexternalport);
			}



			MyUPnP::UPNPNAT_MAPPING udpmapping;
			udpmapping.description = "UDP_" + CString(mappingname);
			udpmapping.internalPort = udpinternalport;
			udpmapping.externalPort = udpexternalport;
			udpmapping.protocol = MyUPnP::UNAT_UDP;
			if ( MyUPnP::UNAT_OK == pnp.AddNATPortMapping(&udpmapping, true) )
			{
				printf("mapping %s udp port succeed. internalport = %d, externalport = %d\n", udpmapping.description, udpinternalport, udpexternalport);
			}
			else
			{
				printf("mapping %s udp port failed. internalport = %d, externalport = %d\n", udpmapping.description, udpinternalport, udpexternalport);
			}

			CommandProcess();
		}

		WSACleanup();
	}

	return nRetCode;
}

void CommandProcess()
{
	char Message[1024];

	while (TRUE)
	{
		memset((void*)Message,0,sizeof(Message));
		printf(">");
		scanf("%s",Message);
		char *pPos = Message;
		while (*pPos != '\0')
		{
			*pPos = tolower(*pPos);
			pPos++;
		}
		char * lowCase = Message;

		if(strcmp(lowCase,"exit")==0)
		{
			printf("Exit? [Y] or [N]\n");
			char bexit;
			while (TRUE)
			{
				printf(">");
				bexit = getchar();
				if(isprint(bexit))
				{
					if(bexit=='y'||bexit=='Y'||bexit=='N'||bexit=='n')
					{
						break;
					}
					else 
					{
						printf("Enter [Y] to exit or enter [N] to continue\n");
					}
				}
			}
			printf("%c\n",bexit);
			if(bexit =='y'||bexit == 'Y')
			{
				break;
			}
		}
	}
}

