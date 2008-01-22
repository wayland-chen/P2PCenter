#if !defined(StringEx_H)
#define StringEx_H

// disable long template name warning
#pragma warning( disable:4786 )
#pragma warning( disable:4172 )

#if !defined(_WINDOWS_)
///#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <tchar.h>
#endif

#include "sfc.h"

#define	SS_NO_REFCOUNT
#include "stdstring.h"

typedef CStdStr<TCHAR>	StringEx;

#include "stringlist.h"

#if 0

#include <string>
#include <algorithm>
#include <functional>
#include <vector>

// using std::string;

/*
#if defined(UNICODE)
#undef string
#define	string wstring
#endif
*/

//////////////////////////////////////////////////////////////////////////////
// StringEx
//
// Purpose:		to extend the stl class string


class SFC_CLASS_STG StringEx :
	public string
{
public:

	StringEx () :
	  string ()
	{}

	StringEx ( TCHAR ch ) :
	  string ( "" )
	{ *this = ch; }

	StringEx ( LPCTSTR str ) :
	  string ( str )
	{}

	StringEx ( LPCTSTR str, int len ) :
	  string (str,len)
	{}

	StringEx ( LPWSTR wstr ) :
	  string ()
		{ *this = wstr; }

	StringEx ( string & str ) :
	  string ( str )
	{}

	StringEx ( const StringEx & str ) :
	  string ( str )
	{}

	virtual ~StringEx ()
	{}

	// release resources
	void release ()
	{
		if ( !empty() )
			erase(begin(),end());
	}

	// test if empty
	bool IsEmpty () const
	{ return empty(); }

	// test if empty
	void Empty ()
	{ erase(begin(),end()); }

	// get element
	TCHAR GetAt ( long index )
	{ return ( (string &) *this)[index]; }

	long GetLength ()  const
	{ return size(); }

	LPTSTR GetBufferSetLength ( long index )
	{ return (LPTSTR) c_str(); }

	LPTSTR GetBuffer ( long index )
	{ return (LPTSTR) c_str(); }

	long Find ( LPCTSTR str )
	{ return find(str); }

	long Find ( LPCTSTR str ) const
	{ return find(str); }

	long Find ( TCHAR str )
	{ return find(str); }

	long Find ( TCHAR str ) const
	{ return find(str); }

	long ReverseFind ( LPCTSTR str )
	{ return rfind(str); }

	long ReverseFind ( LPCTSTR str ) const
	{ return rfind(str); }

	long ReverseFind ( TCHAR str )
	{ return rfind(str); }

	long ReverseFind ( TCHAR str ) const
	{ return rfind(str); }



	// operators
	operator string &   ()					{ return (string &) *this; }
	operator StringEx & ()					{ return *this; }
	operator LPCTSTR    () const			{ return c_str(); }
	operator LPTSTR     ()					{ return (LPTSTR) c_str(); }

	void operator =    ( LPCTSTR str )		{ (string &) *this = str; }
	void operator =    ( LPTSTR str )		{ (string &) *this = str; }
	void operator =    ( TCHAR str )		{ (string &) *this = str; }
	void operator =	   ( string & str )		{ (string &) *this = str; }
	//void operator =	   ( StringEx & str )	{ (string &) *this = (string &) str; }
	void operator =	   ( const StringEx & str )	{ (string &) *this = (string &) str; }
	void operator =    ( LPWSTR str );
	void operator +=   ( LPWSTR str );

	TCHAR operator [] ( int index )
	{ return (TCHAR) ( (string &) *this )[index]; }

	/*
	void operator +=   ( StringEx & str )
	{ ( (string &) *this ) += (string &) str; }

	void operator +=   ( LPTSTR str )
	{ ( (string &) *this ) += str; }

	void operator +=   ( TCHAR str )
	{ ( (string &) *this ) += str; }
	*/

	StringEx & operator+= ( LPCTSTR lpsz )
	{
		long   len    = strlen(lpsz);
		char * buffer = new char[len+1];
		
		if ( buffer )
		{
			strncpy( buffer, lpsz, len +1 );
			((string &) *this) += buffer;
			delete [] buffer;
		}

		return *this;
	}

	StringEx & operator+= ( const StringEx & str )
	{
		*this += str.c_str();
		return *this;
	}

	StringEx & operator+= ( const string & str )
	{
		*this += str.c_str();
		return *this;
	}

	StringEx & operator+= ( TCHAR ch )
	{
		StringEx	str(ch);
		*this += str.c_str();
		return *this;
	}

	/*
	StringEx & operator+ ( const StringEx & str )
	{
		*this + str;
		return *this;
	}

	StringEx & operator+ ( const string & str )
	{
		return (StringEx &) append(str);
	}

	StringEx & operator+ ( TCHAR ch)
	{
		*this + ch;
		return *this;
	}

	StringEx & operator+ ( LPCTSTR lpsz)
	{
		*this + lpsz;
		return *this;
	}
	*/

	// format a string
	void Format ( LPCTSTR lpszFormat, ... );

	// load string resource
	void LoadString	( int id );

	// straight character comparison
	int Compare(LPCTSTR lpsz) const;
	int Compare(TCHAR lpsz) const;

	// compare ignoring case
	int CompareNoCase(LPCTSTR lpsz) const;

	// trim white space
	void TrimRight ();
	void TrimLeft  ();

	// remove continuous occurrences of chTarget starting from right
	void TrimRight(TCHAR chTarget);
	// remove continuous occcurrences of characters in passed string,
	// starting from right
	void TrimRight(LPCTSTR lpszTargets);
	// remove continuous occurrences of chTarget starting from left
	void TrimLeft(TCHAR chTarget);
	// remove continuous occcurrences of characters in
	// passed string, starting from left
	void TrimLeft(LPCTSTR lpszTargets);

	// advanced manipulation

	// replace occurrences of chOld with chNew
	int Replace(TCHAR chOld, TCHAR chNew);
	// replace occurrences of substring lpszOld with lpszNew;
	// empty lpszNew removes instances of lpszOld
	int Replace(LPCTSTR lpszOld, LPCTSTR lpszNew);
	// remove occurrences of chRemove
	int Remove(TCHAR chRemove);
	// insert character at zero-based index; concatenates
	// if index is past end of string
	int Insert(int nIndex, TCHAR ch);
	// insert substring at zero-based index; concatenates
	// if index is past end of string
	int Insert(int nIndex, LPCTSTR pstr);
	// delete nCount characters starting at zero-based index
	int Delete(int nIndex, int nCount = 1);


	// allocat and copy
	void		allocCopy		( StringEx & dest, int copyLen, int copyIndex,
								  int extraLen ) const;

	StringEx	Mid				( int first ) const;
	StringEx	Mid				( int first, int count ) const;
	StringEx	Right			( int count ) const;
	StringEx	Left			( int count ) const;
	StringEx	SpanIncluding	( LPCTSTR charSet ) const;
	StringEx	SpanExcluding	( LPCTSTR charSet ) const;

	// case methods
	void MakeUpper			();
	void MakeLower			();

	// friends
	friend StringEx & operator+ ( const StringEx & str2, const StringEx & str1 );
	friend StringEx & operator+ ( const StringEx & str2, const string & str1 );
	friend StringEx & operator+ ( const StringEx & str2, TCHAR ch );
	friend StringEx & operator+ ( const StringEx & str2, LPCTSTR lpsz);





};

