#pragma once

#include "MediaFile.h"

class CMediaFileMgr : public CKPtrMap<string, CMediaFile>
{
public:
	CMediaFileMgr(void);
	virtual ~CMediaFileMgr(void);
};
