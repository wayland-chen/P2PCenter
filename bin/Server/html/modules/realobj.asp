
 <object id="ObjPlayer" classid="clsid:CFCDAA03-8BE4-11cf-B84B-0020AFBBCCFA" width=100% height=100%>
 <param name="_ExtentX" value="11298">
 <param name="_ExtentY" value="7938">
 <param name="AUTOSTART" value="-1">
 <param name="SHUFFLE" value="0">
 <param name="PREFETCH" value="0">
 <param name="NOLABELS" value="-1">
 <param name="SRC" value="about:blank">
 <param name="CONTROLS" value="Imagewindow">
 <param name="CONSOLE" value="clip1">
 <param name="LOOP" value="0">
 <param name="NUMLOOP" value="0">
 <param name="CENTER" value="0">
 <param name="MAINTAINASPECT" value="0">
 <param name="BACKGROUNDCOLOR" value="#000000">
 </object>
<BR>
 <object id="ObjPlayer2" classid="clsid:CFCDAA03-8BE4-11cf-B84B-0020AFBBCCFA" width=100% height=27>
 <param name="_ExtentX" value="11298">
 <param name="_ExtentY" value="794">
 <param name="AUTOSTART" value="-1">
 <param name="SHUFFLE" value="0">
 <param name="PREFETCH" value="0">
 <param name="NOLABELS" value="-1">
 <param name="SRC" value="about:blank">
 <param name="CONTROLS" value="ControlPanel">
 <param name="CONSOLE" value="clip1">
 <param name="LOOP" value="0">
 <param name="NUMLOOP" value="0">
 <param name="CENTER" value="0">
 <param name="MAINTAINASPECT" value="0">
 <param name="BACKGROUNDCOLOR" value="#000000">
 </object>

<script language="JavaScript"> 

function ObjPlay(str)
{
	ObjPlayer.SetSource(str);
	ObjPlayer.DoPlay();
}

function ObjPause()
{
	ObjPlayer.DoPause();
}

function ObjStop()
{
	ObjPlayer.DoStop();
}

function ObjFullScreen()
{
	if(!ObjPlayer.CanStop())
	{
		alert("影片未开始播放无法切换为全屏模式");
	}
	else
	{
		alert("点击确定按钮后进入全屏播放模式,在全屏播放模式中按 Esc 键退出全屏模式");
		ObjPlayer.SetFullScreen();
	}
}

function ObjSetHeight(h)
{
	ObjPlayer.height = h;
}

function ObjSetWidth(w)
{
	ObjPlayer.width = w;
}

</script>
