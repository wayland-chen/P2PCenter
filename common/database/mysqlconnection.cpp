/*
 *				FILENAME	: *
 *				AUTHOR		: Kevin(fuwenke@gmail.com)
 *				DATE		: 01/23/2008
 *				PURPOSE		: implemention of class CMySQLConnection
 */


#include "mysqllib0.h"
#include "mysqlconnection.h"
#include "mysqlcommand.h"
#include "mapdt.h"
#include <typeinfo>

namespace DB_TOOLKIT
{

	bool CMySQLConnection::Open() throw(CDBException)
	{
		if(IsOpen())
		{
			return true;
		}
		m_lState = CONNECTION_STATE_CLOSED;
		if(NULL==(m_pMySQL=mysql_init(NULL)))
		{
			m_lLastError = mysql_errno(m_pMySQL);
			m_strLastError = mysql_error(m_pMySQL);
			throw CDBException(m_lLastError,m_strLastError);
		}
		if(0 != mysql_options(m_pMySQL,MYSQL_OPT_CONNECT_TIMEOUT,(const char*)&m_lConnectionTimeout))
		{
			m_lLastError = mysql_errno(m_pMySQL);
			m_strLastError = mysql_error(m_pMySQL);
			throw CDBException(m_lLastError,m_strLastError);
		}
		m_lState = CONNECTION_STATE_CONNECTING;
		if(!ParseConnStringAndConnection())
		{
			m_lState = CONNECTION_STATE_CLOSED;
			throw CDBException(m_lLastError,m_strLastError);
		}
		m_lState = CONNECTION_STATE_OPEN;

		return true;
	}


	bool CMySQLConnection::Close() throw()
	{
		if(m_pMySQL)
		{
			mysql_close(m_pMySQL);
			m_pMySQL = NULL;
		}
		m_lState = CONNECTION_STATE_CLOSED;
		m_lLastError = DB_NOERROR;
		m_strLastError.erase();
		m_strConnection.erase();
		
		return true;
	}

	bool CMySQLConnection::ParseConnStringAndConnection() throw()
	{
		if(m_strConnection.empty())
		{
			return false;
		}
		transform(m_strConnection.begin(),m_strConnection.end(),m_strConnection.begin(),tolower);
		string strType[6] = { "server","database","user","password","option","port" };
		string strParam[6];
		string strHost,strUser,strPassword,strDatebase;
		unsigned int nPort,nOptions;
		string::size_type pos1 = string::npos;
		string::size_type pos2 = string::npos;
	    
		for(int i=0;i<6;i++)
		{
			if(string::npos != (pos1=m_strConnection.find(strType[i],0)))
			{
				strParam[i] = m_strConnection.substr(pos1+strType[i].size()+1,
					(string::npos==(pos2=m_strConnection.find(';',pos1))?(string::npos):(pos2-1-m_strConnection.find('=',pos1))));
				pos1 = pos2;
			}
			else
			{
				m_strConnection.erase();
				m_lLastError    = g_ErrorMsg[UD_ERROR_CONN_CONNECTIONSTRING_IDX].m_nError;
				m_strConnection = g_ErrorMsg[UD_ERROR_CONN_CONNECTIONSTRING_IDX].m_szError;
				return false;
			}
		}
		nPort = strtoul(strParam[5].c_str(),NULL,10);
		nOptions = strtoul(strParam[4].c_str(),NULL,10);

		if(NULL==mysql_real_connect(m_pMySQL,strParam[0].c_str(),
			strParam[2].c_str(),strParam[3].c_str(),strParam[1].c_str(),nPort,NULL,nOptions))
		{
			m_lState = CONNECTION_STATE_CLOSED;
			m_lLastError = mysql_errno(m_pMySQL);
			m_strLastError = mysql_error(m_pMySQL);
			return false;
		}
		m_lState = CONNECTION_STATE_OPEN;
		return true;
	}


	bool CMySQLConnection::BeginTransaction() throw(CDBBadQuery,CDBException)
	{
		if(m_lState & CONNECTION_STATE_CLOSED)
		{
			m_strLastError = g_ErrorMsg[UD_ERROR_NO_ACTIVE_CONN_IDX].m_szError;
			m_lLastError = g_ErrorMsg[UD_ERROR_NO_ACTIVE_CONN_IDX].m_nError;
			throw CDBException(m_lLastError,m_strLastError);
		}

		string str = "START TRANSACTION";
		m_lState |= CONNECTION_STATE_EXECUTING;
		if(0 != mysql_real_query(m_pMySQL,str.c_str(),str.length()))
		{
			m_lLastError = mysql_errno(m_pMySQL);
			m_strLastError = mysql_error(m_pMySQL);
			m_lState &=~(unsigned long)CONNECTION_STATE_EXECUTING;
			throw CDBBadQuery(m_lLastError,m_strLastError);
		}
		m_lState &=~(unsigned long)CONNECTION_STATE_EXECUTING;

		return true;
	}

	bool CMySQLConnection::CommitTransaction() throw(CDBBadQuery,CDBException)
	{
		if(m_lState & CONNECTION_STATE_CLOSED)
		{
			m_strLastError = g_ErrorMsg[UD_ERROR_NO_ACTIVE_CONN_IDX].m_szError;
			m_lLastError = g_ErrorMsg[UD_ERROR_NO_ACTIVE_CONN_IDX].m_nError;
			throw CDBException(m_lLastError,m_strLastError);
		}
		
		string str = "COMMIT";
		m_lState |= CONNECTION_STATE_EXECUTING;
		if(0 != mysql_real_query(m_pMySQL,str.c_str(),str.length()))
		{
			m_lLastError = mysql_errno(m_pMySQL);
			m_strLastError = mysql_error(m_pMySQL);
			m_lState &=~(unsigned long)CONNECTION_STATE_EXECUTING;
			throw CDBBadQuery(m_lLastError,m_strLastError);
		}
		m_lState &=~(unsigned long)CONNECTION_STATE_EXECUTING;

		return true;
	}

