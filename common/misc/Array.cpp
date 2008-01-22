
#include "Array.h"

#ifndef WIN32

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

#endif

namespace KBASE
{
	CWordArray::CWordArray()
	{
		m_pData = NULL;
		m_nSize = m_nMaxSize = m_nGrowBy = 0;
	}
	
	CWordArray::~CWordArray()
	{
		delete[] (BYTE*)m_pData;
	}
	
	void CWordArray::SetSize(int nNewSize, int nGrowBy)
	{
		if (nGrowBy != -1)
			m_nGrowBy = nGrowBy;  // set new size
		
		if (nNewSize == 0)
		{
			// shrink to nothing
			delete[] (BYTE*)m_pData;
			m_pData = NULL;
			m_nSize = m_nMaxSize = 0;
		}
		else if (m_pData == NULL)
		{
			// create one with exact size
#ifdef SIZE_T_MAX
			ASSERT(nNewSize <= SIZE_T_MAX/sizeof(WORD));    // no overflow
#endif
			m_pData = (WORD*) new BYTE[nNewSize * sizeof(WORD)];
			
			memset(m_pData, 0, nNewSize * sizeof(WORD));  // zero fill
			
			m_nSize = m_nMaxSize = nNewSize;
		}
		else if (nNewSize <= m_nMaxSize)
		{
			// it fits
			if (nNewSize > m_nSize)
			{
				// initialize the new elements
				
				memset(&m_pData[m_nSize], 0, (nNewSize-m_nSize) * sizeof(WORD));
				
			}
			
			m_nSize = nNewSize;
		}
		else
		{
			// otherwise, grow array
			int nGrowBy = m_nGrowBy;
			if (nGrowBy == 0)
			{
				// heuristically determine growth when nGrowBy == 0
				//  (this avoids heap fragmentation in many situations)
				nGrowBy = min(1024, max(4, m_nSize / 8));
			}
			int nNewMax;
			if (nNewSize < m_nMaxSize + nGrowBy)
				nNewMax = m_nMaxSize + nGrowBy;  // granularity
			else
				nNewMax = nNewSize;  // no slush
			
			WORD* pNewData = (WORD*) new BYTE[nNewMax * sizeof(WORD)];
			
			// copy new data from old
			memcpy(pNewData, m_pData, m_nSize * sizeof(WORD));
			
			// construct remaining elements
			
			memset(&pNewData[m_nSize], 0, (nNewSize-m_nSize) * sizeof(WORD));
			
			
			// get rid of old stuff (note: no destructors called)
			delete[] (BYTE*)m_pData;
			m_pData = pNewData;
			m_nSize = nNewSize;
			m_nMaxSize = nNewMax;
		}
	}
	
	int CWordArray::Append(const CWordArray& src)
	{
		int nOldSize = m_nSize;
		SetSize(m_nSize + src.m_nSize);
		
		memcpy(m_pData + nOldSize, src.m_pData, src.m_nSize * sizeof(WORD));
		
		return nOldSize;
	}
	
	void CWordArray::Copy(const CWordArray& src)
	{
		SetSize(src.m_nSize);
		
		memcpy(m_pData, src.m_pData, src.m_nSize * sizeof(WORD));
		
	}
	
	void CWordArray::FreeExtra()
	{
		if (m_nSize != m_nMaxSize)
		{
			// shrink to desired size
#ifdef SIZE_T_MAX
			ASSERT(m_nSize <= SIZE_T_MAX/sizeof(WORD)); // no overflow
#endif
			WORD* pNewData = NULL;
			if (m_nSize != 0)
			{
				pNewData = (WORD*) new BYTE[m_nSize * sizeof(WORD)];
				// copy new data from old
				memcpy(pNewData, m_pData, m_nSize * sizeof(WORD));
			}
			
			// get rid of old stuff (note: no destructors called)
			delete[] (BYTE*)m_pData;
			m_pData = pNewData;
			m_nMaxSize = m_nSize;
		}
	}
	
	/////////////////////////////////////////////////////////////////////////////
	
