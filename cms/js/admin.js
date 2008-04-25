//Javascript Document

function ajaxLoad(id) {
	var ed = tinyMCE.get('content');
	
	ed.setProgressState(1); // Show progress

	ajaxGet('http://www.fryer.org.uk/current/cms/ajax/edit/', id);
	
	http.onreadystatechange=function()
    	{
        	if(http.readyState==4)
			{
				var response = http.responseText;
				var node=new Array('element','data');
				getNodeFromXML(response, node);
				
				ed.setContent(response);
			}
		}

	ed.setProgressState(0); // Hide progress
}

function ajaxSave(id) {
	var ed = tinyMCE.get('content');

	ed.setProgressState(1); // Show progress
	
	ajaxPost('http://www.fryer.org.uk/current/cms/ajax/edit/'+id, ed.getContent());
	
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

//pass node as an array to do recursive lookups
function getNodeFromXML(xml, node)
{
	//IE does not return responseXML
	if(window.ActiveXObject)
	{
		xmlobject=new ActiveXObject("Microsoft.XMLDOM");
		xmlobject.async="false";
		xmlobject.loadXML(xml);
	}
	else
	{
		parser=new DOMParser();
		xmlobject=parser.parseFromString(xml,"text/xml");
	}
	
	// get the XML elements item
	var element;
	
	//Runs through node replaceing element with each next value in the array
	for (var i = 0 ; i < node.length ; i++) {
		element = xmlobject.getElementsByTagName(node)[0];
	}
	
	alert(element);
}

function tinyMCE_onkeyup(id)
{
	tinyMCE.addEventListener(tinyMCE.getInstanceById('content').getDoc(), 'keydown',delayedSave(id));
	return;
}

var t
function delayedSave(id)
{
	if ( exist(id) ){
	clearTimeout(t);
	t=setTimeout("ajaxSave(id)",1000);
	}
}

function exist (a) {
	try { var b=a;}
	catch(e){return false;}
	return true;
}