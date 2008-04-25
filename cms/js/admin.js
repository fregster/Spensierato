//Javascript Document

function ajaxLoad() {
	var ed = tinyMCE.get('content');
	
	ed.setProgressState(1); // Show progress

	ajaxGet('http://www.fryer.org.uk/current/cms/ajax/edit/', element_id);
	
	http.onreadystatechange=function()
    	{
        	if(http.readyState==4)
			{
				//var response = http.responseText;
				//var node=new Array('element','data');
				//getNodeFromXML(response, node);
				ed.setContent(response);
			}
		}

	ed.setProgressState(0); // Hide progress
}

function ajaxSave() {
	var ed = tinyMCE.get('content');

	ed.setProgressState(1); // Show progress
	
	ajaxPost('http://www.fryer.org.uk/current/cms/ajax/edit/'+element_id, '<html>'+ed.getContent()+'</html>');
	
	http.onreadystatechange=function()
    	{
        	if(http.readyState==4)
			{
				if(http.responseText == 'SAVED'){
					ajaxSaved();
				} else {
					alert(http.responseText);
				}
			}
		}

	ed.setProgressState(0); // Hide progress
	
	//ajaxLoad();
}

function ajaxSaved()
{
	alert('Ajax saved');
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

var t
function delayedSave()
{
	clearTimeout(t);
	t=setTimeout("ajaxSave()",10000);
}

function exist (a) {
	try { var b=a;}
	catch(e){return false;}
	return true;
}