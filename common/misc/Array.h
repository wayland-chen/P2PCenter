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
#ifndef _MMSDK_ARRAY_H
#define _MMSDK_ARRAY_H

#include "define/kwin32types.h"

#include <string.h>
namespace KBASE
{
	class CByteArray 
	{
	public:
		// Construction
		CByteArray();
		// Attributes
		int GetSize() const;
		int GetUpperBound() const;
		void SetSize(int nNewSize, int nGrowBy = -1);
		// Operations
		// Clean up
		void FreeExtra();
		void RemoveAll();
		
		// Accessing elements
		BYTE GetAt(int nIndex) const;
		void SetAt(int nIndex, BYTE newElement);
		
		BYTE& ElementAt(int nIndex);
		
		// Direct Access to the element data (may return NULL)
		const BYTE* GetData() const;
		BYTE* GetData();
		
		// Potentially growing the array
		void SetAtGrow(int nIndex, BYTE newElement);
		
		int Add(BYTE newElement);
		
		int Append(const CByteArray& src);
		void Copy(const CByteArray& src);
		
		// overloaded operator helpers
		BYTE operator[](int nIndex) const;
		BYTE& operator[](int nIndex);
		
		// Operations that move elements around
		void InsertAt(int nIndex, BYTE newElement, int nCount = 1);
		
		void RemoveAt(int nIndex, int nCount = 1);
		void InsertAt(int nStartIndex, CByteArray* pNewArray);
		
		// Implementation
	protected:
		BYTE* m_pData;   // the actual array of data
		int m_nSize;     // # of elements (upperBound - 1)
		int m_nMaxSize;  // max allocated
		int m_nGrowBy;   // grow amount
		
		
	public:
		~CByteArray();
		
	protected:
		// local typedefs for class templates
		typedef BYTE BASE_TYPE;
		typedef BYTE BASE_ARG_TYPE;
	};
	
	
	////////////////////////////////////////////////////////////////////////////
	
	class CWordArray 
	{
	public:
		
		// Construction
		CWordArray();
		
		// Attributes
		int GetSize() const;
		int GetUpperBound() const;
		void SetSize(int nNewSize, int nGrowBy = -1);
		
		// Operations
		// Clean up
		void FreeExtra();
		void RemoveAll();
		
		// Accessing elements
		WORD GetAt(int nIndex) const;
		void SetAt(int nIndex, WORD newElement);
		
		WORD& ElementAt(int nIndex);
		
		// Direct Access to the element data (may return NULL)
		const WORD* GetData() const;
		WORD* GetData();
		
		// Potentially growing the array
		void SetAtGrow(int nIndex, WORD newElement);
		
		int Add(WORD newElement);
		
		int Append(const CWordArray& src);
		void Copy(const CWordArray& src);
		
		// overloaded operator helpers
		WORD operator[](int nIndex) const;
		WORD& operator[](int nIndex);
		
		// Operations that move elements around
		void InsertAt(int nIndex, WORD newElement, int nCount = 1);
		
		void RemoveAt(int nIndex, int nCount = 1);
		void InsertAt(int nStartIndex, CWordArray* pNewArray);
		
		// Implementation
	protected:
		WORD* m_pData;   // the actual array of data
		int m_nSize;     // # of elements (upperBound - 1)
		int m_nMaxSize;  // max allocated
		int m_nGrowBy;   // grow amount
		
		
	public:
		~CWordArray();
		
	protected:
		// local typedefs for class templates
		typedef WORD BASE_TYPE;
		typedef WORD BASE_ARG_TYPE;
	};

	////////////////////////////////////////////////////////////////////////////
	
	class CDWordArray
	{
	public:
		
		// Construction
		CDWordArray();
		
		// Attributes
		int GetSize() const;
		int GetUpperBound() const;
		void SetSize(int nNewSize, int nGrowBy = -1);
		
		// Operations
		// Clean up
		void FreeExtra();
		void RemoveAll();
		
		// Accessing elements
		DWORD GetAt(int nIndex) const;
		void SetAt(int nIndex, DWORD newElement);
		
		DWORD& ElementAt(int nIndex);
		
