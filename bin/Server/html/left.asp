<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<title>左菜单</title>
<link href="style.css" rel="stylesheet" type="text/css">
<base target="frm_main">
<style type="text/css">
body,td {
	font-family: "Verdana,Arial";
	font-size: 12px;
	color: #cccccc;
	margin: 0;
	padding: 0;
	SCROLLBAR-ARROW-COLOR: #2a7f01; SCROLLBAR-DARKSHADOW-COLOR: #def3f3; SCROLLBAR-BASE-COLOR: #efefef
}
div {
	border:0px solid #F00;
}
ul {
	margin: 0;
	padding: 5;
}
a {
	color: #cccccc;
	text-decoration: none;
	line-height:150%;
	padding: 1px;
}
a:hover {
	color:#ff6600;
	text-decoration: underline;
}
#main li {
    float: left;
	list-style:none;
	clear:both;
	width:95%;
	border-bottom:1px dotted #444444;
}
.list0 {
	text-align: center;
}
.list1 {
	width:85%;
	float: left;
}
.list2 {
	color:#666666;
	width:15%;
	font-size: 9px;
	line-height:200%;
	float: left;
}
</style>

<SCRIPT LANGUAGE="JavaScript">

var ListenPort = 81;

function DetectActiveX()
 {
    try
    {
       var WEBCtrl = new ActiveXObject("P2PWebAX.AppOpt");   
       //WEBCtrl.StartApp();
       //ListenPort = WEBCtrl.ListenPort;
       //alert(WEBCtrl.ServiceVersion);
       //WEBCtrl.UpdateService();
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

<SCRIPT LANGUAGE="JavaScript">

function fullScreen()
{
    try
    {
		parent.frm_main.ObjFullScreen();
    }
    catch(e)
    { 
    }

	return false;
}

</SCRIPT>

</head>

<body bgcolor="#000000">
<div align=right>
&nbsp;&nbsp;
<!--input type=button value='显示监控' 
onClick="javascript:window.open('monitor.asp?channelhash=', 
'win_mtr', 'width=200, height=600, left=0, top=0');"!-->
<a href=# onclick="javascript:window.open('monitor.asp?channelhash=', 
'win_mtr', 'width=200, height=600, left=0, top=0'); return false;">显示监控</a>&nbsp;&nbsp;
<a href=# onclick="return fullScreen();">全屏</a>&nbsp;&nbsp;
</div>
<div align="right">
  <table width="100%" border="0">
    <tr> 
      <td>
<!--#include file="Kevin_Test_Node_Name.htm" -->
      </td>
    </tr>
  </table>
</div>

</body>
</html>


