// JavaScript Document
window.setInterval("checkifalive()", 15000);
var alive = true;
var reloadTimer;
function checkifalive(){
	if(alive){
		ajaxGet(ajax_host+"/alive?alive=1&session=",ESESSIN);
		http.onreadystatechange=function(){
			if(http.readyState==4){
				if(http.responseText != '1')
				{
					alive = false;
					//Session has died gray out
					var overlay = new Overlay(); overlay.show();
					
					//Set timer to reload the page to hide any restricted information					
					reloadTimer=setTimeout("window.location.reload()",30000);
				}
			}
		};
	};
};