	void CWordArray::SetAtGrow(int nIndex, WORD newElement)
	{
		if (nIndex >= m_nSize)
			SetSize(nIndex+1);
		m_pData[nIndex] = newElement;
	}
	
	
	
	
	
	void CWordArray::InsertAt(int nIndex, WORD newElement, int nCount)
	{
		
		if (nIndex >= m_nSize)
		{
			// adding after the end of the array
			SetSize(nIndex + nCount);  // grow so nIndex is valid
		}
		else
		{
			// inserting in the middle of the array
			int nOldSize = m_nSize;
			SetSize(m_nSize + nCount);  // grow it to new size
			// shift old data up to fill gap
			memmove(&m_pData[nIndex+nCount], &m_pData[nIndex],
				(nOldSize-nIndex) * sizeof(WORD));
			
			// re-init slots we copied from
			
			memset(&m_pData[nIndex], 0, nCount * sizeof(WORD));
			
		}
		
		// insert new value in the gap
		
		
		
		// copy elements into the empty space
		while (nCount--)
			m_pData[nIndex++] = newElement;
		
	}
	
	
	
	void CWordArray::RemoveAt(int nIndex, int nCount)
	{
		// just remove a range
		int nMoveCount = m_nSize - (nIndex + nCount);
		
		if (nMoveCount)
			memmove(&m_pData[nIndex], &m_pData[nIndex + nCount],
			nMoveCount * sizeof(WORD));
		m_nSize -= nCount;
	}
	
	void CWordArray::InsertAt(int nStartIndex, CWordArray* pNewArray)
	{
		if (pNewArray->GetSize() > 0)
		{
			InsertAt(nStartIndex, pNewArray->GetAt(0), pNewArray->GetSize());
			for (int i = 0; i < pNewArray->GetSize(); i++)
				SetAt(nStartIndex + i, pNewArray->GetAt(i));
		}
	}
	
	int CWordArray::GetSize() const
	{ return m_nSize; }
	int CWordArray::GetUpperBound() const
	{ return m_nSize-1; }
	void CWordArray::RemoveAll()
	{ SetSize(0); }
	WORD CWordArray::GetAt(int nIndex) const
	{ 
		return m_pData[nIndex]; }
	void CWordArray::SetAt(int nIndex, WORD newElement)
	{ 
		m_pData[nIndex] = newElement; }
	
	WORD& CWordArray::ElementAt(int nIndex)
	{ 
		return m_pData[nIndex]; }
	const WORD* CWordArray::GetData() const
	{ return (const WORD*)m_pData; }
	WORD* CWordArray::GetData()
	{ return (WORD*)m_pData; }
	int CWordArray::Add(WORD newElement)
	{ int nIndex = m_nSize;
	SetAtGrow(nIndex, newElement);
	return nIndex; }
	
	WORD CWordArray::operator[](int nIndex) const
	{ return GetAt(nIndex); }
	WORD& CWordArray::operator[](int nIndex)
	{ return ElementAt(nIndex); }
	
	
	//  CDWordArray
	
	
	CDWordArray::CDWordArray()
	{
		m_pData = NULL;
		m_nSize = m_nMaxSize = m_nGrowBy = 0;
	}
	
	CDWordArray::~CDWordArray()
	{
		delete[] (BYTE*)m_pData;
	}
	
