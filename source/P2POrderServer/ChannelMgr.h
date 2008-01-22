#pragma once

#include "misc/KPtrMap.h"
#include "Channel.h"

class CChannelMgr : public CKPtrMap<string, CChannel>
{
public:
	CChannelMgr(void);
	virtual ~CChannelMgr(void);

	bool SaveHtmlFile( string sNodeName );
	bool SaveJSFile( map<string, CChannel *> Channels);
};
