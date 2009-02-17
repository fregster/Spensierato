function ajaxLoad(){
clearTimeout(t);
var ed=tinyMCE.get("content");
ed.setProgressState(1);
ajaxGet(ajax_host+"/ajax/edit/",element_id);
http.onreadystatechange=function(){
if(http.readyState==4){
var _2=http.responseText;
ed.setContent(_2);
}
};
ed.setProgressState(0);
};
function ajaxSave(){
clearTimeout(t);
var ed=tinyMCE.get("content");
ed.setProgressState(1);
ajaxPost(ajax_host+"/ajax/edit/"+element_id,"html="+encodeURI(ed.getContent())+"&title="+encodeURI(document.getElementById('title').value));
http.onreadystatechange=function(){
if(http.readyState==4){
if(http.responseText=="SAVED"){
notification("A draft copy of the text has been saved saved","green","white");
}else{
notification(http.responseText,"red","white");
}
}
};
ed.setProgressState(0);
};
function getNodeFromXML(_4,_5){
if(window.ActiveXObject){
xmlobject=new ActiveXObject("Microsoft.XMLDOM");
xmlobject.async="false";
xmlobject.loadXML(_4);
}else{
parser=new DOMParser();
xmlobject=parser.parseFromString(_4,"text/xml");
}
var _6;
for(var i=0;i<_5.length;i++){
_6=xmlobject.getElementsByTagName(_5)[0];
}
alert(_6);
};
var t;
function delayedSave(){
clearTimeout(t);
t=setTimeout("ajaxSave()",10000);
};
function exist(a){
try{
var b=a;
}
catch(e){
return false;
}
return true;
};
function dhtmlLoadScript(_a){
var e=document.createElement("script");
e.src=_a;
e.type="text/javascript";
document.getElementsByTagName("head")[0].appendChild(e);
};
function dhtmlLoadCSS(_c){
var e=document.createElement("link");
e.href=_c;
e.type="text/css";
e.rel="stylesheet";
document.getElementsByTagName("head")[0].appendChild(e);
};
function dynamicjsloader(){
dhtmlLoadCSS(js_host+"/css/submodal");
dhtmlLoadScript(js_host+"/js/submodal/submodal.js");
};
function adminInit(){
dynamicjsloader();
};