	void CDWordArray::SetSize(int nNewSize, int nGrowBy)
	{
		if (nGrowBy != -1)
			m_nGrowBy = nGrowBy;  // set new size
		
		if (nNewSize == 0)
		{
			// shrink to nothing
			delete[] (BYTE*)m_pData;
			m_pData = NULL;
			m_nSize = m_nMaxSize = 0;
		}
		else if (m_pData == NULL)
		{
			// create one with exact size
			m_pData = (DWORD*) new BYTE[nNewSize * sizeof(DWORD)];
			
			memset(m_pData, 0, nNewSize * sizeof(DWORD));  // zero fill
			
			m_nSize = m_nMaxSize = nNewSize;
		}
		else if (nNewSize <= m_nMaxSize)
		{
			// it fits
			if (nNewSize > m_nSize)
			{
				// initialize the new elements
				
				memset(&m_pData[m_nSize], 0, (nNewSize-m_nSize) * sizeof(DWORD));
				
			}
			
			m_nSize = nNewSize;
		}
		else
		{
			// otherwise, grow array
			int nGrowBy = m_nGrowBy;
			if (nGrowBy == 0)
			{
				// heuristically determine growth when nGrowBy == 0
				//  (this avoids heap fragmentation in many situations)
				nGrowBy = min(1024, max(4, m_nSize / 8));
			}
			int nNewMax;
			if (nNewSize < m_nMaxSize + nGrowBy)
				nNewMax = m_nMaxSize + nGrowBy;  // granularity
			else
				nNewMax = nNewSize;  // no slush
			
			DWORD* pNewData = (DWORD*) new BYTE[nNewMax * sizeof(DWORD)];
			
			// copy new data from old
			memcpy(pNewData, m_pData, m_nSize * sizeof(DWORD));
			
			// construct remaining elements
			
			memset(&pNewData[m_nSize], 0, (nNewSize-m_nSize) * sizeof(DWORD));
			
			
			// get rid of old stuff (note: no destructors called)
			delete[] (BYTE*)m_pData;
			m_pData = pNewData;
			m_nSize = nNewSize;
			m_nMaxSize = nNewMax;
		}
	}
	
	int CDWordArray::Append(const CDWordArray& src)
	{
		int nOldSize = m_nSize;
		SetSize(m_nSize + src.m_nSize);
		
		memcpy(m_pData + nOldSize, src.m_pData, src.m_nSize * sizeof(DWORD));
		
		return nOldSize;
	}
	
	void CDWordArray::Copy(const CDWordArray& src)
	{
		
		SetSize(src.m_nSize);
		
		memcpy(m_pData, src.m_pData, src.m_nSize * sizeof(DWORD));
		
	}
	
	void CDWordArray::FreeExtra()
	{
		
		if (m_nSize != m_nMaxSize)
		{
			// shrink to desired size
			DWORD* pNewData = NULL;
			if (m_nSize != 0)
			{
				pNewData = (DWORD*) new BYTE[m_nSize * sizeof(DWORD)];
				// copy new data from old
				memcpy(pNewData, m_pData, m_nSize * sizeof(DWORD));
			}
			
			// get rid of old stuff (note: no destructors called)
			delete[] (BYTE*)m_pData;
			m_pData = pNewData;
			m_nMaxSize = m_nSize;
		}
	}
	
	/////////////////////////////////////////////////////////////////////////////
	
	void CDWordArray::SetAtGrow(int nIndex, DWORD newElement)
	{
		if (nIndex >= m_nSize)
			SetSize(nIndex+1);
		m_pData[nIndex] = newElement;
	}
	
	
	
	
	
	void CDWordArray::InsertAt(int nIndex, DWORD newElement, int nCount)
	{
		
		if (nIndex >= m_nSize)
		{
			// adding after the end of the array
			SetSize(nIndex + nCount);  // grow so nIndex is valid
		}
		else
		{
			// inserting in the middle of the array
			int nOldSize = m_nSize;
			SetSize(m_nSize + nCount);  // grow it to new size
			// shift old data up to fill gap
			memmove(&m_pData[nIndex+nCount], &m_pData[nIndex],
				(nOldSize-nIndex) * sizeof(DWORD));
			
			// re-init slots we copied from
			
			memset(&m_pData[nIndex], 0, nCount * sizeof(DWORD));
			
		}
		
		// insert new value in the gap
		
		
		
		// copy elements into the empty space
		while (nCount--)
			m_pData[nIndex++] = newElement;
		
	}
	
	
	
