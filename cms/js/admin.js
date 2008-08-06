//Javascript Document

function ajaxLoad() {
	clearTimeout(t);
	var ed = tinyMCE.get('content');
	
	ed.setProgressState(1); // Show progress

	ajaxGet(ajax_host+'/ajax/edit/', element_id);
	
	http.onreadystatechange=function()
    	{
        	if(http.readyState==4)
			{
				var response = http.responseText;
				ed.setContent(response);
			}
		}

	ed.setProgressState(0); // Hide progress
}

function ajaxSave() {
	clearTimeout(t);
	var ed = tinyMCE.get('content');

	ed.setProgressState(1); // Show progress
	
	ajaxPost(ajax_host+'/ajax/edit/'+element_id, '<html>'+ed.getContent()+'</html>');
	
	http.onreadystatechange=function()
    	{
        	if(http.readyState==4)
			{
				if(http.responseText == 'SAVED'){
					notification('A draft copy of the text has been saved saved', 'green', 'white');
				} else {
					notification(http.responseText, 'red', 'white');
				}
			}
		}

	ed.setProgressState(0); // Hide progress
	
	//ajaxLoad();
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

function dhtmlLoadScript(url)
{
   var e = document.createElement("script");
   e.src = url;
   e.type="text/javascript";
   document.getElementsByTagName("head")[0].appendChild(e);
}

function dhtmlLoadCSS(url)
{
   var e = document.createElement("link");
   e.href = url;
   e.type="text/css";
   e.rel="stylesheet";
   document.getElementsByTagName("head")[0].appendChild(e);
}

function dynamicjsloader()
{
	 dhtmlLoadCSS(js_host+"/css/submodal");
	 dhtmlLoadScript(js_host+"/js/submodal/submodal.js");
}