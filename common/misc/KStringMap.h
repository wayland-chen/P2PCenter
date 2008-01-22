/********************************************************************
	created:	2008/01/11
	created:	11:1:2008   15:49
	author:		Kevin
	
	purpose:	

	email:		kan8888.com@163.com
	msn:		fuwenke@gmail.com
	QQ:		799383417
	site:		opensource.kan8888.com
*********************************************************************/

#ifndef _HKSTRINGMAP_H
#define _HKSTRINGMAP_H

#include <map>
#include <string>
using namespace std;
#include "../thread/KCritSec.h"
class CKStringMap
{
public: 
	CKStringMap(void){};
	virtual ~CKStringMap(void) {};

public:

	void Insert(string key, void* pVal)
	{
		CKAutoLock l(&m_KCritSec);
		m_map.insert(map<string, void *>::value_type(key, pVal));
	}

	DWORD GetCount(void)
	{
		CKAutoLock l(&m_KCritSec);
		return (DWORD)m_map.size();
	}

	void* Erase(string key)
	{
		CKAutoLock l(&m_KCritSec);
		map<string, void *>::iterator it = m_map.find(key);
		if (it != m_map.end())
		{
			void *p = it->second;
			m_map.erase(it);
			return p;
		}

		return NULL;
	}

	void* Find(string key)
	{
		CKAutoLock l(&m_KCritSec);

		map<string, void *>::iterator it = m_map.find(key);
		void* p = NULL;
		if (it != m_map.end())
		{
			p = it->second;
		}

		return p;
	}

	bool FirstIndex(string& key)
	{
		CKAutoLock l(&m_KCritSec);

		if (m_map.size() == 0)
			return false;

		map<string, void*>::iterator it = m_map.begin();
		key = it->first;

		return true;
	}

	map<string, void *>* GetMapPtr()
	{
		return &m_map;
	}

	CKCritSec* GetCritSec()
	{
		return &m_KCritSec;
	}

protected:
	CKCritSec			    m_KCritSec;
	map<string, void *>		m_map;
};

#endif // _HKSTRINGMAP_H
