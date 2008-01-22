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

#ifndef __KPTRLIST_H__
#define __KPTRLIST_H__

#include <list>
using namespace std;

#include "../thread/KCritSec.h"

template <class T> 
class CKPtrList
{
public:
	CKPtrList(void){};
	~CKPtrList(){
		RemoveAll();
	}

	unsigned int GetCount()
	{
		// CKAutoLock l(&m_lock);
		return (unsigned int)m_listT.size();
	};

	bool IsEmpty()
	{
		// CKAutoLock l(&m_lock);
		return m_listT.size();
	};

	const T* GetHead()
	{
		// CKAutoLock l(&m_lock);
		T* pElement =  NULL;
		if (m_listT.size()) {
			pElement = m_listT.front();
		}		
		return pElement;
	};

	const T* GetTail()
	{
		// CKAutoLock l(&m_lock);
		T* pElement =  NULL;
		if (m_listT.size()) {
			pElement = m_listT.back();
		}		
		return pElement;
	};

	T* Pop()
	{
		CKAutoLock l(&m_lock);
		T* pElement =  NULL;
		if (m_listT.size()) {
			pElement = (T*)m_listT.front();
			m_listT.pop_front();
		}		
		return pElement;
	};

	T* PopBack()
	{
		CKAutoLock l(&m_lock);
		T* pElement =  NULL;
		if (m_listT.size()) {
			pElement = (T*)m_listT.back();
			m_listT.pop_back();
		}		
		return pElement;
	};

	void Push(T* p)
	{
		CKAutoLock l(&m_lock);
		m_listT.push_back(p);
	};

	void PushFront(T* p)
	{
		CKAutoLock l(&m_lock);
		m_listT.push_front(p);
	};

	void RemoveAt(T* p)
	{
		// CKAutoLock l(&m_lock);

		list<void *>::iterator it = m_listT.begin();
		while (it != m_listT.end()) {
			T* pElement = (T*)*it;
			if (pElement == p) {
				delete pElement;
				pElement = NULL;
				it = m_listT.erase(it);
				break;
			}
			else
				it ++;
		}
	};

	void EraseAt(T* p)
	{
		// CKAutoLock l(&m_lock);
		list<void *>::iterator it = m_listT.begin();
		while (it != m_listT.end()) {
			T* pElement = (T*)*it;
			if (pElement == p) {
				it = m_listT.erase(it);
				break;
			}
			else
				it ++;
		}
	};

	void RemoveAll()
	{
		// CKAutoLock l(&m_lock);
		list<void*>::iterator it = m_listT.begin();
		while (it != m_listT.end()) {
			T* pElement = (T*)*it;
			if (pElement) {
				delete pElement;
				pElement = NULL;
			}
			it = m_listT.erase(it);
		}
	};

	// 取得特定位置元素，dwPos 从0开始，如果 dwPos 大于或等于元素个数，取得求余位置元素
	T* GetElementAtPos(DWORD dwPos)
	{
		// CKAutoLock l(&m_lock);

		if (m_listT.size() == 0)
			return NULL;

		DWORD dwNewPos = dwPos % (DWORD)m_listT.size();
		list<void*>::iterator it = m_listT.begin();
		for (DWORD dw = 0; dw < dwNewPos; dw ++, it ++);
		return (T*)*it;
	}

	list<T*>* GetListPtr()
	{
		return (list<T*>*)&m_listT;
	}

	CKCritSec* GetCritSec()
	{
		return &m_lock;
	}

	T* Find(T* p)
	{
		// CKAutoLock l(&m_lock);
		list<void *>::iterator it = m_listT.begin();
		while (it != m_listT.end()) {
			if (*it == p) {
				return (T*)*it;
			}
			else
				it ++;
		}

		return NULL;
	};

	T* FindByID( const char* szID )
	{
		// CKAutoLock l(&m_lock);
		list<void *>::iterator it = m_listT.begin();
		while (it != m_listT.end()) {
			T* p = (T*)*it;
			if ( p && ( szID ) && ( p->GetID() ) && 0 == strcmp( p->GetID(), szID ) ) {
				return p;
			}
			else
				it ++;
		}

		return NULL;
	};

	T* FindByID( unsigned long ulID )
	{
		// CKAutoLock l(&m_lock);
		list<void *>::iterator it = m_listT.begin();
		while (it != m_listT.end()) {
			T* p = (T*)*it;
			if ( p && p->GetID() == ulID ) {
				return p;
			}
			else
				it ++;
		}

		return NULL;
	};

	void RemoveByID( const char* szID )
	{
		// CKAutoLock l(&m_lock);
		list<void *>::iterator it = m_listT.begin();
		while (it != m_listT.end()) {
			T* p = (T*)*it;
			if ( p && ( szID ) && ( p->GetID() ) && 0 == strcmp( p->GetID(), szID ) ) {
				it = m_listT.erase(it);
				delete p;
				p = NULL;
				return;
			}
			else
				it ++;
		}
	};

	void RemoveByID( unsigned long ulID )
	{
		// CKAutoLock l(&m_lock);
		list<void *>::iterator it = m_listT.begin();
		while (it != m_listT.end()) {
			T* p = (T*)*it;
			if ( p && p->GetID() == ulID ) {
				it = m_listT.erase(it);
				delete p;
				p = NULL;
				return;
			}
			else
				it ++;
		}
	};

protected:
	list<void *>	m_listT;
	CKCritSec		m_lock;
};


#endif // __KPTRLIST_H__

