// JavaScript Document
<!-- This code might not have all the copyright in as it can be removed as part of some compression techniques -->
<!-- Please visit http://www.fryer.org.uk/blog/?page_id=36 for details -->

function ajaxObject()
{
	var http = false;
	//Use IE's ActiveX items to load the file.
	if(typeof ActiveXObject != 'undefined') 
	{
		try {http = new ActiveXObject("Msxml2.XMLHTTP");}
		catch (e) 
		{
			try {http = new ActiveXObject("Microsoft.XMLHTTP");}
			catch (E) {http = false;}
		}
	//If ActiveX is not available, use the XMLHttpRequest of Firefox/Mozilla etc. to load the document.
	} else if (XMLHttpRequest) {
		try {http = new XMLHttpRequest();}
		catch (e) {http = false;}
	}
	
	return http;
}

var http = ajaxObject();

function ajaxGet(url, parameters)
{
	http.open('GET', url + parameters, true);
	http.send(null);
}

function ajaxPost(url, parameters)
{
		http.open('POST', url, true);
		//Send the proper header information along with the request
		http.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
		http.setRequestHeader("Content-length", parameters.length);
		http.setRequestHeader("Connection", "close");
		http.send(parameters);
}

function ajaxGetXML()
{
	//IE does not return responseXML
	if(window.ActiveXObject)
	{
		var xmldoc = new ActiveXObject("Microsoft.XMLDOM");
		xmldoc.async="false";
		xmldoc.loadXML(http.responseText);
	}
	else
	{
		var xmldoc = http.responseXML;
	}
	
	return xmldoc;
}

function searchFunction()
{
        ajaxGet('http://www.fryer.org.uk/test/nuneaton/tools/search?s=', document.forms["searchform"].elements["searchtext"].value );
        document.forms["searchform"].elements["searchtext"].value = '';
        
        http.onreadystatechange=function()
    	{
        	if(http.readyState==4)
			{
			        document.getElementById('page_section_main').innerHTML = http.responseText;
			}
		}
}


//var ToolTips = new Tips($$('.ToolTip'), {
//	className: 'ToolTip'
//});

var notes;

window.onload = function()
{ 
	notes = new Fx.Style('notifications', 'margin-top', {duration:500});
	notes.hide;
	
	//new fx.Opacity('notifications', {duration: 800 });
	//notes.setOpacity(0);
//	document.getElementById('notifications').style.display = 'none';
}

function pageReload(returnVal) 
{
	//returnRefresh(returnVal);
	setTimeout("window.top.location.reload(true)",100);
}

function notification(text)
{
	var timer;
	document.getElementById('notifications').innerHTML = text;
//	document.getElementById('notifications').style.display = '';
	
	notes.start(-100, 100);
	
	//After 2 seconds fade back out
	timer=setTimeout("notes.start(100, -100);",2000);
	
	return false;
	
}