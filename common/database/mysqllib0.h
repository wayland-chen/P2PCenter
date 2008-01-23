/*
 *				FILENAME	: *
 *				AUTHOR		: Kevin(fuwenke@gmail.com)
 *				DATE		: 01/23/2008
 */

#ifndef     __MYSQL_LIB_0_H__
#define     __MYSQL_LIB_0_H__

#include "os.h"

#ifdef OS_IS_MSWINDOWS
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <ctype.h> //for function to lower
#endif

#include <string>
#include <algorithm>
#include <vector>

#ifdef WIN32
#include "mysql/include/mysql.h" 
#else 
#include "mysql.h" 
#endif

#ifdef OS_IS_MSWINDOWS

//#pragma  comment(lib,"mysys.lib")
#pragma  comment(lib,"libmysql.lib")
#pragma  comment(lib,"mysqlclient.lib")

#endif 

using namespace std;

#include <assert.h>

#include "errormsg.h"
#include "dbinterface.h"

#if (!defined(MYSQL_VERSION_ID))
#error !!!!!!!!!please check your mysql package is whether be installed or not!!!!!!!
#endif

#if (MYSQL_VERSION_ID < 40000)
#error !!!!!!!!Be make sure the version of your mysql is no less than 4.0!!!!!!!!!
#endif

#endif
