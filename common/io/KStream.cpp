
#include "../misc/KString.h"
#include "../define/kwin32types.h"
#include "../misc/KDebug.h"
#include "KStream.h"

CKStream::CKStream( void ) :
    m_pNotify(NULL)
{
    // Empty
}

CKStream::CKStream( CKStreamNotify* pNotify ) :
    m_pNotify(pNotify)
{
    // Empty
}

CKStream::~CKStream( void )
{
    // Empty
}

void CKStream::SetNotify( CKStreamNotify* pNotify )
{
    m_pNotify = pNotify;
}

bool CKStream::Read( CKBuffer* pbuf )
{
    assert( pbuf );
    assert( pbuf->GetBuffer() != NULL && pbuf->GetSize() > 0 );

    size_t n = Read( pbuf->GetBuffer(), pbuf->GetSize() );
    pbuf->SetSize( n );
    return ( n > 0 );
}

bool CKStream::Write( const CKBuffer& buf )
{
    assert( buf.GetBuffer() != NULL && buf.GetSize() > 0 );

    size_t n = Write( buf.GetBuffer(), buf.GetSize() );
    return ( n > 0 );
}
