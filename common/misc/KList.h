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

#ifndef _KLIST_H
#define _KLIST_H

#include <assert.h>

template <class T>
class CKSingleList
{
private: // Unimplemented
    CKSingleList( const CKSingleList& );
    CKSingleList& operator=( const CKSingleList& );

protected:
    class CKSingleNode
    {
    public:
        CKSingleNode( CKSingleNode* pNext, const T& Data ) :
            m_pNext(pNext),
            m_Data(Data)
        {
            // Empty
        }
        T& GetData( void ) { return m_Data; }
        const T& GetData( void ) const { return m_Data; }

        CKSingleNode* m_pNext;
        T            m_Data;
    };
    class KIteratorBase
    {
        friend class CKSingleList<T>;
    public:
        KIteratorBase( void ) :
            m_pList(NULL),
            m_pNode(NULL)
        {
            // Empty
        }
        bool operator++( void )
        {
            assert( m_pList && m_pNode );
            m_pNode = m_pNode->m_pNext;
            return ( m_pNode != NULL );
        }
        bool operator++( int )
        {
            assert( m_pList && m_pNode );
            m_pNode = m_pNode->m_pNext;
            return ( m_pNode != NULL );
        }
        operator bool( void ) const
        {
            return ( m_pList != NULL && m_pNode != NULL );
        }
        bool operator==( const KIteratorBase& itr ) const
        {
            return ( itr.m_pList == m_pList && itr.m_pNode == m_pNode );
        }
        bool operator!=( const KIteratorBase& itr ) const
        {
            return ( itr.m_pList != m_pList || itr.m_pNode != m_pNode );
        }

    protected:
        KIteratorBase( const CKSingleList<T>* pList, CKSingleNode* pNode ) :
             m_pList(pList),
             m_pNode(pNode)
        {
            // Empty
        }

    protected:
        const CKSingleList<T>* m_pList;
        CKSingleNode*          m_pNode;
    };
public:
    class KIterator : public KIteratorBase
    {
        friend class CKSingleList<T>;
    public:
        KIterator( void ) : KIteratorBase()
        {
            // Empty
        }
        T& operator*( void )
        {
            assert( m_pList && m_pNode );
            return m_pNode->GetData();
        }
    protected:
        KIterator( CKSingleList<T>* pList, CKSingleNode* pNode ) :
            KIteratorBase( pList, pNode )
        {
            // Empty
        }
    };
    class KConstIterator : public KIteratorBase
    {
        friend class CKSingleList<T>;
    public:
        KConstIterator( void ) : KIteratorBase()
        {
            // Empty
        }
        const T& operator*( void ) const
        {
            assert( m_pList && m_pNode );
            return m_pNode->GetData();
        }
    protected:
        KConstIterator( const CKSingleList<T>* pList, CKSingleNode* pNode ) :
            KIteratorBase( pList, pNode )
        {
            // Empty
        }
    };

    CKSingleList( void ) : m_pHead(NULL), m_pTail(NULL), m_nCount(0) {}
    ~CKSingleList( void )
    {
        while( ! IsEmpty() )
        {
            RemoveHead();
        }
    }

    bool IsEmpty( void ) const { return m_pHead == NULL; }
    unsigned int GetCount( void ) const { return m_nCount; }
    void InsertHead( const T& Data )
    {
        CKSingleNode* pNode = new CKSingleNode( m_pHead, Data );
        assert( pNode );
        m_pHead = pNode;
        if( ! m_pTail )
        {
            m_pTail = pNode;
        }
        m_nCount++;
    }
    void InsertTail( const T& Data )
    {
        CKSingleNode* pNode = new CKSingleNode( NULL, Data );
        assert( pNode );
        if( m_pTail )
        {
            m_pTail->m_pNext = pNode;
        }
        m_pTail = pNode;
        if( ! m_pHead )
        {
            m_pHead = pNode;
        }
        m_nCount++;
    }
    T RemoveHead( void )
    {
        CKSingleNode* pNode = m_pHead;
        m_pHead = pNode->m_pNext;
        if( ! m_pHead )
        {
            m_pTail = NULL;
        }
        T Data = pNode->GetData();
        delete pNode;
        m_nCount--;
        return Data;
    }

