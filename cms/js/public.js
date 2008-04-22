// JavaScript Document
<!-- AJAX Code for search -->
var xmlHttp = false;
function ajaxFunction(url, parameters, how)
{
var http;
var method;

if (how == null || how == 'GET') 
{
	method = 'GET';
}
else 
{
	method = 'POST';
}


try
  {
  // Firefox, Opera 8.0+, Safari
  http=new XMLHttpRequest();
  }
catch (e)
  {
  // Internet Explorer
  try
    {
    http=new ActiveXObject("Msxml2.XMLHTTP");
    }
  catch (e)
    {
    try
      {
      http=new ActiveXObject("Microsoft.XMLHTTP");
      }
    catch (e)
      {
      alert("Your browser does not support AJAX!");
      //return false;
      }
    }
  }
   //Test the connection
   if (!http)
   {
      alert('Cannot create XMLHTTP instance');
      //return false;
   }

    if (http.overrideMimeType)
        {
                //If we can force the mimetype
                http.overrideMimeType('text/xml');
        }

	http.onreadystatechange=function()
    {
        if(http.readyState==4)
                {
                        document.getElementById('page_section_main').innerHTML = http.responseText;
                }
    }

	if(method == 'GET')
	{
        http.open('GET', url + parameters, true);
        http.send(null);
	}
	
	if(method == 'POST')
	{
		http.open('POST', url, true);
		//Send the proper header information along with the request
		http.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
		http.setRequestHeader("Content-length", params.length);
		http.setRequestHeader("Connection", "close");
		http.send(parameters);
	}
}

function searchFunction()
{
        ajaxFunction('http://www.fryer.org.uk/test/nuneaton/tools/search?s=', document.forms["searchform"].elements["searchtext"].value );
        //+ '&types='+ getCheckedValue(document.forms["searchform"].elements["Types"]
        document.forms["searchform"].elements["searchtext"].value = '';
}