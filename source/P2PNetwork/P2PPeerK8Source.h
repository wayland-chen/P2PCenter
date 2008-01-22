#pragma once
#include "p2ppeer.h"

class CP2PPeerK8Source :
	public CP2PPeer
{
public:
	CP2PPeerK8Source(void);
	virtual ~CP2PPeerK8Source(void);

	virtual bool IsHaveBlock( int nBlockNum );
};
