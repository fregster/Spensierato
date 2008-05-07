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
		catch (ee) {http = false;}
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
	var xmldoc;
	if(window.ActiveXObject)
	{
		xmldoc = new ActiveXObject("Microsoft.XMLDOM");
		xmldoc.async="false";
		xmldoc.loadXML(http.responseText);
	}
	else
	{
		xmldoc = http.responseXML;
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
		};
}


//var ToolTips = new Tips($$('.ToolTip'), {
//	className: 'ToolTip'
//});
function pageReload(returnVal) 
{
	//returnRefresh(returnVal);
	setTimeout("window.top.location.reload(true)",100);
}

var notes;
var init;
function initNotification()
{
	if(init !== true)
	{
	init = true;
	notes = new Fx.Style('notifications', 'opacity', {duration:500}).set(0); //will make it immediately transparent
	document.getElementById('notifications').style.display = 'visible'; //Removes the CSS display none, stops flickering
	}
}

function notification(text)
{
	var timer;
	document.getElementById('notifications').innerHTML = text;
	
	notes.start(-0, 1);
	
	//After 3 seconds fade back out
	timer=setTimeout(notes.start(1, 0),3000);
	
	return false;
}

window.onload = function()
{ 
	initNotification();
};