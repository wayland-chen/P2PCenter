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

#ifndef     __KMSSQLBASE_H__
#define     __KMSSQLBASE_H__

#import "c:\program files\common files\system\ado\msado27.tlb " no_namespace rename("EOF","adoEOF")

class CKMSSqlBase
{
public:
	CKMSSqlBase();
	virtual ~CKMSSqlBase();

	bool ReadConnStrFromIni(const char* pFileName, const char* pSection, const char* pKeyName);
	void SetConnString( const char* pConnStr);
	BOOL Open(  const char* pConnStr = 0);
	void Close();
	BOOL IsOpen();
	BOOL TestConnect();
	BOOL Exec(char* pSql, char *pRet, int iRetLen);

protected:

private:

public:

protected:
	char m_pcConnection[256];
	_ConnectionPtr	m_pLocalConnectPtr;
	_RecordsetPtr	m_pRecordsetPtr;
	_CommandPtr		m_pCommand;

private:

};

#endif //__KMSSQLBASE_H__
