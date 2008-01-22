<object ID=ObjPlayer width="100%" height="100%"
classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000"
codebase="http://download.macromedia.com
/pub/shockwave/cabs/flash/swflash.cab#4,0,0,0">
<param name="SRC" value="<%=ObjUrl%>">
<embed src="<%=ObjUrl%>" width="100%" height="100%"></embed>
</object> 


<script language="JavaScript"> 

function ObjPlay(str)
{
	ObjPlayer.Play();
}

function ObjPause()
{
	ObjPlayer.Pause();
}

function ObjStop()
{
	ObjPlayer.PlayStop();
}

function ObjFullScreen()
{
	ObjPlayer.SetZoomRect(0,0,800,600)
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
