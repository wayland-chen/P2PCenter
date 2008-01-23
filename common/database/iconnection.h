/*
 *				FILENAME	: *
 *				AUTHOR		: Kevin(fuwenke@gmail.com)
 *				DATE		: 01/23/2008
 */

#ifndef  __INTERFACE_CONNECTION_H__
#define  __INTERFACE_CONNECTION_H__

#include "dbinterface0.h"
#include "icommand.h"

namespace DB_TOOLKIT
{

	class   IConnection
	{
		friend class IRecordset;
	public:
		enum
		{
			CONNECTION_STATE_OPEN           = 0x01,
			CONNECTION_STATE_CLOSED         = 0x02,
			CONNECTION_STATE_EXECUTING      = 0x04,
			CONNECTION_STATE_FETCHING       = 0x08,
			CONNECTION_STATE_CONNECTING     = 0x10
		};
	public:
		IConnection(string strConnection="") throw()
		{
			m_strConnection = strConnection;
			m_lConnectionTimeout = 60;
			m_lLastError = DB_NOERROR;
			m_lState = CONNECTION_STATE_CLOSED;
		}
	    
		virtual ~IConnection() throw()
		{
			m_lState = CONNECTION_STATE_CLOSED;
			m_lLastError = DB_NOERROR;
		}

	public:
		virtual bool    Open()  = 0;
		virtual bool    Close() = 0;

		virtual bool    Execute(ICommand* pCommand) = 0;
		virtual bool    Execute(const string& strExec) = 0;
		virtual bool	Execute(const char* szExec,unsigned long nLength) = 0;
		
		void    SetConnectionString(const string& strConnection) throw()
		{ 
			m_strConnection = strConnection; 
		}

		string  GetConnectionString() throw()
		{ 
			return m_strConnection; 
		}
		void    SetConnectTimeout(signed long lTimeout=60) throw()
		{
			m_lConnectionTimeout = lTimeout;
		}

		virtual UInt64 GetAffectedRowsCount() = 0;

		virtual bool BeginTransaction() = 0 ;
		virtual bool CommitTransaction() = 0 ;
		virtual bool RollbackTransaction() = 0 ;

		unsigned long  GetState() throw()
		{ 
			return m_lState; 
		}
		
		void SetState(unsigned long lState) throw()
		{
			m_lState = lState;
		}

		bool IsOpen() throw()
		{
			return m_lState & CONNECTION_STATE_OPEN;
		}


	private:
		IConnection(const IConnection&);
		const IConnection& operator = (const IConnection&);

	protected:
		string                 m_strConnection;
		string                 m_strLastError;
		unsigned long          m_lConnectionTimeout;
		unsigned long          m_lLastError;        
		unsigned long          m_lState;
	};
};

#endif
