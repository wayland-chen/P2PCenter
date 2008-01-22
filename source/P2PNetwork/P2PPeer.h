#pragma once

typedef enum CONNNECT_STATUS
{
	CONNNECT_STATUS_NONE = 0,
	CONNNECT_STATUS_HANDSHAKING,
	CONNNECT_STATUS_CONNECTED,
	CONNNECT_STATUS_DISCONNECTED
};

struct ST_TRANS_STAT {
	DWORD dwTime;
	DWORD dwBytes;
	ST_TRANS_STAT()
	{
		dwTime = GetTickCount();
		dwBytes = UDP_PACKET_LENGTH;
	}
};

// 速度统计间隔，单位，秒
#define SPEED_STAT_INTERVAL 10

class CP2PPeer
{
public:
	CP2PPeer(void);
	virtual ~CP2PPeer(void);

	virtual bool IsHaveBlock( int nBlockNum );

	void CalcRequestBlock( DWORD dwBlock );	// 已方向对方请求块
	void CalcRecvBlock( DWORD dwBlock );	// 请求的块被已方接收到
	double HealthRate();

	void CalcGetBlock( DWORD dwBlock );		// 对方向已方请求块
	void CalcSendBlock( DWORD dwBlock );	// 已方向对方发送块
	double ReturnRate();

	PEERINFO m_peerInfo;
	map<DWORD, DWORD> m_mapBlockAreas;
	CONNNECT_STATUS m_connStatus;
	map<DWORD, DWORD> m_mapNeedBlocks;  // 这里的块号均是要发送给此PEER的，second 保留
	DWORD    m_dwUploadBytes;			// 上传能力,十秒内的平均上传字节 单位 BYTE/秒
	DWORD    m_dwRecvBytes;				// 本地统计上传能力,十秒内的平均接收字节 单位 BYTE/秒,对此PEER的一个评级
	DWORD    m_dwLastHeartbeatTime;		// 最后一次发送心跳时间

	DWORD    m_dwRequestBlocks;			// 请求的块数
	DWORD    m_dwReceiveBlocks;			// 收到的块数
	DWORD    m_dwGetBlocks;				// 被请求的块数
	DWORD    m_dwSendBlocks;			// 发送的块数
	list<ST_TRANS_STAT> m_listUploads;	// 上传的字节列表 first = time
	list<ST_TRANS_STAT> m_listReceives;	// 接收到的字节列表
};
