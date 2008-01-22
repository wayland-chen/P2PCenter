
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<title>P2P</title>

<script language="JavaScript">
document.oncontextmenu=new Function("event.returnValue=false;"); 
document.onselectstart=new Function("event.returnValue=false;"); 
function showsubmenu(){
	whichEl = eval("vod_list");
	if (whichEl.style.display == "none") {
		eval("vod_list.style.display=\"\";");
		//parent.mainframe.rows = "30,*,80";		
		idFlag.innerHTML = "<img src='images/turn_right.gif' title='按此切换到正常模式'>";
	}
	else {
		eval("vod_list.style.display=\"none\";");
		//parent.mainframe.rows = "0,*,0";
		idFlag.innerHTML = "<img src='images/turn_left.gif' title='按此切换到精简模式'>";
	}
}
</script>

</head>

<body leftmargin="0" topmargin="0" marginwidth="0" marginheight="0">
<table  width="100%" height="100%" border="0" align="center" cellpadding="0" cellspacing="0">
  <tr> 
    <td><iframe id="vodplayer" name="frm_main" frameborder="0" src="main.asp"  width="100%" height="100%" SCROLLING="NO" border="0" ></iframe></td>
    <td width="8" background="images/turn_bg.gif" bgcolor="#191D23" style="cursor:hand" onclick="showsubmenu()"><span id="idFlag" ><img src="images/turn_right.gif" alt="按此切换到精简模式" width="8" height="7" id="jiantou"></span></td>
    <td width="232" id="vod_list"><iframe id="frm_left" frameborder="0" src="left.asp"  width="100%" height="100%" SCROLLING="yes" border="0" ></iframe></td>
  </tr>
</table>
<div style="position:absolute; top:-100px; left:-100px">
</div>
</body>

</html>