	bool CMySQLConnection::RollbackTransaction() throw(CDBBadQuery,CDBException)
	{
		if(m_lState & CONNECTION_STATE_CLOSED)
		{
			m_strLastError = g_ErrorMsg[UD_ERROR_NO_ACTIVE_CONN_IDX].m_szError;
			m_lLastError = g_ErrorMsg[UD_ERROR_NO_ACTIVE_CONN_IDX].m_nError;
			throw CDBException(m_lLastError,m_strLastError);
		}

		string str = "ROLLBACK";
		m_lState |= CONNECTION_STATE_EXECUTING;
		if(0 != mysql_real_query(m_pMySQL,str.c_str(),str.length()))
		{
			m_lLastError = mysql_errno(m_pMySQL);
			m_strLastError = mysql_error(m_pMySQL);
			m_lState &=~(unsigned long)CONNECTION_STATE_EXECUTING;
			throw CDBBadQuery(m_lLastError,m_strLastError);
		}
		m_lState &=~(unsigned long)CONNECTION_STATE_EXECUTING;

		return true;
	}

	UInt64 CMySQLConnection::GetAffectedRowsCount() throw()
	{
		return mysql_affected_rows(m_pMySQL);
	}

	bool CMySQLConnection::Execute(const string& strExec) throw(CDBBadQuery,CDBException)
	{
		return Execute(strExec.c_str(),strExec.length());
	}

	bool CMySQLConnection::Execute(const char* szExec,unsigned long nLength) throw(CDBBadQuery,CDBException)
	{
		if(m_lState & CONNECTION_STATE_CLOSED)
		{
			m_strLastError = g_ErrorMsg[UD_ERROR_NO_ACTIVE_CONN_IDX].m_szError;
			m_lLastError = g_ErrorMsg[UD_ERROR_NO_ACTIVE_CONN_IDX].m_nError;
			throw CDBException(m_lLastError,m_strLastError);
		}
		
		m_lState |= CONNECTION_STATE_EXECUTING;
		if(0 != mysql_real_query(m_pMySQL,szExec,nLength))
		{
			m_lLastError = mysql_errno(m_pMySQL);
			m_strLastError = mysql_error(m_pMySQL);
			m_lState &=~(unsigned long)CONNECTION_STATE_EXECUTING;
			throw CDBBadQuery(m_lLastError,m_strLastError);
		}
		m_lState &=~(unsigned long)CONNECTION_STATE_EXECUTING;
		
		return true;

	}


	bool CMySQLConnection::Execute(ICommand* pCommand) throw(CDBBadQuery,CDBBadParamException,CDBException)
	{
		if(m_lState & CONNECTION_STATE_CLOSED)
		{
			m_strLastError = g_ErrorMsg[UD_ERROR_NO_ACTIVE_CONN_IDX].m_szError;
			m_lLastError = g_ErrorMsg[UD_ERROR_NO_ACTIVE_CONN_IDX].m_nError;
			throw CDBException(m_lLastError,m_strLastError);
		}
		
		//fwkCMySQLCommand* pCmd = dynamic_cast<CMySQLCommand*>(pCommand);
		CMySQLCommand* pCmd = (CMySQLCommand*)(pCommand);
		if((NULL==pCommand) || !pCmd)
		{
			m_strLastError = g_ErrorMsg[UD_ERROR_INVALID_COMMAND_OBJ_IDX].m_szError;
			m_lLastError = g_ErrorMsg[UD_ERROR_INVALID_COMMAND_OBJ_IDX].m_nError;
			throw CDBBadParamException(m_lLastError,m_strLastError);
		}

		m_lState |= CONNECTION_STATE_EXECUTING;

		string str;
		switch(pCommand->GetCommandType())
		{
		case ICommand::CmdUnknown:
		case ICommand::CmdText:
			str = pCommand->GetCommandText();
			break;
		case ICommand::CmdStoredProc:
			{
				str = "CALL ";
				str += pCommand->GetCommandText();
				str += "(";
				IParameterCollection* pParaColl = pCommand->GetParameters();
				for(int i=0;i<pParaColl->GetParamCount();i++)
				{
					if(((*pParaColl)[i].m_Direction == IParameter::Input) ||
						((*pParaColl)[i].m_Direction == IParameter::InputAndOutput))
					{
						str += (string)((*pParaColl)[i].m_Value);
						str += ",";
					}
					else
					{
						assert(false);
					}
				}
				str.erase(str.find_last_of(','),1);
				str += ")";
			}
			break;
		case ICommand::CmdTableDirect:
			m_strLastError = g_ErrorMsg[UD_ERROR_NOT_IMPLEMENT_IDX].m_szError;
			m_lLastError = g_ErrorMsg[UD_ERROR_NOT_IMPLEMENT_IDX].m_nError;
			m_lState &=~(unsigned long)CONNECTION_STATE_EXECUTING;
			throw CDBException(m_lLastError,m_strLastError);
			break;

			Execute(str);
		}
		m_lState &=~(unsigned long)CONNECTION_STATE_EXECUTING;

		return true;
	}


};