// compare helpers
inline bool operator==(const StringEx& s1, const StringEx& s2)
{ return s1.Compare(s2) == 0; }

inline bool operator==(const StringEx& s1, LPCTSTR s2)
{ return s1.Compare(s2) == 0; }

inline bool operator==(LPCTSTR s1, const StringEx& s2)
{ return s2.Compare(s1) == 0; }

inline bool operator!=(const StringEx& s1, const StringEx& s2)
{ return s1.Compare(s2) != 0; }

inline bool operator!=(const StringEx& s1, LPCTSTR s2)
{ return s1.Compare(s2) != 0; }


inline bool operator!=(const LPCTSTR & s1, const StringEx& s2)
{ return s2.Compare(s1) != 0; }

inline bool operator<(const StringEx& s1, const StringEx& s2)
{ return s1.Compare(s2) < 0; }

inline bool operator<(const StringEx& s1, LPCTSTR s2)
{ return s1.Compare(s2) < 0; }

inline bool operator<(LPCTSTR s1, const StringEx& s2)
{ return s2.Compare(s1) > 0; }

inline bool operator>(const StringEx& s1, const StringEx& s2)
{ return s1.Compare(s2) > 0; }

inline bool operator>(const StringEx& s1, LPCTSTR s2)
{ return s1.Compare(s2) > 0; }


