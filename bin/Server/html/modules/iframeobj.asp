<iframe name=ObjPlayer width=100% height=%100 frameborder=0 
scrolling=no src="about:blank"></iframe>

<script language="JavaScript"> 

function ObjPlay(str)
{
	ObjPlayer.location = str;
}

function ObjPause()
{
	ObjPlayer.location = "about:blank";
}

function ObjStop()
{
	ObjPlayer.location = "about:blank";
}

function ObjFullScreen()
{
}

function ObjSetHeight(h)
{
	document.all.ObjPlayer.height = h;  
}

function ObjSetWidth(w)
{
	ObjPlayer.width = w;
}

</script>
