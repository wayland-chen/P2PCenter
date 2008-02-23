#include "StdAfx.h"
#include ".\channel.h"
#include "P2PApp.h"
#include "misc/MD5.h"

CChannel::CChannel(void) : 
m_bSource(false),
m_llFileSize(0)
{
	memset( m_szHashID, 0, sizeof(m_szHashID) );
}

CChannel::~CChannel(void)
{
	g_pNetwork->PushRequestCmd( P2S_COMMAND::CCommandFactory::CreateLogoutChannelCmd( g_pNetwork->UserID(), g_pNetwork->SessionID(), m_sID) );

	if ( m_file.IsOpen() )
	{
		m_file.Close();
	}
}

void CChannel::AddPeer( PEERINFO& peerinfo )
{
	CKAutoLock l( m_peerMgr.GetCritSec() );

	CP2PPeer* pPeer = m_peerMgr.FindNoLock( peerinfo.ulID );
	if ( pPeer )
	{
		if ( pPeer->m_peerInfo.ulExternalIP != peerinfo.ulExternalIP || pPeer->m_peerInfo.usExternalUDPPort != peerinfo.usExternalUDPPort )
		{
			char szOldIP[256];
			strcpy( szOldIP, inet_ntoa( *(in_addr *)(&pPeer->m_peerInfo.ulExternalIP) ) );
			printf("userid: %u, old ip: %s, newip: %s, oldport: %u, newport: %u\n", 
				peerinfo.ulID, szOldIP, inet_ntoa( *(in_addr *)(&peerinfo.ulExternalIP) ),
				pPeer->m_peerInfo.usExternalUDPPort, peerinfo.usExternalUDPPort );

			//pPeer->m_peerInfo.ulExternalIP = peerinfo.ulExternalIP;
			//pPeer->m_peerInfo.usExternalUDPPort = peerinfo.usExternalUDPPort;
		}
		// handshake
		P2P_COMMAND::CP2PBaseCommand* pCmd = P2P_COMMAND::CP2PCmdFactory::CreateHandshakeCmd( g_pNetwork->UserID(), g_pNetwork->GetRandSession() );
		if ( pCmd )
		{
			pCmd->m_dwIP = peerinfo.ulExternalIP;
			pCmd->m_usPort = peerinfo.usExternalUDPPort;
			g_pNetwork->PushUdpPeerCmd( pCmd );
		}
		// 定向打孔 ffwk
		g_pNetwork->PushRequestCmd( P2S_COMMAND::CCommandFactory::CreateReqShootToCmd( g_pNetwork->UserID(), g_pNetwork->GetRandSession(), pPeer->m_peerInfo.ulID ) );
	}
	else
	{
		switch( peerinfo.ucSource) 
		{
		case PEER_TYPE_K8_GENERAL:
			pPeer = new CP2PPeerK8General();
			break;

		case PEER_TYPE_K8_SOURCE:
			pPeer = new CP2PPeerK8Source();
			break;

		case PEER_TYPE_HTTP:
			pPeer = new CP2PPeerHttp();
			break;

		case PEER_TYPE_FTP:
			pPeer = new CP2PPeerFtp();
			break;

		case PEER_TYPE_BITTORRENT:
			pPeer = new CP2PPeerBittorrent();
			break;

		case PEER_TYPE_EDONKEY:
			pPeer = new CP2PPeerEdonkey();
			break;

		default:
			break;
		}

		if ( pPeer )
		{
			pPeer->m_peerInfo = peerinfo;
			m_peerMgr.InsertNoLock( peerinfo.ulID, pPeer );
			// handshake
			P2P_COMMAND::CP2PBaseCommand* pCmd = P2P_COMMAND::CP2PCmdFactory::CreateHandshakeCmd( g_pNetwork->UserID(), g_pNetwork->GetRandSession() );
			if ( pCmd )
			{
				pCmd->m_dwIP = peerinfo.ulExternalIP;
				pCmd->m_usPort = peerinfo.usExternalUDPPort;
				g_pNetwork->PushUdpPeerCmd( pCmd );
			}
			// 定向打孔 ffwk
			g_pNetwork->PushRequestCmd( P2S_COMMAND::CCommandFactory::CreateReqShootToCmd( g_pNetwork->UserID(), g_pNetwork->GetRandSession(), pPeer->m_peerInfo.ulID ) );
		}
	}
}

PEERINFO CChannel::DeletePeer( const DWORD ulID )
{
	PEERINFO peerinfo;
	CP2PPeer* pPeer = m_peerMgr.Erase( ulID );
	if ( pPeer )
	{
		peerinfo = pPeer->m_peerInfo;
		delete pPeer;
	}

	return peerinfo;
}

