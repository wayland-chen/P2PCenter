
#include "sfc.h"
#include "StringEx.h"

#if defined(_DEBUG)
//#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////
// StringEx

void StringEx::MakeUpper ()
{
   // reverse case of string
   for( iterator p = begin(); p != end(); p++ )
   {
      if( islower( *p ) )
         *p = _toupper( *p );
   }

}


void StringEx::MakeLower ()
{
   // reverse case of string
   for( iterator p = begin(); p != end(); p++ )
   {
      if( isupper( *p ) )
         *p = _tolower( *p );
   }

}


// format a string
void StringEx::Format ( LPCTSTR lpszFormat, ... )
{
	va_list args;
	va_start(args, lpszFormat);

	// buffer for format
	int   len;
	TCHAR buffer[512] = _T("\0");

	// perform format
	len = _vsntprintf(buffer, 512, lpszFormat, args);

	// if there was no error
	if ( len >= 0 )
		(string &) *this = buffer;

	va_end(args);
}


// load string resource

void StringEx::LoadString ( int id )
{
	int   len;
	TCHAR buffer[512];

	// load resource string
	// need to examinie instance

	/*
	#if defined(__AFX_H__)
	HINSTANCE hInstance = AfxGetInstanceHandle();
	#elif defined(__ATLBASE_H__)
	HINSTANCE hInstance = _Module.GetModuleInstance();
	#else
	HINSTANCE hInstance = 0;
	#endif
	*/

	// get default instance
	HINSTANCE hInstance = 0;

	len = 
	::LoadString( hInstance, id, buffer, 512 );
	
	DWORD error = 0;
	if ( len <= 0 )
		error = GetLastError();


	// if there was no error
	if ( len >= 0 )
		(string &) *this = buffer;
}




// trim white space
void StringEx::TrimRight ()
{
	// find last that is not a space
	reverse_iterator last = std::find_if( rbegin(), rend(), std::bind2nd(std::not_equal_to<TCHAR>(),' ') );

	// remove spaces from end (here we use the knowledge that a reverse_terator 
	// is based on an iterator refer include file xstring
	erase( (iterator &) last, end() );
}



int StringEx::Compare(LPCTSTR lpsz) const
{
	// if nothing in string stop
	if ( empty() )
		return -1;

	// do comparison
	int result = compare(lpsz);

	return result;
}

int StringEx::Compare(TCHAR ch) const
{
	// if nothing in string stop
	if ( empty() )
		return -1;

	// do comparison
	TCHAR temp[] = {ch};
	int result = compare(temp);

	return result;
}

int StringEx::CompareNoCase(LPCTSTR lpsz) const
{
	// if nothing in string stop
	if ( empty() )
		return -1;


	return lstrcmpi(begin(), lpsz); 
} 



void StringEx::TrimLeft  ()
{
	// find first that is not space
	iterator last  = std::find_if( begin(), end(), std::bind2nd(std::not_equal_to<TCHAR>(),' ') );

	// remove spaces from start
	erase(begin(),last) ;
}



void StringEx::TrimRight(TCHAR chTarget)
{
	// find last that is not a space
	reverse_iterator last = std::find_if( rbegin(), rend(), std::bind2nd(std::not_equal_to<TCHAR>(),chTarget) );

	// remove spaces from end (here we use the knowledge that a reverse_terator 
	// is based on an iterator refer include file xstring
	erase( (iterator &) last, end() );
}

void StringEx::TrimRight(LPCTSTR lpszTargets)
{
	// find last that is not a space
	reverse_iterator last = std::find_if( rbegin(), rend(), std::bind2nd(std::not_equal_to<TCHAR>(),lpszTargets) );

	// remove spaces from end (here we use the knowledge that a reverse_terator 
	// is based on an iterator refer include file xstring
	erase( (iterator &) last, end() );
}

void StringEx::TrimLeft(TCHAR chTarget)
{
	// find first that is not space
	iterator last  = std::find_if( begin(), end(), std::bind2nd(std::not_equal_to<TCHAR>(),chTarget) );

	// remove spaces from start
	erase(begin(),last) ;
}

void StringEx::TrimLeft(LPCTSTR lpszTargets)
{
	// find first that is not space
	iterator last  = std::find_if( begin(), end(), std::bind2nd(std::not_equal_to<TCHAR>(),lpszTargets) );

	// remove spaces from start
	erase(begin(),last) ;
}


int StringEx::Replace(TCHAR chOld, TCHAR chNew)
{
	// replace if found
	replace( begin(), end(), chOld, chNew );

	return 0;
}

int StringEx::Replace(LPCTSTR lpszOld, LPCTSTR lpszNew)
{
	// replace if found
	replace( begin(), end(), lpszOld, lpszNew );

	return 0;
}

int StringEx::Remove(TCHAR chRemove)
{
	// remove if found
	std::remove_if( begin(), end(), std::bind2nd(std::equal_to<TCHAR>(),chRemove) );


	return 0;
}

int StringEx::Insert(int nIndex, TCHAR ch)
{
	// find start
	iterator start = begin() + nIndex;
	if ( start == end() )
		return 0;

	// insert char
	insert( start, ch );


	return 0;
}

int StringEx::Insert(int nIndex, LPCTSTR pstr)
{
	// find start
	iterator start = begin() + nIndex;
	if ( start == end() )
		return 0;

	// insert string
	insert( nIndex, pstr );

	return 0;
}

int StringEx::Delete(int nIndex, int nCount)
{
	// find start
	iterator start = begin() + nIndex;
	if ( start == end() )
		return 0;

	iterator last = start + nCount;
	if ( last == end() )
		return 0;

	// remove chars
	erase( start, last );

	return 0;
}


