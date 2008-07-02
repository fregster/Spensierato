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
        ajaxGet(document_root+'/tools/search?s=', document.forms.searchform.elements.searchtext.value );
        document.forms.searchform.elements.searchtext.value = '';
        
        http.onreadystatechange=function()
    	{
        	if(http.readyState==4)
			{
			        document.getElementById('page_section_main').innerHTML = http.responseText;
			}
		};
}


var ToolTips = new Tips($$('.ToolTip'), {
	className: 'ToolTip'
});

var SpriteTips = new Tips($$('.sprite'), {
	className: 'ToolTip'
});

function pageReload(returnVal) 
{
	setTimeout('window.top.location.reload(true)',100);
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
	
	notes.start(-0, 0.8);
	
	//After 3 seconds fade back out
	timer=setTimeout('notes.start(0.8, 0)',3000);
	
	return false;
}

//Init scripts to auto load stuff
window.onload = function()
{ 
	new Fx.Scroll({duration: 1200}); //Load the smooth scroller
	initNotification(); //Load the notification bar
	stepFontSize(readCookie('fontSize')-3);
};

function increaseFontSize() {
   stepFontSize(1);
}
function decreaseFontSize() {
	stepFontSize(-1);  
}

function stepFontSize(increment) {
	var p = document.getElementsByTagName('p');
	
	var fontSizes = new Array("xx-small", "x-small", "small", "medium", "large", "x-large", "xx-large");
	
   for(i=0;i<p.length;i++) {
   	var s = 3;
      if(p[i].style.fontSize) {
         
         for(f=0;f<fontSizes.length;f++) {
         	if(p[i].style.fontSize == fontSizes[f]) { var s = f; }
         }
      }
      
      s = s + increment;
      
      if(s > 0 && s < fontSizes.length) {
          p[i].style.fontSize = fontSizes[s];
          
         //Save the font size in a cookie
          createCookie('fontSize',s,'365');
      }
   }
}

function createCookie(name,value,days) {
	if (days) {
		var date = new Date();
		date.setTime(date.getTime()+(days*24*60*60*1000));
		var expires = "; expires="+date.toGMTString();
	}
	else var expires = "";
	document.cookie = name+"="+value+expires+"; path=/";
}

function readCookie(name) {
	var nameEQ = name + "=";
	var ca = document.cookie.split(';');
	for(var i=0;i < ca.length;i++) {
		var c = ca[i];
		while (c.charAt(0)==' ') c = c.substring(1,c.length);
		if (c.indexOf(nameEQ) == 0) return c.substring(nameEQ.length,c.length);
	}
	return null;
}

function eraseCookie(name) {
	createCookie(name,"",-1);
}