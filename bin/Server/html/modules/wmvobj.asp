<object ID="ObjPlayer" WIDTH="100%" HEIGHT="100%" classid="CLSID:6BF52A52-394A-11d3-B153-00C04F79FAA6" VIEWASTEXT>
<param NAME="FileName" VALUE>
<param NAME="AutoStart" VALUE="1">
<PARAM NAME="AutoRewind" VALUE="-1">
<param NAME="AnimationAtStart" VALUE="false">
<param NAME="ShowControls" VALUE="true">
<param NAME="ClickToPlay" VALUE="true">
<param NAME="EnableContextMenu" VALUE="true">
<param NAME="EnablePositionControls" Value="true">
<param name="Balance" value="1">
<param Name="ShowStatusBar" Value="false">
<param NAME="AutoSize" VALUE="1">
<param NAME="url" VALUE="">
<param name="AllowScan" value="1">
<param name="AllowChangeDisplaySize" value="1">
<param name="EnableTracker" value="1">
<param name="EnablePositionControls" value="1">
<param name="stretchToFit" value="1">
</object>

<script language="JavaScript"> 

function ObjPlay(str)
{
	ObjPlayer.URL = str;
	ObjPlayer.controls.play();
}

function ObjPause()
{
	ObjPlayer.controls.pause();
}

function ObjStop()
{
	ObjPlayer.controls.stop();
}

function ObjFullScreen()
{
	alert("点击确定按钮后进入全屏播放模式,在全屏播放模式中按 Esc 键退出全屏模式");
	ObjPlayer.fullScreen = 1;
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