void CChannel::RequestSegment( DWORD dwStartPos, DWORD dwLength )
{
	if ( m_bSource )
		return;

	DWORD dwFileSize = m_llFileSize;
	if ( dwStartPos > dwFileSize)	return;
	if ( dwStartPos + dwLength > dwFileSize)
		dwLength = dwFileSize - dwStartPos;

	DWORD dwStartBlock = dwStartPos / UDP_PACKET_LENGTH;
	DWORD dwBlockNum = ((dwStartPos + dwLength) - dwStartBlock * UDP_PACKET_LENGTH + (UDP_PACKET_LENGTH - 1)) / UDP_PACKET_LENGTH;

	if ( dwStartBlock > 10 * 1000)
		m_packetMgr.RemoveBlock( dwStartBlock - 10 * 1000, dwStartBlock + dwBlockNum );

	m_packetMgr.InsertShouldBlock( dwStartBlock, dwBlockNum );
}

bool CChannel::ReadSegment( DWORD dwStartPos, char* pBuffer, DWORD& dwLength )
{
	if ( m_bSource )
	{
		if ( dwLength < BLOCK_LENGTH)
		{
			DWORD dwLen = m_cacheFile.GetData( dwStartPos, pBuffer, dwLength);
			if ( dwLen == dwLength)
				return true;
		}

		if ( m_file.IsOpen() )
		{
			if ( m_file.Seek( dwStartPos ) )
			{
				char szBuffer[BLOCK_LENGTH];
				memset( szBuffer, 0, sizeof(szBuffer));
				DWORD dwLen = 0;
				if ( (dwLen = (DWORD)m_file.Read( szBuffer, sizeof(szBuffer) )) > 0 )
				{
					if ( BLOCK_LENGTH == dwLen )
					{
						m_cacheFile.PushBlock( dwStartPos, szBuffer, dwLen);
						dwLength = min(dwLength, dwLen);
						memcpy( pBuffer, szBuffer, dwLength);
					}
					else
					{
						dwLength = min(dwLength, dwLen);
						memcpy( pBuffer, szBuffer, dwLength);
					}
					return true;
				}
			}
		}

		return false;
	}

	return m_packetMgr.ReadSegment( dwStartPos, pBuffer, dwLength );
}

bool CChannel::OpenSource( const char* szNodeName, const char* szFilePath, const char* szFileName, const char* szFileHash, const LONGLONG llFileSize, bool bSource )
{
	m_bSource = bSource;
	m_sChannelName = szFileName;

	if ( m_bSource )
	{
		// 打开本地文件
		if ( m_file.IsOpen() )
			m_file.Close();

		if ( !m_file.Open( szFilePath ) )
			return false;

		m_llFileSize = m_file.Size();
		if ( m_llFileSize != llFileSize )
		{
			CKLog::WriteLog( LOG_TYPE_DEBUG, "FileSize不一致, 实际: %I64u, 传入: %I64u", m_llFileSize, llFileSize );
		}
	}

	SetID( szFileHash );
	m_llFileSize = llFileSize;
	char szFileSize[32];
	sprintf(szFileSize, "%I64u", llFileSize );
	g_pNetwork->PushRequestCmd( P2S_COMMAND::CCommandFactory::CreateLoginChannelCmd( 
		g_pNetwork->UserID(), g_pNetwork->SessionID(), szNodeName, m_sID, bSource, szFilePath, szFileName, szFileSize) );

	// 源也要向服务器查询PEER列表，查询以前加入进来的PEER
	g_pNetwork->PushRequestCmd( P2S_COMMAND::CCommandFactory::CreateGetPeersCmd( g_pNetwork->UserID(), g_pNetwork->SessionID(), m_sID, m_bSource ) );

	return true;
}

string CChannel::GetID()
{
	return m_sID;
}

void CChannel::SetID( string sID )
{
	m_sID = sID;
	MD5 md5;
	if ( CMD5::HashFromString( m_sID.c_str(), &md5 ) )
	{
		memcpy( m_szHashID, &md5, sizeof(m_szHashID) );
	}
}

void CChannel::Loop()
{
	if ( !m_bSource )
	{
		m_peerMgr.QueryExistBlocks( m_szHashID );	
		m_peerMgr.SendHeartbeat( m_szHashID );			
		SendGetSegment();
		m_packetMgr.UpdateTimeOutStatus( UDP_REQUEST_TIMEOUT );
	}

	SendPeerNeedBlock();	// 定时发送对方请求的块,在 peer.m_mapNeedBlocks
}

