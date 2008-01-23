/*
 *				FILENAME	: *
 *				AUTHOR		: Kevin(fuwenke@gmail.com)
 *				DATE		: 01/23/2008
 */

#ifndef     __INTERFACE_COMMAND_H__
#define     __INTERFACE_COMMAND_H__

#include "dbinterface0.h"
#include "iparameter.h"

namespace DB_TOOLKIT
{

	class   ICommand
	{
		friend class IConnection;
		friend class IRecordset;
	public:
		enum CmdType
		{
			CmdUnknown      = 0x00,
			CmdText         = 0x01,
			CmdStoredProc   = 0x02,
			CmdTableDirect  = 0x03
		};

		ICommand(IConnection* pConn = NULL)
		{
			m_pConnection = pConn;
			m_CmdType = CmdUnknown;
			m_lCommandTimeout = 60;
			m_pRecordset = NULL;
		}

		virtual ~ICommand()
		{ };

		void SetActiveConnection(IConnection* pConn)
		{
			m_pConnection = pConn;
		}

		void SetCommandType(CmdType type) throw()
		{
			m_CmdType = type;
		}
		
		CmdType GetCommandType() throw()
		{
			return m_CmdType;
		}

		void SetCommandText(const string& strCmdText) throw()
		{
			m_strCmdText = strCmdText;
		}
		const string& GetCommandText() throw()
		{
			return m_strCmdText;
		}

		void SetCommandTimeout(unsigned long lTimeout) throw()
		{
			m_lCommandTimeout = lTimeout;
		}

		unsigned long GetCommandTimeout() throw()
		{
			return m_lCommandTimeout;
		}

		IParameterCollection* GetParameters() throw()
		{
			return &m_Parameters;
		}

		virtual bool Execute() = 0;
		virtual const IRecordset* Store() = 0;
		
	private:
		ICommand(const ICommand&);
		const ICommand& operator = (const ICommand&);

	protected:
		IConnection*			m_pConnection;
		unsigned long			m_lCommandTimeout;
		CmdType					m_CmdType;
		string					m_strCmdText;
		IParameterCollection	m_Parameters;
		IRecordset*				m_pRecordset;
	};
};

#endif
