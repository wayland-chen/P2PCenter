/*
 *				FILENAME	: *
 *				AUTHOR		: Kevin(fuwenke@gmail.com)
 *				DATE		: 01/23/2008
 */

#ifndef     __MYSQL_COMMAND_H__
#define     __MYSQL_COMMAND_H__

#include "mysqllib0.h"
#include "icommand.h"
#include "mysqlrecordset.h"

namespace DB_TOOLKIT
{

	class CMySQLCommand : public ICommand
	{
	public:
		CMySQLCommand() throw()
		{
		}
		CMySQLCommand(IConnection* pConnection)
			:ICommand(pConnection)
		{
		}
		~CMySQLCommand() throw()
		{
			if(m_pRecordset)
			{
				m_pRecordset->Close();
				delete m_pRecordset;
			}
		}
		virtual bool Execute() throw(CDBBadQuery,CDBException)
		{
			if(NULL == m_pConnection)
			{
				throw CDBException(UD_ERROR_NO_ACTIVE_CONN,g_ErrorMsg[UD_ERROR_NO_ACTIVE_CONN_IDX].m_szError);
			}
			return m_pConnection->Execute(this);
		}

		virtual const IRecordset* Store() throw(CDBBadQuery,CDBOutOfMemory,CDBException)
		{
			if(NULL == m_pConnection)
			{
				throw CDBException(UD_ERROR_NO_ACTIVE_CONN,g_ErrorMsg[UD_ERROR_NO_ACTIVE_CONN_IDX].m_szError);
			}

			if(NULL == m_pRecordset)
			{
				if(!(m_pRecordset=new CMySQLRecordset(m_pConnection)))
				{
					throw CDBOutOfMemory(UD_ERROR_OUT_OF_MEMORY);
				}
			}
			try
			{
				m_pRecordset->Store(this);
			}
			catch(const CDBBadQuery& e)
			{
				m_pRecordset->Close();
				delete m_pRecordset;
				m_pRecordset = NULL;
				throw e;
			}
			catch(const CDBBadParamException& e)
			{
				m_pRecordset->Close();
				delete m_pRecordset;
				m_pRecordset = NULL;
				throw e;
			}
			catch(const CDBException& e)
			{
				m_pRecordset->Close();
				delete m_pRecordset;
				m_pRecordset = NULL;
				throw e;
			}
			return m_pRecordset;
		}
		
	private:
		CMySQLCommand(const CMySQLCommand&);
		const CMySQLCommand& operator = (const CMySQLCommand&);
	};
};

#endif