	void CDWordArray::RemoveAt(int nIndex, int nCount)
	{
		// just remove a range
		if(!m_nSize) return;
		int nMoveCount = m_nSize - (nIndex + nCount);
		
		if (nMoveCount)
		{
			memmove(&m_pData[nIndex], &m_pData[nIndex + nCount],
				nMoveCount * sizeof(DWORD));
		}
		else if(m_nSize==(nIndex+nCount))
			memset(&m_pData[nIndex], 0,
			sizeof(DWORD));
		m_nSize -= nCount;
	}
	
	void CDWordArray::InsertAt(int nStartIndex, CDWordArray* pNewArray)
	{
		if (pNewArray->GetSize() > 0)
		{
			InsertAt(nStartIndex, pNewArray->GetAt(0), pNewArray->GetSize());
			for (int i = 0; i < pNewArray->GetSize(); i++)
				SetAt(nStartIndex + i, pNewArray->GetAt(i));
		}
	}
	
	
	
	int CDWordArray::GetSize() const
	{ return m_nSize; }
	int CDWordArray::GetUpperBound() const
	{ return m_nSize-1; }
	void CDWordArray::RemoveAll()
	{ SetSize(0); }
	DWORD CDWordArray::GetAt(int nIndex) const
	{ 
		return m_pData[nIndex]; }
	void CDWordArray::SetAt(int nIndex, DWORD newElement)
	{ 
		m_pData[nIndex] = newElement; }
	
	DWORD& CDWordArray::ElementAt(int nIndex)
	{
		return m_pData[nIndex]; }
	const DWORD* CDWordArray::GetData() const
	{ return (const DWORD*)m_pData; }
	DWORD* CDWordArray::GetData()
	{ return (DWORD*)m_pData; }
	int CDWordArray::Add(DWORD newElement)
	{ int nIndex = m_nSize;
	SetAtGrow(nIndex, newElement);
	return nIndex; }
	
	DWORD CDWordArray::operator[](int nIndex) const
	{ return GetAt(nIndex); }
	DWORD& CDWordArray::operator[](int nIndex)
	{ return ElementAt(nIndex); }
	
	
	
	//  CByteArray
	
	
	CByteArray::CByteArray()
	{
		m_pData = NULL;
		m_nSize = m_nMaxSize = m_nGrowBy = 0;
	}
	
	CByteArray::~CByteArray()
	{
		delete[] (BYTE*)m_pData;
	}
	
	void CByteArray::SetSize(int nNewSize, int nGrowBy)
	{
		if (nGrowBy != -1)
			m_nGrowBy = nGrowBy;  // set new size
		
		if (nNewSize == 0)
		{
			// shrink to nothing
			delete[] (BYTE*)m_pData;
			m_pData = NULL;
			m_nSize = m_nMaxSize = 0;
		}
		else if (m_pData == NULL)
		{
			// create one with exact size
			m_pData = (BYTE*) new BYTE[nNewSize * sizeof(BYTE)];
			
			memset(m_pData, 0, nNewSize * sizeof(BYTE));  // zero fill
			
			m_nSize = m_nMaxSize = nNewSize;
		}
		else if (nNewSize <= m_nMaxSize)
		{
			// it fits
			if (nNewSize > m_nSize)
			{
				// initialize the new elements
				
				memset(&m_pData[m_nSize], 0, (nNewSize-m_nSize) * sizeof(BYTE));
				
			}
			
			m_nSize = nNewSize;
		}
		else
		{
			// otherwise, grow array
			int nGrowBy = m_nGrowBy;
			if (nGrowBy == 0)
			{
				// heuristically determine growth when nGrowBy == 0
				//  (this avoids heap fragmentation in many situations)
				nGrowBy = min(1024, max(4, m_nSize / 8));
			}
			int nNewMax;
			if (nNewSize < m_nMaxSize + nGrowBy)
				nNewMax = m_nMaxSize + nGrowBy;  // granularity
			else
				nNewMax = nNewSize;  // no slush
			
			BYTE* pNewData = (BYTE*) new BYTE[nNewMax * sizeof(BYTE)];
			
			// copy new data from old
			memcpy(pNewData, m_pData, m_nSize * sizeof(BYTE));
			
			// construct remaining elements
			
			memset(&pNewData[m_nSize], 0, (nNewSize-m_nSize) * sizeof(BYTE));
			
			
			// get rid of old stuff (note: no destructors called)
			delete[] (BYTE*)m_pData;
			m_pData = pNewData;
			m_nSize = nNewSize;
			m_nMaxSize = nNewMax;
		}
	}
	
