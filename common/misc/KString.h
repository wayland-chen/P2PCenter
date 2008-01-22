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

#ifndef _KSTRING_H
#define _KSTRING_H

#include "../define/kwin32types.h"
#include "../misc/KPtrList.h"
#include <assert.h>

class CKBuffer
{
public:
    CKBuffer( void );
    CKBuffer( const CKBuffer& other );
    CKBuffer( size_t len );
    CKBuffer( CPBYTE pbuf, size_t len );
    ~CKBuffer( void );

    CKBuffer& operator=( const CKBuffer& other );
    BYTE  operator[]( size_t n ) const;
    BYTE& operator[]( size_t n );

    void   Clear( void );

    void   Set( CPBYTE pbuf, size_t len );
    size_t GetSize( void ) const;
    void   SetSize( size_t len );
    PBYTE  GetBuffer( void );
    CPBYTE GetBuffer( void ) const;
    void   SetBuffer( CPBYTE pbuf );

protected:
    size_t m_nAlloc;
    size_t m_nLen;
    PBYTE  m_buf;
};

class CKString
{
public:
    CKString( void );
    CKString( const CKString& other );
    CKString( CPCHAR sz );
    CKString( CPCHAR buf, size_t len );
    CKString( char c, UINT nrep = 1 );
    ~CKString( void );

    CKString& operator=( const CKString& other );
    CKString& operator=( CPCHAR sz );

    void  Set( CPCHAR buf, size_t len );

    UINT    GetLength( void ) const;
    bool    IsEmpty( void ) const;
    int     Compare( const CKString& other ) const;
    int     CompareNoCase( const CKString& other ) const;

    char    GetAt( UINT pos ) const;
    char&   GetAt( UINT pos );
    void    SetAt( UINT pos, char c );
    void    Append( CPCHAR sz );
    void    ToLower( void );
    void    ToUpper( void );
    void    DeQuote( void );

    CPCHAR Find( char c, UINT pos = 0 ) const;

    inline operator CPCHAR( void )      const { assert(m_sz); return m_sz; }
    inline char  operator[]( int pos )  const { return GetAt( pos ); }

protected:
    PCHAR   m_sz;
};
typedef CKPtrList<CKString> CKStringList;

inline bool operator==( const CKString& lhs, const CKString& rhs ) { return (lhs.Compare( rhs ) == 0); }
inline bool operator==( const CKString& lhs, CPCHAR rhs )         { return (lhs.Compare( rhs ) == 0); }
inline bool operator==( CPCHAR lhs, const CKString& rhs )         { return (rhs.Compare( lhs ) == 0); }

inline bool operator!=( const CKString& lhs, const CKString& rhs ) { return (lhs.Compare( rhs ) != 0); }
inline bool operator!=( const CKString& lhs, CPCHAR rhs )         { return (lhs.Compare( rhs ) != 0); }
inline bool operator!=( CPCHAR lhs, const CKString& rhs )         { return (rhs.Compare( lhs ) != 0); }

inline bool operator<=( const CKString& lhs, const CKString& rhs ) { return (lhs.Compare( rhs ) <= 0); }
inline bool operator<=( const CKString& lhs, CPCHAR rhs )         { return (lhs.Compare( rhs ) <= 0); }
inline bool operator<=( CPCHAR lhs, const CKString& rhs )         { return (rhs.Compare( lhs ) > 0); }

inline bool operator>=( const CKString& lhs, const CKString& rhs ) { return (lhs.Compare( rhs ) >= 0); }
inline bool operator>=( const CKString& lhs, CPCHAR rhs )         { return (lhs.Compare( rhs ) >= 0); }
inline bool operator>=( CPCHAR lhs, const CKString& rhs )         { return (rhs.Compare( lhs ) < 0); }

inline bool operator<( const CKString& lhs, const CKString& rhs ) { return (lhs.Compare( rhs ) < 0); }
inline bool operator<( const CKString& lhs, CPCHAR rhs )         { return (lhs.Compare( rhs ) < 0); }
inline bool operator<( CPCHAR lhs, const CKString& rhs )         { return (rhs.Compare( lhs ) >= 0); }

inline bool operator>( const CKString& lhs, const CKString& rhs ) { return (lhs.Compare( rhs ) > 0); }
inline bool operator>( const CKString& lhs, CPCHAR rhs )         { return (lhs.Compare( rhs ) > 0); }
inline bool operator>( CPCHAR lhs, const CKString& rhs )         { return (rhs.Compare( lhs ) <= 0); }

#endif //ndef _KSTRING_H
