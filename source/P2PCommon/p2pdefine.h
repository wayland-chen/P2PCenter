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

#pragma once

//客户端与客户端之间的信令
typedef enum P2P_CMD_TYPE
{
	P2P_CMD_HANDSHAKE = 0x10001000,		// 握手
	P2P_CMD_HANDSHAKE_RET,		
	P2P_CMD_GETBLOCK_AREA,				// 取得拥有块
	P2P_CMD_GETBLOCK_AREA_RET,			// 取得拥有块返回
	P2P_CMD_GETSEGMENT,					// 取得文件片段
	P2P_CMD_GETSEGMENT_RET,				// 取得文件片段返回
	P2P_CMD_STAT_UPLOAD,				// 查询对方上传能力
	P2P_CMD_STAT_UPLOAD_RET,			// 查询对方上传能力返回
	P2P_CMD_HEARTBEAT,					// 心跳,维持链路
	P2P_CMD_CANCELSEGMENT,				// 取消请求的块
	P2P_CMD_CANCELSEGMENT_RET,			// 取消请求的块返回
	P2P_CMD_EXIT						// 单方退出
};

typedef __int64 QWORD;

#define UDP_PACKET_LENGTH 933
#define UDP_BUFFER_LENGTH 1024
// 请求超时时间 单位(毫秒)
#define UDP_REQUEST_TIMEOUT 3000
#define P2P_QUERYBLOCK_INTERVAL 5000	  // 查询存在块间隔
#define P2P_MAX_GETSEGMENT_INTERVAL 10000 // 单次请求最大块区间 10M内
#define P2P_HEARTBEAT_INTERVAL      15000 // P2P心跳间隔
#define P2P_TIMEREQUEST_MAX_NUM		64	  // 单次向单PEER请求最大块数量
#define P2P_TIMESEND_MAX_NUM		64	  // 单次向单PEER发送最大块数量