inline bool operator>(LPCTSTR s1, const StringEx& s2)
{ return s2.Compare(s1) < 0; }

inline bool operator<=(const StringEx& s1, const StringEx& s2)
{ return s1.Compare(s2) <= 0; }

inline bool operator<=(const StringEx& s1, LPCTSTR s2)
{ return s1.Compare(s2) <= 0; }

inline bool operator<=(LPCTSTR s1, const StringEx& s2)
{ return s2.Compare(s1) >= 0; }

inline bool operator>=(const StringEx& s1, const StringEx& s2)
{ return s1.Compare(s2) >= 0; }

inline bool operator>=(const StringEx& s1, LPCTSTR s2)
{ return s1.Compare(s2) >= 0; }

inline bool operator>=(LPCTSTR s1, const StringEx& s2)
{ return s2.Compare(s1) <= 0; }


inline
StringEx & operator+ ( const StringEx & str2, const StringEx & str1 )
{
	StringEx str = str2 + str1.c_str();
	return StringEx(str);
}

inline
StringEx & operator+ ( const StringEx & str2, const string & str1 )
{
	// get buffer
	long   len    = str1.size();
	char * buffer = new char[len+1];
	
	if ( buffer )
	{
		_tcsncpy( buffer, str1.c_str(), len + 1 );
		((string &) *this) += buffer;
		delete [] buffer;
	}

	StringEx str = str2 + str1.c_str();
	return StringEx(str);
}

inline
StringEx & operator+ ( const StringEx & str2, TCHAR ch )
{
	str2.append(1, ch);
	StringEx	c(ch);
	StringEx str = str2 + c.c_str();
	return StringEx(str);
}

inline
StringEx & operator+ ( const StringEx & str2, LPCTSTR lpsz)
{
	StringEx str = str2 + StringEx(lpsz);
	return StringEx(str);
}


//////////////////////////////////////////////////////////////////////////////////////
// StringList


class StringList :
	public std::vector<StringEx>
{
public:

	StringList () :
	  std::vector<StringEx> ()
	{}

	StringList ( int noElements ) :
	  std::vector<StringEx> (noElements)
	{}

	virtual ~StringList ()
	{ release(); }


	void release ()
	{
		if ( !empty() )
			erase(begin(),end());
	}

	// test if empty
	bool IsEmpty ()
	{ return empty(); }

	// get list size
	long GetSize ()
	{ return size(); }

	// get element
	string GetAt ( long index )
	{ return ( (std::vector<string> &) *this)[index]; }

	// clear list
	void RemoveAll ()
	{ clear(); }

	// embedded string methods
	void getEmbeddedStr		( string & embeddedStr );
	void setEmbeddedStr		( string & embeddedStr );
	void appendEmbeddedStr	( string & embeddedStr );

	// remove duplicates from list
	void removeDuplicates	();

	// list manipulation methods
	void operator -=		( StringList & list );
	void operator +=		( StringList & list );
	void operator  =		( StringList & list );

	// add a string
	void operator << (  const string & depends )
	{
		push_back( depends );
	}

	void operator << ( LPTSTR depends )
	{
		string dependsStr = depends;
		push_back( dependsStr );
	}


	void Add ( LPTSTR depends )
	{
		string dependsStr = depends;
		push_back( dependsStr );
	}

	void Add ( string & depends )
	{
		push_back( depends );
	}

};


	typedef std::vector<string>::iterator itrString;





//////////////////////////////////////////////////////////////////////////////////////
// UIntList


class UIntList :
	public std::vector<UINT>
{
public:

	UIntList () :
	  std::vector<UINT> ()
	{}

	UIntList ( int noElements ) :
	  std::vector<UINT> (noElements)
	{}

	virtual ~UIntList ()
	{ release(); }


	void release ()
	{
		if ( !empty() )
			erase(begin(),end());
	}

	// clear list
	void RemoveAll ()
	{ clear(); }

	// get list size
	long GetSize ()
	{ return size(); }

	// get element
	UINT GetAt ( long index )
	{ return ( (std::vector<UINT> &) *this)[index]; }


	// list manipulation methods
	void operator -=		( UIntList & list );
	void operator +=		( UIntList & list );
	void operator  =		( UIntList & list );

	// add a string
	void operator << (  const UINT value )
	{
		push_back( value );
	}


	void Add ( UINT value )
	{
		push_back( value );
	}

};


	typedef std::vector<UINT>::iterator itrUInt;


#endif // 0

#endif