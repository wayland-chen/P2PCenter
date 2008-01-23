/************************************************************************
项目名称：PP流媒体
文件名称：dataobject.h
功    能：数据库操作基本类
作    者：Kevin
创建日期：2006－03－14
修改记录：
		
************************************************************************/

#ifndef  BATABASE_DATA_OBJECT_H
#define BATABASE_DATA_OBJECT_H


#include "mysqlconnection.h"
#include "../msgframe/connection_pool_t.h"
#include "mysqlrecordset.h"
#include "mysqlrow.h"
  
using namespace DB_TOOLKIT;
class CDataObject  
{ 
public:
	
	CDataObject();
	virtual ~CDataObject();
public:
	CDataObject(CConnectionPoolT<CMySQLConnection> *pPoolConns);
	void SetConnectionPool(CConnectionPoolT<CMySQLConnection> *pool);
	 const CConnectionPoolT<CMySQLConnection> * GetConnectionPool();
protected:
	CMySQLConnection* GetConnection();
	BOOL ReleaseConnection(CMySQLConnection *pConn);
    CConnectionPoolT<CMySQLConnection> * m_pPoolConns;
//////////////////////////////////////////////////////////////////////////
};


#endif
