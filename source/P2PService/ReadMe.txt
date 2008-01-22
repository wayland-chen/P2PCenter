IE to P2PService 的URL规定:
	获取文件接口:
		格式: http://host:port/filename.rmvb?serveraddr=serverhost&tcpport=tcpport&updoort=3344&agent=agentname&upuserid=1&userid=111&nodename=nodename(char[32])&filehash=char[32]&filesize=DWORD[4]&filepath=D:\xxx
		示例:http://127.0.0.1:81/test.rmvb?serveraddr=p2p.kan8888.com&tcpport=8902&udpport=3344&agent=shengcong&upuserid=1&userid=999&nodename=KevinTestNode&filehash=ABCD3458934759834753479857349444&filesize=333654321&filename=FileName.rmvb&filepath=C:\\ttt\test.rmvb
		必选参数:nodename 节点HASH,即目录,一个代理可以有多个目录,也可以称作组别 group
				filehash 对应目录下文件的MD5标准HASH码,32字节,文件以此作为区分标准
				filesize 文件实际长度,单位,字节
				filepath 文件路径,仅对于源有效
		可选参数:serveraddr(default:p2p.kan8888.com), tcpport(default:8902), udpport(default:3344), agent, upuserid, userid
		当前版本 serveraddr tcpport udpport还处于保留状态.P2PNetwork连接的服务器配置由 P2PNetwork.ini提供
		
	获取监控数据接口:
		格式:http://host:port/monitor.js?channelhash=char[32]
		示例:http://host:port/monitor.js?channelhash=ABCD3458934759834753479857349444
		可选参数: channelhash 长度必须为32字节文件标准MD5编码HASH码,为空取得所有频道监控
	
App to P2PNetwork 的URL规定:
	格式: kp2p://tcpserver:tcpport/....
	参数与 IE to P2PService 的URL规定类同,少tcpserver与tcpport
	注:UDP打洞 UDPSERVER:UPDPORT由连接tcpserver:tcpport后,登录返回