		// Direct Access to the element data (may return NULL)
		const DWORD* GetData() const;
		DWORD* GetData();
		
		// Potentially growing the array
		void SetAtGrow(int nIndex, DWORD newElement);
		
		int Add(DWORD newElement);
		
		int Append(const CDWordArray& src);
		void Copy(const CDWordArray& src);
		
		// overloaded operator helpers
		DWORD operator[](int nIndex) const;
		DWORD& operator[](int nIndex);
		
		// Operations that move elements around
		void InsertAt(int nIndex, DWORD newElement, int nCount = 1);
		
		void RemoveAt(int nIndex, int nCount = 1);
		void InsertAt(int nStartIndex, CDWordArray* pNewArray);
		
		// Implementation
	protected:
		DWORD* m_pData;   // the actual array of data
		int m_nSize;     // # of elements (upperBound - 1)
		int m_nMaxSize;  // max allocated
		int m_nGrowBy;   // grow amount
		
		
	public:
		int GetCount();
		DWORD GetHead();
		DWORD GetTail();
		~CDWordArray();
		
	protected:
		// local typedefs for class templates
		typedef DWORD BASE_TYPE;
		typedef DWORD BASE_ARG_TYPE;
	};
	
	class CPtrArray 
	{
	public:
		
		// Construction
		CPtrArray();
		
		// Attributes
		int GetSize() const;
		int GetUpperBound() const;
		void SetSize(int nNewSize, int nGrowBy = -1);
		
		// Operations
		// Clean up
		void FreeExtra();
		void RemoveAll();
		
		// Accessing elements
		char* GetAt(int nIndex) const;
		void SetAt(int nIndex, void* newElement);
		
		void*& ElementAt(int nIndex);
		
		// Direct Access to the element data (may return NULL)
		const void** GetData() const;
		void** GetData();
		
		// Potentially growing the array
		void SetAtGrow(int nIndex, void* newElement);
		
		int Add(void* newElement);
		
		int Append(const CPtrArray& src);
		void Copy(const CPtrArray& src);
		
		// overloaded operator helpers
		void* operator[](int nIndex) const;
		void*& operator[](int nIndex);
		
		// Operations that move elements around
		void InsertAt(int nIndex, void* newElement, int nCount = 1);
		
		void RemoveAt(int nIndex, int nCount = 1);
		void InsertAt(int nStartIndex, CPtrArray* pNewArray);
		
		// Implementation
	protected:
		void** m_pData;   // the actual array of data
		int m_nSize;     // # of elements (upperBound - 1)
		int m_nMaxSize;  // max allocated
		int m_nGrowBy;   // grow amount
		
		
	public:
		void* RemoveHead();
		void RemoveTail();
		~CPtrArray();
		
	protected:
		// local typedefs for class templates
		typedef void* BASE_TYPE;
		typedef void* BASE_ARG_TYPE;
	};

#define  BLOCKNUM  800
	class 	CLinkQueue
	{
	public:
		CLinkQueue()
		{
			m_nInIndex = 0;
			m_nOutIndex = 0;
			memset(m_pData,0,BLOCKNUM*sizeof(void *));
		}
		~CLinkQueue(){};
		
		BOOL Push(void *data)
		{
			if(m_pData[m_nInIndex] != NULL )
				return FALSE;
			
			//ÖÃÈë
			m_pData[m_nInIndex] = data;
			m_nInIndex++;
			if(m_nInIndex > BLOCKNUM-1) m_nInIndex = 0;
			return TRUE;
		}
		void * Pop()
		{
			if(m_pData[m_nOutIndex] == NULL )
				return NULL;
			//Å×³ö
			void *ret = m_pData[m_nOutIndex];
			m_pData[m_nOutIndex] = NULL;
			m_nOutIndex++;
			if(m_nOutIndex > BLOCKNUM-1) m_nOutIndex = 0;
			return ret;
		}
		BOOL IsEmpty()
		{
			return (m_nOutIndex == m_nInIndex);
		}
		
	private:
		void *m_pData[BLOCKNUM];
		int m_nInIndex;
		int m_nOutIndex;
		
	};
};
#endif // __ARRAY_H
