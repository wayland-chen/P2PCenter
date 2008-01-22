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

#ifndef _STRING_BUFFER_H
#define _STRING_BUFFER_H

class CStringBuffer
{
public:
	CStringBuffer();
	CStringBuffer(const CStringBuffer& stringSrc);
	CStringBuffer(const char* lpsz);
	CStringBuffer(const unsigned char* psz);

	int GetLength() const;
	bool IsEmpty() const;
	void Empty();

	char GetAt(int nIndex) const;
	char operator[](int nIndex) const;
	void SetAt(int nIndex, char ch);
	operator const char*() const;

	const CStringBuffer& operator=(const CStringBuffer& stringSrc);
	const CStringBuffer& operator=(char ch);
	const CStringBuffer& operator=(const char* lpsz);

	const CStringBuffer& operator+=(const CStringBuffer& string);
	const CStringBuffer& operator+=(char ch);
	const CStringBuffer& operator+=(const char* lpsz);

	int Compare(const char* lpsz) const;
	int CompareNoCase(const char* lpsz) const;

	CStringBuffer Mid(int nFirst, int nCount) const;
	CStringBuffer Mid(int nFirst) const;
	CStringBuffer Left(int nCount) const;
	CStringBuffer Right(int nCount) const;
	void MakeUpper();
	void MakeLower();
	void MakeReverse();

	void TrimRight();
	void TrimLeft();

	int Insert(int nIndex, char ch);
	int Insert(int nIndex, const char* pstr);
	int Delete(int nIndex, int nCount = 1);

	int Find(char ch) const;
	int ReverseFind(char ch) const;
	int Find(char ch, int nStart) const;
	int Find(const char* lpszSub) const;
	int Find(const char* lpszSub, int nStart) const;

	void Format(const char* lpszFormat, ...);
	char * GetBuffer();
private:
	char * GetBuffer(int nLength);
    void SetLength(int nLength);
	
public:
	char * GetBufferSetLength(int nLength);
	

public:
	~CStringBuffer();

private:
	char* 	m_pchData; 
	int 	m_nSize;
	int		m_nLength;

private:
	bool Grow(int nMinSize);

};

//定义操作符重载函数
extern CStringBuffer operator+(const CStringBuffer& string1, const CStringBuffer& string2);
extern CStringBuffer operator+(const CStringBuffer& string, char ch);
extern CStringBuffer operator+(char ch, const CStringBuffer& string);
extern CStringBuffer operator+(const CStringBuffer& string, const char* lpsz);
extern CStringBuffer operator+(const char* lpsz, const CStringBuffer& string);
	
extern bool operator==(const CStringBuffer& s1, const CStringBuffer& s2);
extern bool operator==(const CStringBuffer& s1, const char* s2);
extern bool operator==(const char* s1, const CStringBuffer& s2);
extern bool operator!=(const CStringBuffer& s1, const CStringBuffer& s2);
extern bool operator!=(const CStringBuffer& s1, const char* s2);
extern bool operator!=(const char* s1, const CStringBuffer& s2);
extern bool operator<(const CStringBuffer& s1, const CStringBuffer& s2);
extern bool operator<(const CStringBuffer& s1, const char* s2);
extern bool operator<(const char* s1, const CStringBuffer& s2);
extern bool operator>(const CStringBuffer& s1, const CStringBuffer& s2);
extern bool operator>(const CStringBuffer& s1, const char* s2);
extern bool operator>(const char* s1, const CStringBuffer& s2);
extern bool operator<=(const CStringBuffer& s1, const CStringBuffer& s2);
extern bool operator<=(const CStringBuffer& s1, const char* s2);
extern bool operator<=(const char* s1, const CStringBuffer& s2);
extern bool operator>=(const CStringBuffer& s1, const CStringBuffer& s2);
extern bool operator>=(const CStringBuffer& s1, const char* s2);
extern bool operator>=(const char* s1, const CStringBuffer& s2);

#ifndef __AFXWIN_H__
#ifndef __ATLMISC_H__
	typedef CStringBuffer CString;
#endif
#endif


#endif //#ifndef _STRING_BUFFER_H