    KIterator      Begin( void )       { return KIterator( this, m_pHead ); }
    KConstIterator Begin( void ) const { return KConstIterator( this, m_pHead ); }
    KIterator      End( void )       { return KIterator( this, NULL ); }
    KConstIterator End( void ) const { return KConstIterator( this, NULL ); }

protected:
    CKSingleNode* m_pHead;
    CKSingleNode* m_pTail;
    unsigned int m_nCount;
};

template <class T>
class CKDoubleList
{
private: // Unimplemented
    CKDoubleList( const CKDoubleList& );
    CKDoubleList& operator=( const CKDoubleList& );

protected:
    class KDoubleNode
    {
    public:
         KDoubleNode( KDoubleNode* pPrev, KDoubleNode* pNext, const T& Data ) :
            m_pPrev(pPrev),
            m_pNext(pNext),
            m_Data(Data)
        {
            // Empty
        }
        T& GetData( void ) { return m_Data; }
        const T& GetData( void ) const { return m_Data; }

        KDoubleNode* m_pPrev;
        KDoubleNode* m_pNext;
        T            m_Data;
    };
    class KIteratorBase
    {
        friend class CKDoubleList<T>;
    public:
        KIteratorBase( void ) :
            m_pList(NULL),
            m_pNode(NULL)
        {
            // Empty
        }
        bool operator++( void )
        {
            assert( m_pList && m_pNode );
            m_pNode = m_pNode->m_pNext;
            return ( m_pNode != NULL );
        }
        bool operator++( int )
        {
            assert( m_pList && m_pNode );
            m_pNode = m_pNode->m_pNext;
            return ( m_pNode != NULL );
        }
        bool operator--( void )
        {
            assert( m_pList && m_pNode );
            m_pNode = m_pNode->m_pPrev;
            return ( m_pNode != NULL );
        }
        bool operator--( int )
        {
            assert( m_pList && m_pNode );
            m_pNode = m_pNode->m_pNext;
            return ( m_pNode != NULL );
        }
        operator bool( void ) const
        {
            return ( m_pList != NULL && m_pNode != NULL );
        }
        bool operator==( const KIteratorBase& itr ) const
        {
            return ( itr.m_pList == m_pList && itr.m_pNode == m_pNode );
        }
        bool operator!=( const KIteratorBase& itr ) const
        {
            return ( itr.m_pList != m_pList || itr.m_pNode != m_pNode );
        }

    protected:
        KIteratorBase( const CKDoubleList<T>* pList, KDoubleNode* pNode ) :
             m_pList(pList),
             m_pNode(pNode)
        {
            // Empty
        }

    protected:
        const CKDoubleList<T>* m_pList;
        KDoubleNode*          m_pNode;
    };
public:
    class KIterator : public KIteratorBase
    {
        friend class CKDoubleList<T>;
    public:
        KIterator( void ) : KIteratorBase()
        {
            // Empty
        }
        T& operator*( void )
        {
            assert( m_pList && m_pNode );
            return m_pNode->GetData();
        }
    protected:
        KIterator( const CKDoubleList<T>* pList, KDoubleNode* pNode ) :
            KIteratorBase( pList, pNode )
        {
            // Empty
        }
    };
    class KConstIterator : public KIteratorBase
    {
        friend class CKDoubleList<T>;
    public:
        KConstIterator( void ) : KIteratorBase()
        {
            // Empty
        }
        const T& operator*( void ) const
        {
            assert( m_pList && m_pNode );
            return m_pNode->GetData();
        }
    protected:
        KConstIterator( const CKDoubleList<T>* pList, KDoubleNode* pNode ) :
            KIteratorBase( pList, pNode )
        {
            // Empty
        }
    };

    CKDoubleList( void ) : m_pHead(NULL), m_pTail(NULL), m_nCount(0) {}
    ~CKDoubleList( void )
    {
        while( ! IsEmpty() )
        {
            RemoveHead();
        }
    }

