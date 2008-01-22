#if !defined(HttpSocketHandler_H)
#define HttpSocketHandler_H


#include "HttpServerInfo.h"
#include "HttpRequestProcess.h"
#include "SocketHandler.h"
#include "TransferStatus.h"

#include "log/KLog.h"
#include "../SocketClientMgr.h"

#include <atlbase.h>
#include <atlapp.h>
#include <atlmisc.h>

extern CSocketClientMgr g_socketClientMgr;


////////////////////////////////////////////////////////////////////////////////////
// HttpSocketHandler
// 
// Purpose:		handler for http socket. socket handler creates the socket and
//				the handler notifies us of socket events


class HttpSocketHandler : 
	public SocketHandler
{
	TransferStatus		_status;			// controls transfer
	HttpServerInfo &	_server;			// ref to server info
	HttpRequestProcess	_processReq;		// used to process requests
	bool				_keepAlive;			// shows if connection left open after
											// request processed
	DWORD				_channelId;

	bool				_cansend;

public:

	HttpSocketHandler ( HttpServerInfo & server ) : 
		
		SocketHandler(),

		_status(),
		_server( server ),
		_processReq(),
		_keepAlive(false),

		_channelId(0),
		_cansend(false)
	{
		CKLog::WriteLog( LOG_TYPE_DEBUG, "HttpSocketHandler create this = %d", this);
	}

	virtual ~HttpSocketHandler ()
	{
		SocketHandler::release();
		if ( g_socketClientMgr.m_pP2PNetwork && 0 != _channelId)
			g_socketClientMgr.m_pP2PNetwork->CloseSource( _channelId);
		_channelId = 0;
		CKLog::WriteLog( LOG_TYPE_DEBUG, "HttpSocketHandler release this = %d", this);
	}

	// socket event handlers
	bool onClose ( int error )
	{
		// close the socket
		Socket & sock = getSocket();
		sock.close();

		// remove handler
		delete this;

		return true;
	}

	bool onConnect ( int error )
	{
		//应当是客户端关闭
		getSocket().close();
		return true;
	}

	bool onSend_bak ( int error )
	{
		// get socket
		Socket & sock = getSocket();

		// get buffer to send
		LPTSTR buffer = _processReq.data();
		long   length = _processReq.size();

		// send response
		//long noSent = sock.send( buffer, length );
		//fwk
		long noSent = sock.send( buffer + _status.getNoSent(), min( _status.getNoToSend(), 8192 )  );

		// update transfer status
		_status.incNoSent( noSent );
		_status.decNoToSend( noSent );

		printf("noSent:%u, noToSent:%u, cursent: %u, size: %u\n", _status.getNoSent(), _status.getNoToSend(), noSent, _status.size() );

		// if done and dont keep open close socket
		// this will delete the handler 
		if ( _status.finished() && !_keepAlive )
		{
			printf("Sent finish. noSent:%u, noToSent:%u, size: %u\n", _status.getNoSent(), _status.getNoToSend(), _status.size() );
			sock.close();
		}

		return true;
	}

	bool onSend ( int error )
	{
		if ( !_cansend)
			return true;

		Socket & sock = getSocket();

		char szBuffer[1440];
		unsigned long ulLen = sizeof(szBuffer);
		static int nCount = 0;

		if ( _status.finished() && !_keepAlive) 
		{
			sock.close();
			return true;
		}

		DWORD dwPosition = _processReq.getResponse().getRangeStart() + _status.getNoSent();

		ulLen = min( sizeof(szBuffer), _status.getNoToSend());

		if ( g_socketClientMgr.m_pP2PNetwork->ReadSegment( _channelId, dwPosition, szBuffer, ulLen ) )
		{
			DWORD dwLen = sock.send( szBuffer,ulLen );
			if ( SOCKET_ERROR == dwLen ) 
			{
				sock.close();
				return false;
			}

			if ( dwLen > 1440)
			{
				sock.close();
				return false;
			}
			_status.incNoSent( dwLen );
			_status.decNoToSend( dwLen );

			if ( ++nCount > 1 )
			{
				CKLog::WriteLog( LOG_TYPE_DEBUG, 
					"Already Write Stream time 10, pos %u, len: %u, this: 0x%.8x", dwPosition, ulLen, this );
				g_socketClientMgr.m_pP2PNetwork->RequestSegment( _channelId, 
					dwPosition, REQUEST_MAX_SIZE );
				nCount = 0;
			}
		}
		else
			Sleep(100);

		return true;
	}

	bool onReceive ( int error )
	{
		_cansend = false;

		// get socket
		Socket & sock = getSocket();

		// reset transfer status
		_status.reset();

		// get no of bytes
		long length = sock.getRecieveSize();
		if ( length == 0 )
		{
			return false;
		}


		// recieve data
		char buffer[1024];

		// terminate buffer
		buffer[length] = '\0';

		// recieve 
		length = sock.recieve( buffer, 1024 );
		if ( length == 0 )
		{
			return false;
		}

		printf("%s",buffer);

		// process request
		if ( _processReq.process( buffer, length ) )
		{
			process_recv();
		}

		_cansend = true;

		return true;
	}

	void process_recv()
	{
		string strUrl = _processReq.getRequest().getUrl();
		string strFilehash = _processReq.getRequest().getParamentValue("filehash");
		if ( strFilehash.length() == 32 )
			process_media();
		else if ( _processReq.getRequest().getUrl() == "/monitor.js" )
			process_monitor();
		else
			getSocket().close();
	}

	void process_media()
	{
		_processReq.startResponse();
		_status.setSizeToSend( 
			_processReq.getResponse().getContentLength() - _processReq.getResponse().getRangeStart() );
		LPTSTR buffer = _processReq.getResponse().data();
		long   length = _processReq.getResponse().size();
		CKLog::WriteLog( LOG_TYPE_DEBUG, "send data len: %u, buf:\n%s\n", length, (char *)buffer );

		unsigned short usTcpPort, usUdpPort;
		unsigned long  ulUpuserid, ulUserid;
		__int64 i64Filesize;

		string sServeraddr, sAgentname, sNodename, sFileHash, sFilename, sFilepath;
		usUdpPort = (unsigned short)atoi(_processReq.getRequest().getParamentValue("udpport").c_str());
		if ( 0 == usUdpPort) usUdpPort = 3344;
		usTcpPort = (unsigned short)atoi(_processReq.getRequest().getParamentValue("tcpport").c_str());
		if ( 0 == usTcpPort) usTcpPort = 8902;

		ulUpuserid = (unsigned short)atoi(_processReq.getRequest().getParamentValue("upuserid").c_str());
		ulUserid = (unsigned short)atoi(_processReq.getRequest().getParamentValue("userid").c_str());

		i64Filesize = _atoi64(_processReq.getRequest().getParamentValue("filesize").c_str());

		sServeraddr = _processReq.getRequest().getParamentValue("serveraddr");
		sAgentname = _processReq.getRequest().getParamentValue("agent");
		sNodename = _processReq.getRequest().getParamentValue("nodename");
		sFileHash = _processReq.getRequest().getParamentValue("filehash");
		sFilename = _processReq.getRequest().getParamentValue("filename");
		sFilepath = _processReq.getRequest().getParamentValue("filepath");

        if ( "" == sServeraddr) sServeraddr = "p2p.kan8888.com";

		char szUrl[1024];
		_snprintf(szUrl, sizeof(szUrl), "kp2p://%s:%d%s?udpport=%d&agent=%s&upuserid=%u&userid=%u&nodename=%s&filehash=%s&filesize=%I64u&filename=%s&filepath=%s", 
			sServeraddr.c_str(), 
			usTcpPort,
			_processReq.getRequest().getUrl().c_str(),
			usUdpPort,
			sAgentname.c_str(),
			ulUpuserid,
			ulUserid,
			sNodename.c_str(),
			sFileHash.c_str(),
			i64Filesize,
			sFilename.c_str(),
			sFilepath.c_str()
			);

		_channelId = g_socketClientMgr.m_pP2PNetwork->OpenSource( szUrl, false );
		CKLog::WriteLog( LOG_TYPE_DEBUG, "OpenSource this: 0x%.8x ChannelID: %u", this, _channelId );
		if ( _channelId > 0 )
		{
			int nStartPos = _processReq.getResponse().getRangeStart();
			g_socketClientMgr.m_pP2PNetwork->RequestSegment( _channelId, nStartPos, REQUEST_MAX_SIZE );
		}

		CKLog::WriteLog( LOG_TYPE_DEBUG, "传输状态 %u, %u, %u", _status.getBlockSize(), _status.getNoSent(), _status.getNoToSend());

		int nSendLen = getSocket().send( buffer, length );
	}

	void process_monitor()
	{
		CString sOut = "";

		sOut += "var mnr_id = new Array();\n";
		sOut += "var mnr_externalip = new Array();\n";
		sOut += "var mnr_externaludpport = new Array();\n";
		sOut += "var mnr_externaltcpport = new Array();\n";
		sOut += "var mnr_issource = new Array();\n";
		sOut += "var mnr_conntype = new Array();\n";
		sOut += "var mnr_uploadbytes = new Array();\n";
		sOut += "var mnr_recvbytes = new Array();\n";
		sOut += "var mnr_requestblocks = new Array();\n";
		sOut += "var mnr_receiveblocks = new Array();\n";
		sOut += "var mnr_getblocks = new Array();\n";
		sOut += "var mnr_sendblocks = new Array();\n";
		sOut += "var mnr_healthrate = new Array();\n";
		sOut += "var mnr_returnrate = new Array();\n";
		sOut += "\n";

		stMonitorInfo monInfo;
		bool bGetInfo = false;

		if ( _processReq.getRequest().getParamentValue("channelhash").length() == 32 )
		{
			if ( g_socketClientMgr.m_pP2PNetwork->GetChannelMonitorInfo( 
				_processReq.getRequest().getParamentValue("channelhash").c_str(), monInfo))
				bGetInfo = true;
		}
		else	// 如果无channelhash取第一个频道
		{
			list<DWORD> listChannels;
			if ( g_socketClientMgr.m_pP2PNetwork->GetAllChannelID( listChannels) && listChannels.size() > 0 )
			{
				list<DWORD>::iterator it = listChannels.begin();
				while ( it != listChannels.end())
				{
					if ( g_socketClientMgr.m_pP2PNetwork->GetChannelMonitorInfo( *it, monInfo))
						bGetInfo = true;
					it ++;
					break; // 目前只取第一个频道
				}
			}
		}

		if ( bGetInfo)
		{
			int iIndex = 0;
			list<stPeerInfo>::iterator itPI = monInfo.Peers.begin();
			while ( itPI != monInfo.Peers.end() )
			{
				sOut += BuildPeerStr( iIndex, *itPI).c_str();
				sOut += "\n";
				itPI ++;
				iIndex ++;
			}
		}

		sOut += "var mnr_channelname = \"" + CString(monInfo.sChannelName.c_str()) + "\";\n";
		sOut += "\n";

		// 发送监控消息
		_processReq.getResponse().setContentLength( sOut.GetLength());
		_processReq.startResponse();
		LPTSTR buffer = _processReq.getResponse().data();
		long   length = _processReq.getResponse().size();
		CKLog::WriteLog( LOG_TYPE_DEBUG, "send montior data len: %u, buf:\n%s\n", length, (char *)buffer );
		int nSendLen = getSocket().send( buffer, length );
		nSendLen = getSocket().send( sOut.GetBuffer(0), sOut.GetLength() );
		//getSocket().close();
	}

	string BuildPeerStr( int iIndex, stPeerInfo& peerInfo)
	{
		string sRet;
		char szTmp[256];

		sprintf( szTmp, "mnr_id[%u]=%u;\n", iIndex, peerInfo.dwID);
		sRet += szTmp;
		sprintf( szTmp, "mnr_externalip[%u]=\"%s\";\n", iIndex, peerInfo.sExternalIP.c_str());
		sRet += szTmp;
		sprintf( szTmp, "mnr_externaludpport[%u]=%u;\n", iIndex, peerInfo.wExternalUDPPort);
		sRet += szTmp;
		sprintf( szTmp, "mnr_externaltcpport[%u]=%u;\n", iIndex, peerInfo.wExternalTCPPort);
		sRet += szTmp;
		sprintf( szTmp, "mnr_issource[%u]=%u;\n", iIndex, peerInfo.bSource);
		sRet += szTmp;
		sprintf( szTmp, "mnr_conntype[%u]=%u;\n", iIndex, peerInfo.ConnType);
		sRet += szTmp;
		sprintf( szTmp, "mnr_uploadbytes[%u]=%u;\n", iIndex, peerInfo.dwUploadBytes);
		sRet += szTmp;
		sprintf( szTmp, "mnr_recvbytes[%u]=%u;\n", iIndex, peerInfo.dwRecvBytes);
		sRet += szTmp;

		sprintf( szTmp, "mnr_requestblocks[%u]=%u;\n", iIndex, peerInfo.dwRequestBlocks);
		sRet += szTmp;
		sprintf( szTmp, "mnr_receiveblocks[%u]=%u;\n", iIndex, peerInfo.dwReceiveBlocks);
		sRet += szTmp;
		sprintf( szTmp, "mnr_getblocks[%u]=%u;\n", iIndex, peerInfo.dwGetBlocks);
		sRet += szTmp;
		sprintf( szTmp, "mnr_sendblocks[%u]=%u;\n", iIndex, peerInfo.dwSendBlocks);
		sRet += szTmp;
		sprintf( szTmp, "mnr_healthrate[%u]=%.2f;\n", iIndex, peerInfo.dbHealthRate);
		sRet += szTmp;
		sprintf( szTmp, "mnr_returnrate[%u]=%.2f;\n", iIndex, peerInfo.dbReturnRate);
		sRet += szTmp;

		return sRet;
	}

	// run method
	//long run ();
};


#endif