
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#ifndef WIN32
	#include <unistd.h>
    #include <strings.h>
#endif

#include "StringBuffer.h"

#define ALLOC_BLOCK_SIZE 4096

#ifndef isblank
#define isblank(c) ((c) == ' ' || (c) == '\t')
#endif

CStringBuffer::CStringBuffer()
{
	m_pchData = NULL;
	m_nSize = 0;
	m_nLength = 0;

	*this = "";
}
CStringBuffer::CStringBuffer(const CStringBuffer& stringSrc)
{
	m_pchData = NULL;
	m_nSize = 0;
	m_nLength = 0;
	
	*this = stringSrc;
}
CStringBuffer::CStringBuffer(const char* lpsz)
{
	m_pchData = NULL;
	m_nSize = 0;
	m_nLength = 0;

	*this = lpsz;
}
CStringBuffer::CStringBuffer(const unsigned char* psz)
{
	m_pchData = NULL;
	m_nSize = 0;
	m_nLength = 0;

	*this = psz;
}
CStringBuffer::~CStringBuffer()
{
	if(m_pchData)
		free( m_pchData );
}
bool CStringBuffer::Grow(int nMinSize)
{
	nMinSize ++;
	if(nMinSize > m_nSize)
	{
		int nSize = (nMinSize + ALLOC_BLOCK_SIZE - 1) / ALLOC_BLOCK_SIZE * ALLOC_BLOCK_SIZE;
		char* temp;

		if(m_pchData)
			temp = (char*)realloc(m_pchData,  nSize );
		else
			temp = (char*)malloc(nSize);
		if(temp)
		{
			m_pchData = temp;
			m_nSize = nSize;
		}
	}
	
	return (nMinSize <= m_nSize);
}

int CStringBuffer::GetLength() const
{
	return m_nLength;
}
bool CStringBuffer::IsEmpty() const
{
	return (m_nLength == 0);
}
void CStringBuffer::Empty()
{
	m_nLength = 0;
	if(m_pchData)
		*m_pchData = 0;
}

char CStringBuffer::GetAt(int nIndex) const
{
	return m_pchData[nIndex];
}
char CStringBuffer::operator[](int nIndex) const
{
	return m_pchData[nIndex];
}
void CStringBuffer::SetAt(int nIndex, char ch)
{
	m_pchData[nIndex] = ch;
}
CStringBuffer::operator const char*() const
{
	if(m_pchData)
		return (const char*)m_pchData;
	else
		return "";
}

const CStringBuffer& CStringBuffer::operator=(const CStringBuffer& stringSrc)
{
	return operator=((const char*)stringSrc);	
}
const CStringBuffer& CStringBuffer::operator=(char ch)
{
	char str[2] = { ch, 0};
	
	return operator=((const char*)str);	
}
const CStringBuffer& CStringBuffer::operator=(const char* lpsz)
{
	if(lpsz)
	{
		int nLength = strlen(lpsz);
		Grow( nLength );
		if(m_nSize > nLength)
		{
			strcpy(m_pchData, lpsz);
			m_nLength = nLength;
		}
	}
	else Empty();
		

	return *this;
}

const CStringBuffer& CStringBuffer::operator+=(const CStringBuffer& string)
{
	return operator+=((const char*)string);
}
const CStringBuffer& CStringBuffer::operator+=(char ch)
{
	char str[2] = {ch, 0};
	return operator+=((const char*)str);
}
const CStringBuffer& CStringBuffer::operator+=(const char* lpsz)
{
	if(lpsz)
	{
		int nSize = strlen(lpsz) + m_nLength;
		Grow( nSize );
		if(m_nSize > nSize)
		{
			strcpy(m_pchData + m_nLength, lpsz);
			m_nLength = nSize;
		}
	}

	return *this;
}

int CStringBuffer::Compare(const char* lpsz) const
{
	if(m_pchData && lpsz)
		return strcmp(m_pchData, lpsz);
	else if(m_pchData > lpsz)
		return 1;
	else
		return -1;
}
int CStringBuffer::CompareNoCase(const char* lpsz) const
{
	if(m_pchData && lpsz)
#ifndef WIN32
        return strcasecmp(m_pchData,lpsz);
#else
		return stricmp(m_pchData, lpsz);
#endif
	else if(m_pchData > lpsz)
		return 1;
	else
		return -1;
}