	int CByteArray::Append(const CByteArray& src)
	{
		int nOldSize = m_nSize;
		SetSize(m_nSize + src.m_nSize);
		
		memcpy(m_pData + nOldSize, src.m_pData, src.m_nSize * sizeof(BYTE));
		
		return nOldSize;
	}
	
	void CByteArray::Copy(const CByteArray& src)
	{
		SetSize(src.m_nSize);
		
		memcpy(m_pData, src.m_pData, src.m_nSize * sizeof(BYTE));
		
	}
	
	void CByteArray::FreeExtra()
	{
		
		if (m_nSize != m_nMaxSize)
		{
			// shrink to desired size
			BYTE* pNewData = NULL;
			if (m_nSize != 0)
			{
				pNewData = (BYTE*) new BYTE[m_nSize * sizeof(BYTE)];
				// copy new data from old
				memcpy(pNewData, m_pData, m_nSize * sizeof(BYTE));
			}
			
			// get rid of old stuff (note: no destructors called)
			delete[] (BYTE*)m_pData;
			m_pData = pNewData;
			m_nMaxSize = m_nSize;
		}
	}
	
	/////////////////////////////////////////////////////////////////////////////
	
	void CByteArray::SetAtGrow(int nIndex, BYTE newElement)
	{
		if (nIndex >= m_nSize)
			SetSize(nIndex+1);
		m_pData[nIndex] = newElement;
	}
	
	
	
	
	
	void CByteArray::InsertAt(int nIndex, BYTE newElement, int nCount)
	{
		if (nIndex >= m_nSize)
		{
			// adding after the end of the array
			SetSize(nIndex + nCount);  // grow so nIndex is valid
		}
		else
		{
			// inserting in the middle of the array
			int nOldSize = m_nSize;
			SetSize(m_nSize + nCount);  // grow it to new size
			// shift old data up to fill gap
			memmove(&m_pData[nIndex+nCount], &m_pData[nIndex],
				(nOldSize-nIndex) * sizeof(BYTE));
			
			// re-init slots we copied from
			
			memset(&m_pData[nIndex], 0, nCount * sizeof(BYTE));
			
		}
		
		// insert new value in the gap
		
		
		
		// copy elements into the empty space
		while (nCount--)
			m_pData[nIndex++] = newElement;
		
	}
	
	
	
	void CByteArray::RemoveAt(int nIndex, int nCount)
	{
		// just remove a range
		int nMoveCount = m_nSize - (nIndex + nCount);
		
		if (nMoveCount)
			memmove(&m_pData[nIndex], &m_pData[nIndex + nCount],
			nMoveCount * sizeof(BYTE));
		m_nSize -= nCount;
	}
	
	void CByteArray::InsertAt(int nStartIndex, CByteArray* pNewArray)
	{
		if (pNewArray->GetSize() > 0)
		{
			InsertAt(nStartIndex, pNewArray->GetAt(0), pNewArray->GetSize());
			for (int i = 0; i < pNewArray->GetSize(); i++)
				SetAt(nStartIndex + i, pNewArray->GetAt(i));
		}
	}
	
	
	
	int CByteArray::GetSize() const
	{ return m_nSize; }
	int CByteArray::GetUpperBound() const
	{ return m_nSize-1; }
	void CByteArray::RemoveAll()
	{ SetSize(0); }
	BYTE CByteArray::GetAt(int nIndex) const
	{
		return m_pData[nIndex]; }
	void CByteArray::SetAt(int nIndex, BYTE newElement)
	{
		m_pData[nIndex] = newElement; }
	
