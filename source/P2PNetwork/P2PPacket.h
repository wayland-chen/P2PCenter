#pragma once

typedef enum PACKET_STATUS
{
	PACKET_STATUS_NONE = 0,
	PACKET_STATUS_REQUESTED,
	PACKET_STATUS_ARRIVED
};

class CP2PPacket
{
public:
	CP2PPacket(void);
	virtual ~CP2PPacket(void);

	char* m_pBuffer;
	// 最近发送请求的时间戳
	DWORD m_dwLastRequest;
	// 是不有效,即是否已取得了数据
	PACKET_STATUS  m_packetStatus;
};
