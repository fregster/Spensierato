// JavaScript Document
<!-- AJAX Code for search -->
var xmlHttp = false;

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

function ajaxReadyStateChange()
{
	if(http.readyState==4)
	{
	        alert(http.responseText);
	}
}

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