	BYTE& CByteArray::ElementAt(int nIndex)
	{
		return m_pData[nIndex]; }
	const BYTE* CByteArray::GetData() const
	{ return (const BYTE*)m_pData; }
	BYTE* CByteArray::GetData()
	{ return (BYTE*)m_pData; }
	int CByteArray::Add(BYTE newElement)
	{ int nIndex = m_nSize;
	SetAtGrow(nIndex, newElement);
	return nIndex; }
	
	BYTE CByteArray::operator[](int nIndex) const
	{ return GetAt(nIndex); }
	BYTE& CByteArray::operator[](int nIndex)
	{ return ElementAt(nIndex); }
	
	
	//  CPtrArray
	
	CPtrArray::CPtrArray()
	{
		m_pData = NULL;
		m_nSize = m_nMaxSize = m_nGrowBy = 0;
	}
	
	CPtrArray::~CPtrArray()
	{
		delete[] (BYTE*)m_pData;
	}
	
	void CPtrArray::SetSize(int nNewSize, int nGrowBy)
	{
		if (nGrowBy != -1)
			m_nGrowBy = nGrowBy;  // set new size
		
		if (nNewSize == 0)
		{
			// shrink to nothing
			delete[] (BYTE*)m_pData;
			m_pData = NULL;
			m_nSize = m_nMaxSize = 0;
		}
		else if (m_pData == NULL)
		{
			// create one with exact size
			m_pData = (void**) new BYTE[nNewSize * sizeof(void*)];
			
			memset(m_pData, 0, nNewSize * sizeof(void*));  // zero fill
			
			m_nSize = m_nMaxSize = nNewSize;
		}
		else if (nNewSize <= m_nMaxSize)
		{
			// it fits
			if (nNewSize > m_nSize)
			{
				// initialize the new elements
				
				memset(&m_pData[m_nSize], 0, (nNewSize-m_nSize) * sizeof(void*));
				
			}
			
			m_nSize = nNewSize;
		}
		else
		{
			// otherwise, grow array
			int nGrowBy = m_nGrowBy;
			if (nGrowBy == 0)
			{
				// heuristically determine growth when nGrowBy == 0
				//  (this avoids heap fragmentation in many situations)
				nGrowBy = min(1024, max(4, m_nSize / 8));
			}
			int nNewMax;
			if (nNewSize < m_nMaxSize + nGrowBy)
				nNewMax = m_nMaxSize + nGrowBy;  // granularity
			else
				nNewMax = nNewSize;  // no slush
			
			void** pNewData = (void**) new BYTE[nNewMax * sizeof(void*)];
			
			// copy new data from old
			memcpy(pNewData, m_pData, m_nSize * sizeof(void*));
			
			// construct remaining elements
			
			memset(&pNewData[m_nSize], 0, (nNewSize-m_nSize) * sizeof(void*));
			
			
			// get rid of old stuff (note: no destructors called)
			delete[] (BYTE*)m_pData;
			m_pData = pNewData;
			m_nSize = nNewSize;
			m_nMaxSize = nNewMax;
		}
	}
	
	int CPtrArray::Append(const CPtrArray& src)
	{
		int nOldSize = m_nSize;
		SetSize(m_nSize + src.m_nSize);
		
		memcpy(m_pData + nOldSize, src.m_pData, src.m_nSize * sizeof(void*));
		
		return nOldSize;
	}
	
	void CPtrArray::Copy(const CPtrArray& src)
	{
		SetSize(src.m_nSize);
		
		memcpy(m_pData, src.m_pData, src.m_nSize * sizeof(void*));
		
	}
	
	void CPtrArray::FreeExtra()
	{
		
		if (m_nSize != m_nMaxSize)
		{
			// shrink to desired size
			void** pNewData = NULL;
			if (m_nSize != 0)
			{
				pNewData = (void**) new BYTE[m_nSize * sizeof(void*)];
				// copy new data from old
				memcpy(pNewData, m_pData, m_nSize * sizeof(void*));
			}
			
			// get rid of old stuff (note: no destructors called)
			delete[] (BYTE*)m_pData;
			m_pData = pNewData;
			m_nMaxSize = m_nSize;
		}
	}
	
