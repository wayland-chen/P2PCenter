#include "StdAfx.h"
#include "Channel.h"
#include ".\mssqlstore.h"
#include <ATLComTime.h>

CMSSQLStore::CMSSQLStore(void)
{
}

CMSSQLStore::~CMSSQLStore(void)
{
}

bool CMSSQLStore::AppendChannelToDB( CChannel* pChannel)
{
	if ( IsOpen())
	{
		try
		{
			char szSql[256];
			sprintf( szSql, "SELECT * FROM Channel WHERE userid = %d AND filehash = '%s' AND filesize = '%s'", 
				pChannel->m_nUserID, pChannel->m_sChannelID.c_str(), pChannel->m_sFileSize.c_str());

			HRESULT _hr = m_pRecordsetPtr->Open(
				szSql,
				_variant_t((IDispatch*)m_pLocalConnectPtr,true),
				adOpenStatic,
				adLockOptimistic,
				adCmdText
				);

			if (FAILED(_hr))
			{
				m_pRecordsetPtr->Close();
				return FALSE;
			}

			if( m_pRecordsetPtr->adoEOF)
			{
				m_pRecordsetPtr->AddNew();
				m_pRecordsetPtr->PutCollect( "userid", _variant_t(pChannel->m_nUserID));
				m_pRecordsetPtr->PutCollect( "filehash", _variant_t(pChannel->m_sChannelID.c_str()));
				m_pRecordsetPtr->PutCollect( "filesize", _variant_t(pChannel->m_sFileSize.c_str()));
				m_pRecordsetPtr->PutCollect( "appenddate", _variant_t(COleDateTime::GetCurrentTime()));
			}

			m_pRecordsetPtr->PutCollect( "agent", _variant_t(pChannel->m_sAgent.c_str()));
			m_pRecordsetPtr->PutCollect( "upuserid", _variant_t(pChannel->m_nUpUserID));
			m_pRecordsetPtr->PutCollect( "nodename", _variant_t(pChannel->m_sNodeName.c_str()));
			m_pRecordsetPtr->PutCollect( "filename", _variant_t(pChannel->m_sFileName.c_str()));
			m_pRecordsetPtr->PutCollect( "updatedate", _variant_t(COleDateTime::GetCurrentTime()));

			_hr = m_pRecordsetPtr->Update();
			
			if (FAILED(_hr))
			{
				m_pRecordsetPtr->Close();
				return FALSE;
			}

			m_pRecordsetPtr->Close();

			return true;

		}
		catch(_com_error e)
		{
		}
	}

	return false;
}

bool CMSSQLStore::UpdateChannelTick( CChannel* pChannel)
{
	if ( IsOpen())
	{
		try
		{
			char szSql[256];
			sprintf( szSql, "SELECT * FROM Channel WHERE userid = %d AND filehash = '%s' AND filesize = '%s'", 
				pChannel->m_nUserID, pChannel->m_sChannelID.c_str(), pChannel->m_sFileSize.c_str());

			HRESULT _hr = m_pRecordsetPtr->Open(
				szSql,
				_variant_t((IDispatch*)m_pLocalConnectPtr,true),
				adOpenStatic,
				adLockOptimistic,
				adCmdText
				);

			if (FAILED(_hr))
			{
				m_pRecordsetPtr->Close();
				return FALSE;
			}

			if( !m_pRecordsetPtr->adoEOF)
			{
				m_pRecordsetPtr->PutCollect( "updatedate", _variant_t(COleDateTime::GetCurrentTime()));
				_hr = m_pRecordsetPtr->Update();
				if (FAILED(_hr))
				{
					m_pRecordsetPtr->Close();
					return FALSE;
				}
			}

			m_pRecordsetPtr->Close();

			return true;

		}
		catch(_com_error e)
		{
		}
	}

	return false;
}

bool CMSSQLStore::RemoveChannelFromDB( CChannel* pChannel)
{
	char szSql[256];
	sprintf( szSql, "DELETE Channel WHERE userid = %d AND filehash = '%s'", 
		pChannel->m_nUserID, pChannel->m_sChannelID.c_str());

	return Exec( szSql, NULL, 0);
}
