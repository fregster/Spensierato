//Javascript Document

function ajaxLoad(id) {
	var ed = tinyMCE.get('content');

	ed.setProgressState(1); // Show progress

	ajaxPost('http://www.fryer.org.uk/current/cms/ajax/'+id, ed.getContent());
	
	http.onreadystatechange=function()
    	{
        	if(http.readyState==4)
			{
				alert(http.responseText);
			}
		}

	ed.setProgressState(0); // Hide progress
}

function ajaxSave(id) {
	var ed = tinyMCE.get('content');

	ed.setProgressState(1); // Show progress
	
	ajaxPost('http://www.fryer.org.uk/current/cms/ajax/'+id, ed.getContent());
	
	http.onreadystatechange=function()
    	{
        	if(http.readyState==4)
			{
				alert(http.responseText);
			}
		}

	ed.setProgressState(0); // Hide progress
	
	ajaxLoad();
}

var t
var c

function delayedSave(id)
{
	clearTimeout(t);
	t=setTimeout("ajaxSave(id)",1000);
}