	/////////////////////////////////////////////////////////////////////////////
	
	void CPtrArray::SetAtGrow(int nIndex, void* newElement)
	{
		if (nIndex >= m_nSize)
			SetSize(nIndex+1);
		m_pData[nIndex] = newElement;
	}
	
	
	
	
	
	void CPtrArray::InsertAt(int nIndex, void* newElement, int nCount)
	{
		if (nIndex >= m_nSize)
		{
			// adding after the end of the array
			SetSize(nIndex + nCount);  // grow so nIndex is valid
		}
		else
		{
			// inserting in the middle of the array
			int nOldSize = m_nSize;
			SetSize(m_nSize + nCount);  // grow it to new size
			// shift old data up to fill gap
			memmove(&m_pData[nIndex+nCount], &m_pData[nIndex],
				(nOldSize-nIndex) * sizeof(void*));
			
			// re-init slots we copied from
			
			memset(&m_pData[nIndex], 0, nCount * sizeof(void*));
			
		}
		
		// insert new value in the gap
		
		
		
		// copy elements into the empty space
		while (nCount--)
			m_pData[nIndex++] = newElement;
		
	}
	
	
	
	void CPtrArray::RemoveAt(int nIndex, int nCount)
	{
		// just remove a range
		int nMoveCount = m_nSize - (nIndex + nCount);
		
		if (nMoveCount)
			memmove(&m_pData[nIndex], &m_pData[nIndex + nCount],
			nMoveCount * sizeof(void*));
		m_nSize -= nCount;
	}
	
	void CPtrArray::InsertAt(int nStartIndex, CPtrArray* pNewArray)
	{
		if (pNewArray->GetSize() > 0)
		{
			InsertAt(nStartIndex, pNewArray->GetAt(0), pNewArray->GetSize());
			for (int i = 0; i < pNewArray->GetSize(); i++)
				SetAt(nStartIndex + i, pNewArray->GetAt(i));
		}
	}
	int CPtrArray::GetSize() const
	{ return m_nSize; }
	int CPtrArray::GetUpperBound() const
	{ return m_nSize-1; }
	void CPtrArray::RemoveAll()
	{ SetSize(0); }
	char* CPtrArray::GetAt(int nIndex) const
	{ 
		return (char *)m_pData[nIndex]; }
	void CPtrArray::SetAt(int nIndex, void* newElement)
	{ 
		m_pData[nIndex] = newElement; }
	
	void*& CPtrArray::ElementAt(int nIndex)
	{
		return m_pData[nIndex]; }
	const void** CPtrArray::GetData() const
	{ return (const void**)m_pData; }
	void** CPtrArray::GetData()
	{ return (void**)m_pData; }
	int CPtrArray::Add(void* newElement)
	{ int nIndex = m_nSize;
	SetAtGrow(nIndex, newElement);
	return nIndex; }
	
	void* CPtrArray::operator[](int nIndex) const
	{ return GetAt(nIndex); }
	void*& CPtrArray::operator[](int nIndex)
	{ return ElementAt(nIndex); }
	
	
	
	
	DWORD CDWordArray::GetTail()
	{
		int nCount=sizeof(m_pData)/sizeof(DWORD);
		return m_pData[nCount-1];
	}
	
	DWORD CDWordArray::GetHead()
	{
		return m_pData[0];
	}
	
	int CDWordArray::GetCount()
	{
		int count=sizeof(m_pData)/sizeof(DWORD);
		if(!m_pData)
			count=0;
		return count;
	}
	
	void CPtrArray::RemoveTail()
	{
		if(m_nSize<=0)
			return ;
		RemoveAt(m_nSize-1);
	}
	
	void* CPtrArray::RemoveHead()
	{
		if(m_nSize>0)
		{
			LPVOID p=GetAt(0);
			RemoveAt(0);
			return p;
		}
		return NULL;
	}
};

