#pragma once

#include "P2PPacket.h"

// 块索引从0开始
class CP2PPacketMgr :
	public CKPtrMap<DWORD, CP2PPacket>
{
public:
	CP2PPacketMgr(void);
	virtual ~CP2PPacketMgr(void);
	
	// 读取文件片段
	bool ReadSegment( DWORD dwStartPos, char* pBuffer, DWORD& dwLength );
	// 删除闭区间 [dwStartBlock, dwEndBlock] 以外的块
	void RemoveBlock( DWORD dwStartBlock, DWORD dwEndBlock );
	// 收到新包
	bool PacketArrived( DWORD dwBlockIndex, char* pBuffer );
	// 请求一些块
	void InsertShouldBlock( DWORD dwStartBlock, DWORD dwBlockNum );
	// 删除所有未发出请求的块号
	void ClearShouldBlock();
	// 更新超时块状态,以便再次请求
	// 定时调用,时间不要超过1秒,建议定书100us
	void UpdateTimeOutStatus( DWORD dwTimeUsec = UDP_REQUEST_TIMEOUT );
	// 调用此函式后立即发起请求.在此函数里更新了时间戳,如为 false,则没有可以请求的块
	bool GetShouldBlock( DWORD& dwStart, DWORD& dwEnd );
	// 更新MAP里块状态
	void UpdatePacketStatus( const map<DWORD, DWORD>& mapRequests, PACKET_STATUS status);

	// 取得已存在的块,完整的块
	bool GetExistBlockAreas( map<DWORD, DWORD>& mapAreas );
	bool GetExistBlockList( list<DWORD>& listBlock, DWORD dwStartBlock, DWORD dwEndBlock );
};
