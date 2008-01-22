// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>

#include <map>
#include <string>
#include <list>
using namespace std;

#include <atlbase.h>
#include <atlapp.h>
#include <atlmisc.h>

#include <assert.h>

#include "log/KLog.h"
#include "p2pdefine.h"
#include "p2sdefine.h"

#include "thread/KThread.h"
#include "misc/KPtrList.h"
#include "misc/KPtrMap.h"
#include "network/KSocket.h"

#include "ip2pnetwork.h"

#include "TcpClient.h"
#pragma comment(lib, "TcpClient.lib")
#pragma comment(lib, "WS2_32.lib")

#include "P2SCommand.h"
#include "P2PCommand.h"

#include "P2PNetwork.h"
extern CP2PNetwork* g_pNetwork;

// TODO: 在此处引用程序要求的附加头文件