void StringEx::allocCopy ( StringEx & dest, int copyLen, int copyIndex,
				            int extraLen ) const
{
	// will clone the data attached to this string
	// allocating 'nExtraLen' characters
	// Places results in uninitialized string 'dest'
	// Will copy the part or all of original data to start of new string

	int newLen = copyLen + extraLen;
	if ( newLen == 0 )
	{	
		dest = "";
	}
	else
	{
		// get allocator for string
		StringEx::allocator_type allocator = 
		dest.get_allocator();

		// setup buffer for copy
		allocator.allocate( newLen, NULL );

		// copy data
		TCHAR * pData = (TCHAR *) c_str();

		memcpy( (TCHAR *) dest.c_str(), 
			    pData + copyIndex, 
				copyLen * sizeof(TCHAR) );
	}
}


StringEx StringEx::Mid(int first) const
{
	return Mid(first, size() - first);
}

StringEx StringEx::Mid(int first, int count) const
{
	// out-of-bounds requests return sensible things
	if (first < 0)
		first = 0;
	if (count < 0)
		count = 0;

	if ( first + count > size() )
		count = size() - first;
	if ( first > size() )
		count = 0;

#ifdef _DEBUG
	if(!(first >= 0))
		throw 1;
	if(!(first + count <= size()))
		throw 1;
#endif	// _DEBUG

	// optimize case of returning entire string
	if ( first == 0 && first + count == size() )
		return *this;

	StringEx dest;
	allocCopy(dest, count, first, 0);
	return dest;
}

StringEx StringEx::Right(int count) const
{
	if (count < 0)
		count = 0;
	if (count >= size())
		return *this;

	StringEx dest;
	allocCopy( dest, count, size()-count, 0 );
	return dest;
}

StringEx StringEx::Left ( int count ) const
{
	if ( count < 0 )
		count = 0;
	if ( count >= size() )
		return *this;

	StringEx dest;
	allocCopy(dest, count, 0, 0);
	return dest;
}

// strspn equivalent
StringEx StringEx::SpanIncluding ( LPCTSTR charSet ) const
{
	// get data
	TCHAR * pData = (TCHAR *) c_str();

	return Left(_tcsspn(pData, charSet));
}

// strcspn equivalent
StringEx StringEx::SpanExcluding ( LPCTSTR charSet ) const
{
	// get data
	TCHAR * pData = (TCHAR *) c_str();

	return Left(_tcscspn(pData, charSet));
}



void StringEx::operator = ( LPWSTR wstr )
{
	// get buffer
	long   len    = wcslen(wstr);
	char * buffer = new char[len+1];
	
	if ( buffer )
	{
		wcstombs( buffer, wstr, len );
		((string &) *this) = buffer;
		delete [] buffer;
	}
}


void StringEx::operator += ( LPWSTR wstr )
{
	// get buffer
	long   len    = wcslen(wstr);
	char * buffer = new char[len+1];
	
	if ( buffer )
	{
		wcstombs( buffer, wstr, len );
		((string &) *this) += buffer;
		delete [] buffer;
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// StringList

void StringList::getEmbeddedStr ( string & embeddedStr )
{
	// clear embedded string
	embeddedStr = "";

	// get ptr to list
	string str;

	// copy list
	for ( iterator item = begin(); item != end(); item++ )
	{
		// get string
		str = *item;

		// copy string
		embeddedStr += str;
		embeddedStr += '\0';
	}

	// add final null terminator
	embeddedStr += '\0';
}



void StringList::setEmbeddedStr ( string & embeddedStr )
{
	// if nothing then move on
	if ( embeddedStr.empty() )
		return;

	// clear list
	if ( !empty() )
		erase(begin(),end());

	appendEmbeddedStr(embeddedStr);
}


void StringList::appendEmbeddedStr ( string & embeddedStr )
{
	// if nothing then move on
	if ( embeddedStr.empty() )
		return;

	// init from embedded string
	LPTSTR start = (LPTSTR) embeddedStr.c_str();
	LPTSTR end   = start + embeddedStr.size();
	for ( LPTSTR str = (LPTSTR) embeddedStr.c_str(); 
	      *str != '\0'|| str > end;  )
	{
		// copy string
		string tmpStr = str;

		// add it
		push_back(tmpStr);

		// go to next string
		int len = _tcslen(str) + 1;
		str += len;
	}
}


void StringList::operator -= ( StringList & list )
{
	// remove strings from our list that are in
	// list
	StringList result;
	result.resize( size() + list.size() );
	std::set_difference( begin(), end(), 
					list.begin(), list.end(),
					result.begin() );

	*this = result;
}


void StringList::operator += ( StringList & list )
{
	// add strings from list to ours
	StringList result;
	result.resize( size() + list.size() );
	std::set_union( begin(), end(),
			   list.begin(), list.end(),
			   result.begin() );

	*this = result;
}


void StringList::operator = ( StringList & list )
{
	// copy strings
	if ( !empty() )
		clear();
	resize( list.size() );
	std::copy( list.begin(), list.end(), begin() );
}

void StringList::removeDuplicates ()
{
	// remove duplicate strings from list
	for ( iterator item = begin();
	      item != end();
		  item++ )
	{

		// get string
		string & str = *item;

		// start search after this string
		iterator srch = item + 1;
		if ( srch == end() )
			continue;

		// search for it, if found remove it
		iterator found = std::find( srch, end(), str );
		if ( found != end() )
			erase(found);

	}
}









