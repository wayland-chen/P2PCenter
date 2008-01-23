#include "dataobject.h"


CDataObject::CDataObject()
{
	m_pPoolConns = NULL;
}
CDataObject::~CDataObject()
{
	
}
CDataObject::CDataObject(CConnectionPoolT<CMySQLConnection> *pPoolConns)
{
	m_pPoolConns = pPoolConns;
}

void CDataObject::SetConnectionPool(CConnectionPoolT<CMySQLConnection> *pool)
{
	m_pPoolConns = pool;
}

const CConnectionPoolT<CMySQLConnection> * CDataObject::GetConnectionPool()
{
	return m_pPoolConns;
}

CMySQLConnection* CDataObject::GetConnection()
{
	return m_pPoolConns->CreateElement();
}

BOOL CDataObject::ReleaseConnection(CMySQLConnection *pConn)
{
	return m_pPoolConns->ReleaseElement(pConn);
}
