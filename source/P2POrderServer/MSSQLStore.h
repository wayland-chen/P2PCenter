#pragma once

#include "database/mssql/kmssqlbase.h"

class CChannel;

class CMSSQLStore : public CKMSSqlBase
{
public:
	CMSSQLStore(void);
	virtual ~CMSSQLStore(void);

	bool AppendChannelToDB( CChannel* pChannel);
	bool UpdateChannelTick( CChannel* pChannel);
	bool RemoveChannelFromDB( CChannel* pChannel);
};
