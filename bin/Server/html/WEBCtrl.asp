<HTML>
<HEAD>
<META HTTP-EQUIV="Content-Type" CONTENT="text/html; charset=gb2312">
<TITLE>播放页</TITLE>

<style type="text/css">
body,td,span {
 font-family: "Verdana,Arial";
 font-size: 12px;
 color: #cccccc;
 margin: 0;
 padding: 0;
 SCROLLBAR-ARROW-COLOR: #2a7f01; SCROLLBAR-DARKSHADOW-COLOR: #def3f3; SCROLLBAR-BASE-COLOR: #efefef
}
</style>

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

</SCRIPT>


<SCRIPT language=javascript> 
var pageUrl=""; 
pageUrl = window.location; 
pageUrl = pageUrl.toString();
var Parameter = pageUrl.split("?"); 

function request(paras){ //获取 url 的参数值，不区分大小写,如无此参数，返回空字符串.
var url = location.href;
var paraString = url.substring(url.indexOf("?")+1,url.length).split("&");
var paraObj = {}
for (i=0; j=paraString[i]; i++){
paraObj[j.substring(0,j.indexOf("=")).toLowerCase()] = j.substring(j.indexOf("=")+1,j.length);
}
var returnValue = paraObj[paras.toLowerCase()];

if(typeof(returnValue)=="undefined"){
return "";
}else{
return returnValue;
}
}

var FileName = request("filename");
if( "" == FileName )
{
	FileName = "UnKnow.rmvb";
}

</SCRIPT>
</HEAD>
<BODY bgcolor="#000000" leftMargin=0 topMargin=0>
 
<%
	Dim FileName, FileExt, Pos, BtmHeight, ObjUrl
	BtmHeight = 0
	FileName = Request("filename")
	ObjUrl = "http://127.0.0.1:81/" & FileName & "?" & Request.ServerVariables("QUERY_STRING")
	'Response.Write ObjUrl
	Pos = InStrRev(FileName, ".")
	If Pos > 0 Then
		FileExt = Mid(FileName, Pos + 1, LEN(FileName))
		FileExt = UCase(FileExt)
		'Response.Write "<BR> " & Pos & " - " & FileExt & "<BR>"
	End If
%>

<center>

<%
Select Case FileExt
	Case "RMVB", "RM", "RV", "WMA", "MP3"
	BtmHeight = 47%>
<!--#INCLUDE FILE="modules/realobj.asp" --> 
<%
	Case "WMV"
	BtmHeight = 20%>
<!--#INCLUDE FILE="modules/wmvobj.asp" --> 
<%
	Case "FLVV"
	BtmHeight = 20%>
<!--#INCLUDE FILE="modules/flvobj.asp" --> 
<%
	Case "FLV"
	BtmHeight = 20%>
<!--#INCLUDE FILE="modules/iframeobj.asp" --> 
<%
	Case "SWF"
	BtmHeight = 20%>
<!--#INCLUDE FILE="modules/flashobj.asp" --> 
<%
	Case Else
	Response.Write("不支持此文件格式")
	Response.End
End Select
%>

</center>

<div align=right>
&nbsp;<span id="text" align=left></span>&nbsp;&nbsp;
<input type=button value='显示监控' 
onClick="javascript:window.open('monitor.asp?channelhash=<%=Request("filehash")%>', 
'win_mtr', 'width=200, height=600, left=0, top=0');">
<input type=button onclick="ObjFullScreen();" value="全屏">
</div>

<%BtmHeight = BtmHeight - 20%>

<script language="JavaScript"> 
var obj_h = document.body.clientHeight - <%=BtmHeight%>;
function onResizeCheck(myEvents)
{
	obj_h = document.body.clientHeight - <%=BtmHeight%>;
	ObjSetHeight(obj_h);
}
window.onresize= onResizeCheck;

ObjSetHeight(obj_h);
var str = "http://127.0.0.1:" + ListenPort + "/" + FileName + "?" + Parameter[1];
<%If FileExt = "FLV" Then%>
//ObjPlay("http://www.kan8888.com/p2p/html/files/flvplayer2.swf?showfsbutton=true&autostart=true&file=" + escape(str));
ObjPlay("http://www.kan8888.com/p2p/html/files/vcastr2.swf?vcastr_file=" + escape(str) + "&vcastr_config=1|0|100|0|3|0x666666|70|0xffcc00|0xffffff|0xffffff|Kan8888.com");
<%Else%>
ObjPlay(str);
<%End If%>
</script>


</BODY>
</HTML>