CStringBuffer CStringBuffer::Mid(int nFirst, int nCount) const
{
	CStringBuffer result;

	for(int i = nFirst; i < m_nLength && (i - nFirst) < nCount; i ++)
	{
		result += m_pchData[i];
	}

	return result;
}
CStringBuffer CStringBuffer::Mid(int nFirst) const
{
	CStringBuffer result;

	for(int i = nFirst; i < m_nLength; i ++)
	{
		result += m_pchData[i];
	}

	return result;
}
CStringBuffer CStringBuffer::Left(int nCount) const
{
	return Mid(0, nCount);
}
CStringBuffer CStringBuffer::Right(int nCount) const
{
	if(nCount < m_nLength)
		return Mid(m_nLength - nCount, nCount);
	else
		return *this;
}

void CStringBuffer::MakeUpper()
{
	for(int i = 0; i < m_nLength; i ++)
		m_pchData[i] = toupper(m_pchData[i]);
}
void CStringBuffer::MakeLower()
{
	for(int i = 0; i < m_nLength; i ++)
		m_pchData[i] = tolower(m_pchData[i]);
}
void CStringBuffer::MakeReverse()
{
	for(int i = 0; i < m_nLength; i ++)
	{
		if( isascii(m_pchData[i]) )
		{
			if( isupper( m_pchData[i] ) )
				m_pchData[i] = tolower( m_pchData[i] );
			else
				m_pchData[i] = toupper( m_pchData[i] );
		}
	}
}

void CStringBuffer::TrimRight()
{
	for(int i = m_nLength - 1; i >= 0; i --)
	{
		if( isblank( m_pchData[i] ) )
		{
			m_pchData[i] = 0;
			m_nLength --;
		}
		else 	break;
	}
}
void CStringBuffer::TrimLeft()
{
	int nCount = 0;
	
	for(int i = 0; i < m_nLength; i ++)
	{
		if( isblank( m_pchData[i] ) )
			nCount ++;
		else 	break;
	}
	
	Delete(0, nCount);
}

int CStringBuffer::Insert(int nIndex, char ch)
{
	int nSize = m_nLength + 1;
	Grow(nSize);
	if(m_nSize > nSize)
	{
		for(int i = m_nLength; i > nIndex; i ++)
			m_pchData[i + 1] = m_pchData[i];
		m_pchData[nIndex] = ch;
		m_nLength = nSize;
	}
	
	return m_nLength;
}
int CStringBuffer::Insert(int nIndex, const char* pstr)
{
	if(pstr && *pstr)
	{
		int nLength = strlen(pstr);
		int nSize = nLength + m_nLength;
		Grow(nSize);
		if(m_nSize > nSize)
		{
			for(int i = m_nLength; i > nIndex; i ++)
				m_pchData[i + nLength] = m_pchData[i];
			memcpy(m_pchData + nIndex, pstr, nLength);
			m_nLength = nSize;
		}
	}

	return m_nLength;
}
int CStringBuffer::Delete(int nIndex, int nCount)
{
	int nLength = 0;
	if(nCount) for(int i = nIndex; i <= m_nLength - nCount; i ++)
	{
		m_pchData[ i ] = m_pchData[ i + nCount];
		nLength ++;
	}
	m_nLength -= nLength;

	return m_nLength;
}

int CStringBuffer::Find(char ch) const
{
	int nPos = -1;
	for(int i = 0; i < m_nLength; i ++)
	{
		if(m_pchData[i] == ch)
		{
			nPos = i;
			break;
		}
	}
	
	return nPos;
}
int CStringBuffer::ReverseFind(char ch) const
{
	int nPos = -1;
	for(int i = m_nLength - 1; i >= 0; i --)
	{
		if(m_pchData[i] == ch)
		{
			nPos = i;
			break;
		}
	}

	return nPos;
}
int CStringBuffer::Find(char ch, int nStart) const
{
	int nPos = -1;
	for(int i = nStart; i < m_nLength; i ++)
	{
		if(m_pchData[i] == ch)
		{
			nPos = i;
			break;
		}
	}
	
	return nPos;
}
int CStringBuffer::Find(const char* lpszSub) const
{
	if(m_pchData && lpszSub)
	{
		char* temp = strstr(m_pchData, lpszSub);
		if(temp)
			return temp - m_pchData;
	}

	return -1;
}
int CStringBuffer::Find(const char* lpszSub, int nStart) const
{
	if(m_pchData && lpszSub && nStart >= 0 && nStart < m_nLength)
	{
		char* temp = strstr(m_pchData + nStart, lpszSub);
		if(temp)
			return temp - m_pchData;
	}

	return -1;
}
char* CStringBuffer::GetBuffer()
{
	return m_pchData;
}

