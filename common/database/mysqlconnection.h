/*
 *				FILENAME	: *
 *				AUTHOR		: Kevin(fuwenke@gmail.com)
 *				DATE		: 01/23/2008
 */

#ifndef     __MYSQL_CONNECTION_H__
#define     __MYSQL_CONNECTION_H__

#include "mysqllib0.h"
//#include "mysqlrecordset.h"

namespace DB_TOOLKIT
{

	//ConnectionString应定义为形如：
	//"Server=localhost;Database=MyDatabase;User=MyUserName;Password=MyPassword;
	//Option=4;Port=3306"
	class CMySQLRecordset;

	class CMySQLConnection : public IConnection
	{
		friend class CMySQLRecordset;
	public:
		CMySQLConnection(string strConnection="") throw()
			:IConnection(strConnection) 
		{
			m_lState = CONNECTION_STATE_CLOSED;
			m_pMySQL = NULL;
		}
	    
		virtual ~CMySQLConnection() throw()
		{
			Close();		
		}

	public:
		virtual bool Open() throw(CDBException);
		virtual bool Close() throw();
		virtual bool BeginTransaction() throw(CDBBadQuery,CDBException);
		virtual bool CommitTransaction()throw(CDBBadQuery,CDBException);
		virtual bool RollbackTransaction()throw(CDBBadQuery,CDBException);
		virtual bool Execute(const string& strExec) throw(CDBBadQuery,CDBException);
		virtual bool Execute(ICommand* pCommand) throw(CDBBadQuery,CDBBadParamException,CDBException);
		virtual bool Execute(const char* szExec,unsigned long nLength) throw(CDBBadQuery,CDBException);
		virtual UInt64 GetAffectedRowsCount() throw();

	

	protected:
		bool ParseConnStringAndConnection() throw();

	private:
		CMySQLConnection(const CMySQLConnection&);
		const CMySQLConnection& operator = (const CMySQLConnection&);
	private:
		MYSQL*  m_pMySQL;
	};
};

#endif
