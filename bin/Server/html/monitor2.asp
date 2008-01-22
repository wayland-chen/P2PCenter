<%mnr_debug = 0%>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">

<title>连接监控</title>

<style type="text/css">
body,td {
 font-family: "Verdana,Arial";
 font-size: 12px;
 color: #cccccc;
 margin: 0;
 padding: 0;
 SCROLLBAR-ARROW-COLOR: #2a7f01; SCROLLBAR-DARKSHADOW-COLOR: #def3f3; SCROLLBAR-BASE-COLOR: #efefef
}
</style>

</head>

<body bgcolor="#000000">
<div align="right">
  <table width="100%" border="0">
    <tr> 
      <td>
<span id="text"></span>
      </td>
    </tr>
  </table>
</div>


<script id="scr" language=javascript src="http://localhost:<%=Request("Port")%>/monitor.js?channelhash=<%=Request("channelhash")%>&monitor=1"></script>

<script language="javascript">
  var timeid = null;
  var use_time = 1;
  function loadscr() {
       clearTimeout(timeid);
 scr.src = "http://localhost:<%=Request("Port")%>/monitor.js?channelhash=<%=Request("channelhash")%>&monitor=" + timeid;
 text.innerHTML = "节目：" + mnr_channelname + "<br>";
 text.innerHTML += "在线：" + mnr_id.length + " 人<br>";
 for ( var ic = 0; ic < mnr_id.length; ic ++)
 {
  text.innerHTML += ("-------------------------------<br>");
  text.innerHTML += ("用户ID:" + mnr_id[ic] + "<br>");
  text.innerHTML += ("外网地址:" + mnr_externalip[ic] + ":" + mnr_externaludpport[ic] + "<br>");
  var speed = mnr_recvbytes[ic] / 10000;
  text.innerHTML += ("下载速度:" + speed.toFixed(2) + " KB/S<br>");

  var downkbs = (mnr_receiveblocks[ic] * 933 / 1024);
  if ( downkbs > 1024)
  {
    downkbs /= 1024;
    text.innerHTML += ("下载总数:" + downkbs.toFixed(2) + " MB<br>");
  }
  else
  {
    text.innerHTML += ("下载总数:" + downkbs.toFixed(2) + " KB<br>");
  }

  var speed = mnr_uploadbytes[ic] / 10000;
  text.innerHTML += ("上传速度:" + speed.toFixed(2) + " KB/S<br>");

<%If mnr_debug = 1 Then%>

  var upkbs = (mnr_sendblocks[ic] * 933 / 1024);
  if ( upkbs > 1024)
  {
    upkbs /= 1024;
    text.innerHTML += ("上传总数:" + upkbs.toFixed(2) + " MB<br>");
  }
  else
  {
    text.innerHTML += ("上传总数:" + upkbs.toFixed(2) + " KB<br>");
  }

  text.innerHTML += ("外网TCP端口:" + mnr_externaltcpport[ic] + "<br>");
  text.innerHTML += ("请求块数:" + mnr_requestblocks[ic] + "<br>");
  text.innerHTML += ("收到块数:" + mnr_receiveblocks[ic] + "<br>");
  text.innerHTML += ("被请求块数:" + mnr_getblocks[ic] + "<br>");
  text.innerHTML += ("回送块数:" + mnr_sendblocks[ic] + "<br>");
  text.innerHTML += ("接收健康率:" + mnr_healthrate[ic] + "%<br>");
  text.innerHTML += ("回送健康率:" + mnr_returnrate[ic] + "%<br>");
  text.innerHTML += ("是否为源:" + mnr_issource[ic] + "<br>");
<%End If%>
 }
 use_time ++;
 timeid = setTimeout("loadscr();", 1000);
}
loadscr();
window.focus();
</script>

</body>
</html>


