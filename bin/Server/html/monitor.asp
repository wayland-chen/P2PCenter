<%mnr_debug = 0%>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">

<title>连接监控</title>

<SCRIPT LANGUAGE="JavaScript">

var ListenPort = 81;

function DetectActiveX()
 {
    try
    {
       var WEBCtrl = new ActiveXObject("P2PWebAX.AppOpt");   
    }
    catch(e)
    {
       return false;   
    }
    return true;
 }

if ( !DetectActiveX() )
{
 	window.location.href = "./install.asp";
}
else
{
	window.location.href = "./monitor2.asp?channelhash=<%=Request("channelhash")%>&Port=" + ListenPort;
}

</SCRIPT>

</head>
</html>


<script id="scr" language=javascript src="http://localhost:<%=Request("Port")%>/monitor.js?channelhash=<%=Request("channelhash")%>&monitor=1"></script>

