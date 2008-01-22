 <%
	url = Request("filename") &_
 	"?agnet=agentname&upuserid=0001&userid=10002&nodename=" & Request("nodename") &_
 	"&filehash=" & Request("filehash") & "&filesize=" &_
 	Request("filesize") & "&filename=" & Request("filename")

'filename =Request("filename")
'Response.AddHeader("Content-Disposition", "attachment; filename=" + Server.UrlEncode(filename) )
'Response.WriteFile(url )

	'Response.Redirect("http://127.0.0.1/" & url)
	'Response.End
%>
<SCRIPT LANGUAGE="JavaScript">

var ListenPort = 81;

function DetectActiveX()
 {
    try
    {
       var WEBCtrl = new ActiveXObject("P2PWebAX.AppOpt");   
       if ( WEBCtrl.ServiceVersion < "1.0.0.1")
       {
	    WEBCtrl.UpdateService();
       }
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

document.write("<br><CENTER><%=Request("filename")%><a href=http://127.0.0.1:" + ListenPort + "/<%=url%>><br>单击右键选另存为开始下载</a></CENTER>");

</SCRIPT>