void CChannel::SendPeerNeedBlock()
{
	list<P2P_COMMAND::CCmdGetSegmentRet *> listCmd;
	if ( m_peerMgr.GetNeedSendCmd( listCmd ) )
	{
		list<P2P_COMMAND::CCmdGetSegmentRet *>::iterator it = listCmd.begin();
		while ( it != listCmd.end() )
		{
			P2P_COMMAND::CCmdGetSegmentRet* pCmd = *it;
			if ( pCmd )
			{
				if ( !m_bSource )
				{
					CP2PPacket* pPacket = NULL;
					CKAutoLock l( m_packetMgr.GetCritSec() );
					if ( (pPacket = m_packetMgr.FindNoLock( pCmd->m_nBlockNum)) != NULL 
						&& PACKET_STATUS_ARRIVED == pPacket->m_packetStatus )
					{
						memcpy( pCmd->m_szBuffer, pPacket->m_pBuffer, sizeof(pCmd->m_szBuffer) );
						memcpy( pCmd->m_szChannelID, m_szHashID, sizeof(pCmd->m_szChannelID) );
						pCmd->m_nCheckSum = g_pNetwork->CalcCheckSum( pCmd->m_szBuffer, sizeof(pCmd->m_szBuffer) );
						g_pNetwork->PushUdpPeerCmd( pCmd );
						it = listCmd.erase( it );
						continue;
					}
					else
					{
						CKLog::WriteLog( LOG_TYPE_DEBUG, "no find this block or status is not right.");
					}
				}
				else
				{
					// if source, read from file.
					DWORD dwFilePos = pCmd->m_nBlockNum * UDP_PACKET_LENGTH;
					memset( pCmd->m_szBuffer, 0, sizeof(pCmd->m_szBuffer) );
					DWORD dwLength = sizeof(pCmd->m_szBuffer);
					if ( ReadSegment( dwFilePos, pCmd->m_szBuffer, dwLength) )
					{
						if ( dwLength == sizeof(pCmd->m_szBuffer))
						{
							memcpy( pCmd->m_szChannelID, m_szHashID, sizeof(pCmd->m_szChannelID) );
							pCmd->m_nCheckSum = g_pNetwork->CalcCheckSum( pCmd->m_szBuffer, sizeof(pCmd->m_szBuffer) );
							g_pNetwork->PushUdpPeerCmd( pCmd );

							static UINT nSendPackets = 0;
							if ( ++nSendPackets % 100 == 1)
							{
								CKLog::WriteLog( LOG_TYPE_DEBUG, "already send packet num %d.", nSendPackets);
							}
							it = listCmd.erase( it );
							continue;
						}
						else if ( dwFilePos + dwLength == m_llFileSize)	// 最后一块
						{
							memcpy( pCmd->m_szChannelID, m_szHashID, sizeof(pCmd->m_szChannelID) );
							pCmd->m_nCheckSum = g_pNetwork->CalcCheckSum( pCmd->m_szBuffer, sizeof(pCmd->m_szBuffer) );
							g_pNetwork->PushUdpPeerCmd( pCmd );

							CKLog::WriteLog( LOG_TYPE_FORCE, 
								"发送最后一块,块号：%d, 文件读取位置：%d,实际读取长度:%d,文件长度:%I64u",
								pCmd->m_nBlockNum, dwFilePos, dwLength, m_llFileSize);

							it = listCmd.erase( it );
							continue;
						}
					}
					else
					{
						CKLog::WriteLog( LOG_TYPE_DEBUG, 
							"no ReadSegment this block or status is not right.pos = %d, Len = %d, blocknum:%d", 
							dwFilePos, dwLength, pCmd->m_nBlockNum);
					}
				}
			}

			// record failed log
			delete pCmd;
			it = listCmd.erase( it );
		}
	}
}

void CChannel::SendGetSegment()
{
	// 取得自己想要的块 
	DWORD dwStart, dwEnd;
	if ( m_peerMgr.GetCount() > 0 )
	{
		if ( m_packetMgr.GetShouldBlock( dwStart, dwEnd ) )
		{
			map<DWORD, DWORD> mapReqeusts;
			m_peerMgr.SendGetSegment( m_szHashID, dwStart, dwEnd, mapReqeusts );

			if ( mapReqeusts.size() > 0)
				m_packetMgr.UpdatePacketStatus( mapReqeusts, PACKET_STATUS_REQUESTED);
		}
		/*
		1, 确定哪些Peer 拥有此块
		2, 优选一个Peer 发起请求,可连续请求多个块,如果均无此块,向源发起
		*/
	}
}

bool CChannel::IsSource()
{
	return m_bSource;
}

LONGLONG CChannel::Length()
{
	return m_llFileSize;
}

string CChannel::ChannelName()
{
	return m_sChannelName;
}