char * CStringBuffer::GetBufferSetLength(int nLength)
{
	
	char* pszBuffer = GetBuffer( nLength );
	SetLength( nLength );
	return( pszBuffer );
	
}

void CStringBuffer::SetLength(int nLength)
{
	if(nLength>0)
	{
		m_nLength = nLength;
		m_pchData[nLength] =0;
	}
}

char* CStringBuffer::GetBuffer( int nMinBufferLength )
{
	m_pchData = (char*)realloc(m_pchData,nMinBufferLength+1);
	return m_pchData;
}



void CStringBuffer::Format(const char* lpszFormat, ...)
{
	Grow(ALLOC_BLOCK_SIZE - 1);
	if(m_pchData)
	{
		while(1)
		{
			va_list list;
			va_start(list, lpszFormat);
#ifdef WIN32
			m_nLength = _vsnprintf(m_pchData, m_nSize - 1, lpszFormat, list);
#else
            m_nLength = vsnprintf(m_pchData, m_nSize - 1, lpszFormat, list);
#endif
 			
			va_end(list);

 			if( m_nLength != -1 && m_nLength <= (m_nSize - 1) || !Grow(m_nLength) )
				break;
		};
	}
}

CStringBuffer operator+(const CStringBuffer& string1, const CStringBuffer& string2)
{
	CStringBuffer result;

	result += string1;
	result += string2;

	return result;
}
CStringBuffer operator+(const CStringBuffer& string, char ch)
{
	CStringBuffer result;

	result += string;
	result += ch;

	return result;
}
CStringBuffer operator+(char ch, const CStringBuffer& string)
{
	CStringBuffer result;

	result += ch;
	result += string;

	return result;
}
CStringBuffer operator+(const CStringBuffer& string, const char* lpsz)
{
	CStringBuffer result;

	result += string;
	result += lpsz;

	return result;
}
CStringBuffer operator+(const char* lpsz, const CStringBuffer& string)
{
	CStringBuffer result;

	result += lpsz;
	result += string;

	return result;
}
					

bool operator==(const CStringBuffer& s1, const CStringBuffer& s2)
{
	return ( strcmp(s1, s2) == 0);
}
bool operator==(const CStringBuffer& s1, const char* s2)
{
	return ( strcmp(s1, s2) == 0);
}
bool operator==(const char* s1, const CStringBuffer& s2)
{
	return ( strcmp(s1, s2) == 0);
}
bool operator!=(const CStringBuffer& s1, const CStringBuffer& s2)
{
	return ( strcmp(s1, s2) != 0);
}
bool operator!=(const CStringBuffer& s1, const char* s2)
{
	return ( strcmp(s1, s2) != 0);
}
bool operator!=(const char* s1, const CStringBuffer& s2)
{
	return ( strcmp(s1, s2) != 0);
}
bool operator<(const CStringBuffer& s1, const CStringBuffer& s2)
{
	return ( strcmp(s1, s2) < 0);
}
bool operator<(const CStringBuffer& s1, const char* s2)
{
	return ( strcmp(s1, s2) < 0);
}
bool operator<(const char* s1, const CStringBuffer& s2)
{
	return ( strcmp(s1, s2) < 0);
}
bool operator>(const CStringBuffer& s1, const CStringBuffer& s2)
{
	return ( strcmp(s1, s2) > 0);
}
bool operator>(const CStringBuffer& s1, const char* s2)
{
	return ( strcmp(s1, s2) > 0);
}
bool operator>(const char* s1, const CStringBuffer& s2)
{
	return ( strcmp(s1, s2) > 0);
}
bool operator<=(const CStringBuffer& s1, const CStringBuffer& s2)
{
	return ( strcmp(s1, s2) <=  0);
}
bool operator<=(const CStringBuffer& s1, const char* s2)
{
	return ( strcmp(s1, s2) <= 0);
}
bool operator<=(const char* s1, const CStringBuffer& s2)
{
	return ( strcmp(s1, s2) <= 0);
}
bool operator>=(const CStringBuffer& s1, const CStringBuffer& s2)
{
	return ( strcmp(s1, s2) >= 0);
}
bool operator>=(const CStringBuffer& s1, const char* s2)
{
	return ( strcmp(s1, s2) >= 0);
}
bool operator>=(const char* s1, const CStringBuffer& s2)
{
	return ( strcmp(s1, s2) >= 0);
}

