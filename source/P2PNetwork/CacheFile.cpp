#include "StdAfx.h"
#include ".\cachefile.h"

CCacheBlock::CCacheBlock(void) :
m_dwLastVisit(GetTickCount())
{
}

CCacheBlock::~CCacheBlock(void)
{
}

//------------------------------------------------

CCacheFile::CCacheFile(void)
{
}

CCacheFile::~CCacheFile(void)
{
	map<CCacheBlock *, CCacheBlock *>::iterator it = m_Blocks.begin();
	while ( it != m_Blocks.end() )
	{
		delete it->second;
		it = m_Blocks.erase(it);
	}
}

void CCacheFile::DeleteOverTimeBlock()
{
	DWORD dwLastVisitTime = GetTickCount();

	if ( m_Blocks.size() > RESERVE_BLOCK_NUM )
	{
		CCacheBlock *pDelBlock = NULL;
		map<CCacheBlock *, CCacheBlock *>::iterator it = m_Blocks.begin();
		while ( it != m_Blocks.end() )
		{
			if ( it->second->m_dwLastVisit < dwLastVisitTime)
			{
				dwLastVisitTime = it->second->m_dwLastVisit;
				pDelBlock = it->second;
			}
			it ++;
		}

		if ( pDelBlock )
		{
			it = m_Blocks.find( pDelBlock);
			if ( it != m_Blocks.end())
			{
				m_Blocks.erase( it);

				// 间断记录一次日志
				static DWORD dwTempTime = GetTickCount();
				if ( dwTempTime + 5000 < GetTickCount() )
				{
					CKLog::WriteLog( LOG_TYPE_DEBUG, "删除缓存块，开始点： %u， 最后访问时间差：%u", 
						pDelBlock->m_dwStartPos, GetTickCount() - pDelBlock->m_dwLastVisit);
					dwTempTime = GetTickCount();
				}

				delete pDelBlock;
			}
		}
	}
}

void CCacheFile::PushBlock( DWORD dwStartPos, char* pBuffer, DWORD dwLen)
{
	if ( BLOCK_LENGTH == dwLen )
	{
		// 删除超出保留的块
		DeleteOverTimeBlock();

		CCacheBlock* pBlock = new CCacheBlock();
		if ( pBlock)
		{
			pBlock->m_dwStartPos = dwStartPos;
			memcpy( pBlock->m_szData, pBuffer, BLOCK_LENGTH);
			m_Blocks[pBlock] = pBlock;
		}
	}
}

DWORD CCacheFile::GetData( DWORD dwStartPos, char* pBuffer, DWORD dwLen)
{
	map<CCacheBlock *, CCacheBlock *>::iterator it = m_Blocks.begin();
	while ( it != m_Blocks.end() )
	{
		if ( it->second->m_dwStartPos <= dwStartPos && 
			(dwStartPos + dwLen) <= (it->second->m_dwStartPos + BLOCK_LENGTH) )
		{
			it->second->m_dwLastVisit = GetTickCount();
			DWORD dwPos = dwStartPos - it->second->m_dwStartPos;
			memcpy( pBuffer, it->second->m_szData + dwPos, dwLen);

			// 间断记录一次日志
			static DWORD dwTempTime = GetTickCount();
			if ( dwTempTime + 5000 < GetTickCount() )
			{
				CKLog::WriteLog( LOG_TYPE_DEBUG, "从缓存读取,绝对开始点: %u, 相对开始点: %u, 读取长度: %u, 大块开始点: %u", 
					dwStartPos, dwPos, dwLen, it->second->m_dwStartPos);
				dwTempTime = GetTickCount();
			}

			return dwLen;
		}
		it ++;
	}

	return 0;
}

