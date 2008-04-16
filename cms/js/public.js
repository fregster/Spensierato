// JavaScript Document
<!-- AJAX Code for search -->
var xmlHttp = false;
function ajaxFunction(url, parameters)
{
var xmlHttp;
try
  {
  // Firefox, Opera 8.0+, Safari
  xmlHttp=new XMLHttpRequest();
  }
catch (e)
  {
  // Internet Explorer
  try
    {
    xmlHttp=new ActiveXObject("Msxml2.XMLHTTP");
    }
  catch (e)
    {
    try
      {
      xmlHttp=new ActiveXObject("Microsoft.XMLHTTP");
      }
    catch (e)
      {
      alert("Your browser does not support AJAX!");
      //return false;
      }
    }
  }
   //Test the connection
   if (!xmlHttp)
   {
      alert('Cannot create XMLHTTP instance');
      //return false;
   }

    if (xmlHttp.overrideMimeType)
        {
                //If we can force the mimetype
                xmlHttp.overrideMimeType('text/xml');
        }

        xmlHttp.onreadystatechange=function()
    {
        if(xmlHttp.readyState==4)
                {
                        document.getElementById('page_section_main').innerHTML = xmlHttp.responseText;
                }
    }

        xmlHttp.open('GET', url + parameters, true);
        xmlHttp.send(null);
}

function searchFunction()
{
        ajaxFunction('http://www.fryer.org.uk/test/nuneaton/tools/search?s=', document.forms["searchform"].elements["searchtext"].value );
        //+ '&types='+ getCheckedValue(document.forms["searchform"].elements["Types"]
        document.forms["searchform"].elements["searchtext"].value = '';
}