    bool IsEmpty( void ) const { return m_pHead == NULL; }
    unsigned int GetCount( void ) const { return m_nCount; }
    void InsertHead( const T& Data )
    {
        KDoubleNode* pNode = new KDoubleNode( NULL, m_pHead, Data );
        assert( pNode );
        if( m_pHead )
        {
            m_pHead->m_pPrev = pNode;
        }
        m_pHead = pNode;
        if( ! m_pTail )
        {
            m_pTail = pNode;
        }
        m_nCount++;
    }
    void InsertTail( const T& Data )
    {
        KDoubleNode* pNode = new KDoubleNode( m_pTail, NULL, Data );
        assert( pNode );
        if( m_pTail )
        {
            m_pTail->m_pNext = pNode;
        }
        m_pTail = pNode;
        if( ! m_pHead )
        {
            m_pHead = pNode;
        }
        m_nCount++;
    }
    void InsertBefore( const KIterator& itr, const T& Data )
    {
        assert( this == itr.m_pList && NULL != itr.m_pNode );
        assert( m_pHead && m_pTail && m_nCount );
        KDoubleNode* pNode = new KDoubleNode( NULL, itr.m_pNode, Data );
        assert( pNode );
        if( itr.m_pNode->m_pPrev )
        {
            itr.m_pNode->m_pPrev->m_pNext = pNode;
            pNode->m_pPrev = itr.m_pNode->m_pPrev;
        }
        else
        {
            assert( m_pHead == itr.m_pNode );
            m_pHead = pNode;
        }
        itr.m_pNode->m_pPrev = pNode;
        m_nCount++;
    }
    void InsertAfter( const KIterator& itr, const T& Data )
    {
        assert( this == itr.m_pList && NULL != itr.m_pNode );
        assert( m_pHead && m_pTail && m_nCount );
        KDoubleNode* pNode = new KDoubleNode( itr.m_pNode, NULL, Data );
        assert( pNode );
        if( itr.m_pNode->m_pNext )
        {
            itr.m_pNode->m_pNext->m_pPrev = pNode;
            pNode->m_pNext = itr.m_pNode->m_pNext;
        }
        else
        {
            assert( m_pTail == itr.m_pNode );
            m_pTail = pNode;
        }
        itr.m_pNode->m_pNext = pNode;
        m_nCount++;
    }
    T Remove( KIterator& itr )
    {
        assert( this == itr.m_pList && NULL != itr.m_pNode );
        assert( m_pHead && m_pTail && m_nCount );
        KDoubleNode* pNode = itr.m_pNode;
        itr.m_pNode = NULL;
        if( pNode->m_pPrev )
        {
            pNode->m_pPrev->m_pNext = pNode->m_pNext;
        }
        else
        {
            assert( m_pHead == pNode );
            m_pHead = pNode->m_pNext;
        }
        if( pNode->m_pNext )
        {
            pNode->m_pNext->m_pPrev = pNode->m_pPrev;
        }
        else
        {
            assert( m_pTail == pNode );
            m_pTail = pNode->m_pPrev;
        }
        T Data = pNode->GetData();
        delete pNode;
        m_nCount--;
        return Data;
    }
    T RemoveHead( void )
    {
        assert( m_pHead && m_pTail && m_nCount );
        KDoubleNode* pNode = m_pHead;
        m_pHead = pNode->m_pNext;
        if( ! m_pHead )
        {
            m_pTail = NULL;
        }
        T Data = pNode->GetData();
        delete pNode;
        m_nCount--;
        return Data;
    }
    T RemoveTail( void )
    {
        assert( m_pHead && m_pTail && m_nCount );
        KDoubleNode* pNode = m_pTail;
        m_pTail = pNode->m_pPrev;
        if( ! m_pTail )
        {
            m_pHead = NULL;
        }
        T Data = pNode->GetData();
        delete pNode;
        m_nCount--;
        return Data;
    }

    KIterator      Begin( void )       { return KIterator( this, m_pHead ); }
    KConstIterator Begin( void ) const { return KConstIterator( this, m_pHead ); }
    KIterator      End( void )       { return KIterator( this, NULL ); }
    KConstIterator End( void ) const { return KConstIterator( this, NULL ); }

    protected:
        KDoubleNode* m_pHead;
        KDoubleNode* m_pTail;
        unsigned int m_nCount;
};

#endif  // _KLIST_H
