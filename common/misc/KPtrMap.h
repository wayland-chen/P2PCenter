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

#ifndef __KBUFFERMAP_H__
#define __KBUFFERMAP_H__

#include <map>
using namespace std;
#include "../thread/KCritSec.h"

template <class TKey = DWORD, class T = void> 
class CKPtrMap
{
public: 
	CKPtrMap(void){};
	virtual ~CKPtrMap(void) { Flush(); };

public:

	void Insert(TKey key, T* pVal)
	{
		CKAutoLock l(&m_KCritSec);
		m_map.insert(map<TKey, T*>::value_type(key, pVal));
	}

	void InsertNoLock(TKey key, T* pVal)
	{
		m_map.insert(map<TKey, T*>::value_type(key, pVal));
	}

	DWORD GetCount(void)
	{
		CKAutoLock l(&m_KCritSec);
		return (DWORD)m_map.size();
	}

	void Flush(void)
	{
		CKAutoLock l(&m_KCritSec);

		while (m_map.size())
		{
			T* p = m_map.begin()->second;
			m_map.erase(m_map.begin());
			delete p;
			p = NULL;
		}
	}

	void Remove(TKey key)
	{
		CKAutoLock l(&m_KCritSec);
		map<TKey, T*>::iterator it = m_map.find(key);
		if (it != m_map.end())
		{
			T *p = it->second;
			m_map.erase(it);
			if ( p )
			{
				delete p;
				p = NULL;
			}
		}
	}

	void RemoveNoLock(TKey key)
	{
		map<TKey, T*>::iterator it = m_map.find(key);
		if (it != m_map.end())
		{
			T *p = it->second;
			m_map.erase(it);
			if ( p )
			{
				delete p;
				p = NULL;
			}
		}
	}

	T* Erase(TKey key)
	{
		CKAutoLock l(&m_KCritSec);
		map<TKey, T*>::iterator it = m_map.find(key);
		if (it != m_map.end())
		{
			T *p = it->second;
			m_map.erase(it);
			return p;
		}

		return NULL;
	}

	T* EraseNoLock(TKey key)
	{
		map<TKey, T*>::iterator it = m_map.find(key);
		if (it != m_map.end())
		{
			T *p = it->second;
			m_map.erase(it);
			return p;
		}

		return NULL;
	}

	T* Pop()
	{
		CKAutoLock l(&m_KCritSec);
		map<TKey, T*>::iterator it = m_map.begin();
		if (it != m_map.end())
		{
			T *p = it->second;
			m_map.erase(it);
			return p;
		}

		return NULL;
	}

	T* Find(TKey key)
	{
		CKAutoLock l(&m_KCritSec);

		map<TKey, T*>::iterator it = m_map.find(key);
		T* p = NULL;
		if (it != m_map.end())
		{
			p = it->second;
		}

		return p;
	}

	T* FindNoLock(TKey key)
	{
		map<TKey, T*>::iterator it = m_map.find(key);
		T* p = NULL;
		if (it != m_map.end())
		{
			p = it->second;
		}

		return p;
	}

	bool FirstIndex(TKey& key)
	{
		CKAutoLock l(&m_KCritSec);

		if (m_map.size() == 0)
			return false;

		map<TKey, T*>::iterator it = m_map.begin();
		key = it->first;

		return true;
	}

	map<TKey, T*>* GetMapPtr()
	{
		return &m_map;
	}

	CKCritSec* GetCritSec()
	{
		return &m_KCritSec;
	}

protected:
	CKCritSec			m_KCritSec;
	map<TKey, T*>		m_map;
};

#endif // __KBUFFERMAP_H__
