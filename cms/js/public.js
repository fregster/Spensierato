function ajaxObject(){
var _1=false;
if(typeof ActiveXObject!="undefined"){
try{
_1=new ActiveXObject("Msxml2.XMLHTTP");
}
catch(e){
try{
_1=new ActiveXObject("Microsoft.XMLHTTP");
}
catch(E){
_1=false;
}
}
}else{
if(XMLHttpRequest){
try{
_1=new XMLHttpRequest();
}
catch(ee){
_1=false;
}
}
}
return _1;
};
var http=ajaxObject();
function ajaxGet(_2,_3){
http.open("GET",_2+_3,true);
http.send(null);
};
function ajaxPost(_4,_5){
http.open("POST",_4,true);
http.setRequestHeader("Content-type","application/x-www-form-urlencoded");
http.setRequestHeader("Content-length",_5.length);
http.setRequestHeader("Connection","close");
http.send(_5);
};
function ajaxGetXML(){
var _6;
if(window.ActiveXObject){
_6=new ActiveXObject("Microsoft.XMLDOM");
_6.async="false";
_6.loadXML(http.responseText);
}else{
_6=http.responseXML;
}
return _6;
};

var notes;
var initalised;
var SmoothScroll;
var autoSearch;
var aSt;
var t;
var ToolTips;
var SpriteTips;
var growl;
function init(){
if(initalised!=true){
stepFontSize(readCookie("fontSize"));
notes=$("notifications");
var se = document.getElementById('searchtext'); 
if(se != null){se.setAttribute("autocomplete", "off"); }
autoSearch=$("ajaxSearchResults");
//notes.fade.bind(notes,[0]);
rounded();
jsShow();
jsHide();
(!typeof(Browser.ie) == 'undefined') ? growl=new Growler.Classic() : growl=new Growler.Modern();
if(typeof(window['n']) != "undefined"){
notification(n);
}
if(typeof window.imageFlowLoad == 'function'){
imageFlowLoad();
}
if(typeof window.adminInit == 'function'){
	adminInit();
}
ToolTips=new Tips($$(".ToolTip"),{className:"ToolTip"});
SpriteTips=new Tips($$(".sprite"),{className:"ToolTip",fixed: true});
initalised=true;
}
};
function delayedSearch(){
clearTimeout(t);
t=setTimeout("searchFunction()",450);
};
function searchMouseIn(){
	clearTimeout(aSt);
	autoSearch.fade('in');
};
function searchMouseOut(){
	autoSearch.fade(1,0.7);
	aSt=setTimeout("autoSearch.fade(0.8, 0)",5000);
}
function searchFunction(){
ajaxGet(ajax_host+"/search?searchtext=",document.forms.searchform.elements.searchtext.value);
http.onreadystatechange=function(){
if(http.readyState==4){
x=http.responseXML.documentElement.getElementsByTagName("result");
if(x.length>0)
{
var txt='<div style="width: 180px;" class="autosearch" onmouseover="searchMouseIn();" onmouseout="searchMouseOut();"><div class="as_header"><div class="as_corner"></div><div class="as_bar"></div></div><ul id="as_ul">';
for (i=0;i<x.length;i++)
{
xx=x[i].getElementsByTagName("title");
{
try
{
al=x[i].getElementsByTagName("page");
txt=txt + '<li class="search_title"><a href="'+al[0].firstChild.nodeValue +'"><span class="tl"> </span><span class="tr"> </span><span>' + xx[0].firstChild.nodeValue + "</span></a></li>";
}
catch (er)
{
}
}
}
txt=txt + '</ul><div class="as_footer"><div class="as_corner"></div><div class="as_bar"></div></div>';
//txt='<div style="left: 347px; top: 1024px; width: 175px;" class="autosearch" id="as_testinput_xml"><div class="as_header"><div class="as_corner"></div><div class="as_bar"></div></div><ul id="as_ul"><li><a name="1" href="#"><span class="tl"> </span><span class="tr"> </span><span><em>W</em>aldron, Ashley<br><small>Leicestershire</small></span></a></li><li><a name="2" href="#"><span class="tl"> </span><span class="tr"> </span><span><em>W</em>heeler, Bysshe<br><small>Lincolnshire</small></span></a></li></ul><div class="as_footer"><div class="as_corner"></div>	<div class="as_bar"></div></div></div>';
}else{
//var txt='<div>No Search Results</div>';
var txt='<div style="width: 180px;" class="autosearch" onmouseover="searchMouseIn();" onmouseout="searchMouseOut();"><div class="as_header"><div class="as_corner"></div><div class="as_bar"></div></div><ul id="as_ul"><li><span class="tl"> </span><span class="tr"> </span><span>No Results</span></li></lu><div class="as_footer"><div class="as_corner"></div><div class="as_bar"></div></div>'
}
document.getElementById("ajaxSearchResults").innerHTML=txt;
autoSearch.fade(-0,0.7);
searchMouseOut();
}
};
};
function topPageReload(_7){
setTimeout("window.top.location.reload(true)",100);
};
function addLoadEvent(_8){
var _9=window.onload;
if(typeof window.onload!="function"){
window.onload=_8;
}else{
window.onload=function(){
_9();
_8();
};
}
};
addLoadEvent(init);
function jsShow(){
var e = document.getElementsByTagName('div');
for (i = 0; i < e.length; i++) {
if (e[i].className.indexOf("js-show") >= 0) { 
e[i].style.display = 'block';
}
}
};
function jsHide(){
var e = document.getElementsByTagName('div');
for (i = 0; i < e.length; i++) {
if (e[i].className.indexOf("js-hide") >= 0) { 
e[i].style.display = 'none';
}
}
};
function jsToggleDisplay(id){
	if(document.getElementById(id).style.display == 'none'){
		jsShowID(id);
	}else{
		jsHideID(id);
	}
};
function jsHideID(id){
	document.getElementById(id).style.display="none";
};
function jsShowID(id){
	document.getElementById(id).style.display="block";
};
function notification(_a,_b,_c){
if(use_growl){
	growl.notify(_a);
}
else{
	if(typeof window.notes.fade == 'function'){
		var _d;
		if(!_c){
		_c="black";
		}
		document.getElementById("notifications").style.backgroundColor=_b;
		document.getElementById("notifications").style.font=_c;
		document.getElementById("notifications").innerHTML=_a;
		notes.fade(-0,0.8);
		_d=setTimeout("notes.fade(0.8, 0)",3000);
}
}
return false;
};
//var fxSlideHorizontal = new Fx.Slide(null, {mode: 'horizontal'})();
function increaseFontSize(){
stepFontSize(1);
};
function decreaseFontSize(){
stepFontSize(-1);
};
function stepFontSize(_e){
var p=document.getElementsByTagName("p");
var _10=new Array("xx-small","x-small","small","medium","large","x-large","xx-large");
var s=3;
_e=parseInt(_e);
for(i=0;i<p.length;i++){
if(p[i].style.fontSize){
for(f=0;f<_10.length;f++){
if(p[i].style.fontSize==_10[f]){
var s=f;
}
}
}
var s=s+_e;
if(s<0){
s=0;
}
if(s>=_10.length){
s=_10.length-1;
}
p[i].style.fontSize=_10[s];
createCookie("fontSize",s-Math.floor(_10.length/2),"365");
}
};
function createCookie(_12,_13,_14){
if(_14){
var _15=new Date();
_15.setTime(_15.getTime()+(_14*24*60*60*1000));
var _16="; expires="+_15.toGMTString();
}else{
var _16="";
}
document.cookie=_12+"="+_13+_16+"; path="+document_root;
};
function readCookie(_17){
var _18=_17+"=";
var ca=document.cookie.split(";");
for(var i=0;i<ca.length;i++){
var c=ca[i];
while(c.charAt(0)==" "){
c=c.substring(1,c.length);
}
if(c.indexOf(_18)==0){
return c.substring(_18.length,c.length);
}
}
return null;
};
function eraseCookie(_1c){
createCookie(_1c,"",-1);
};
function pageReload(){
window.location.href=unescape(window.location.pathname);
};
function NiftyCheck(){
if(!document.getElementById||!document.createElement){
return false;
}
var b=navigator.userAgent.toLowerCase();
if(b.indexOf("msie 5")>0&&b.indexOf("opera")==-1){
return false;
}
return true;
};
function rounded(){
var bk;
if(!NiftyCheck()){
return;
}
var v=getElements("rounded");
var l=v.length;
for(var i=0;i<l;i++){
sizex=4;
sizey=4;
sizex_b=4;
sizey_b=4;
var _22=v[i].className.indexOf("rounded");
var _23=v[i].className.indexOf(" ",_22);
if(_23<0){
_23=v[i].className.length;
}
var _24=v[i].className.substring(_22,_23);
params=_24.split("-");
if(params.length>1){
sizex=parseInt(params[1]);
sizey=parseInt(params[2]);
if(params.length>3){
sizex_b=parseInt(params[3]);
sizey_b=parseInt(params[4]);
}
}
color=get_current_style(v[i],"background-color","transparent");
bk=get_current_style(v[i].parentNode,"background-color","transparent");
AddRounded(v[i],bk,color,sizex,sizey,true);
AddRounded(v[i],bk,color,sizex_b,sizey_b,false);
}
};
function mathsquare(x){
return x*x;
};
function Blend(a,b,_28){
var ca=Array(parseInt("0x"+a.substring(1,3)),parseInt("0x"+a.substring(3,5)),parseInt("0x"+a.substring(5,7)));
var cb=Array(parseInt("0x"+b.substring(1,3)),parseInt("0x"+b.substring(3,5)),parseInt("0x"+b.substring(5,7)));
return "#"+("0"+Math.round(ca[0]+(cb[0]-ca[0])*_28).toString(16)).slice(-2).toString(16)+("0"+Math.round(ca[1]+(cb[1]-ca[1])*_28).toString(16)).slice(-2).toString(16)+("0"+Math.round(ca[2]+(cb[2]-ca[2])*_28).toString(16)).slice(-2).toString(16);
return "#"+("0"+Math.round(ca[0]+(cb[0]-ca[0])*_28).toString(16)).slice(-2).toString(16)+("0"+Math.round(ca[1]+(cb[1]-ca[1])*_28).toString(16)).slice(-2).toString(16)+("0"+Math.round(ca[2]+(cb[2]-ca[2])*_28).toString(16)).slice(-2).toString(16);
};
function AddRounded(el,bk,_2d,_2e,_2f,top){
if(!_2e&&!_2f){
return;
}
var i,j;
var d=document.createElement("div");
d.style.backgroundColor=bk;
var _34=0;
for(i=1;i<=_2f;i++){
var _35,_36,_37;
arc=Math.sqrt(1-mathsquare(1-i/_2f))*_2e;
var _38=_2e-Math.ceil(arc);
var _39=Math.floor(_34);
var _3a=_2e-_38-_39;
var x=document.createElement("div");
var y=d;
x.style.margin="0px "+_38+"px";
x.style.height="1px";
x.style.overflow="hidden";
for(j=1;j<=_3a;j++){
if(j==1){
if(j==_3a){
_35=((arc+_34)*0.5)-_39;
}else{
_36=Math.sqrt(1-mathsquare((_2e-_38-j+1)/_2e))*_2f;
_35=(_36-(_2f-i))*(arc-_39-_3a+1)*0.5;
_35=0;
}
}else{
if(j==_3a){
_36=Math.sqrt(1-mathsquare((_2e-_38-j+1)/_2e))*_2f;
_35=1-(1-(_36-(_2f-i)))*(1-(_34-_39))*0.5;
}else{
_37=Math.sqrt(1-mathsquare((_2e-_38-j)/_2e))*_2f;
_36=Math.sqrt(1-mathsquare((_2e-_38-j+1)/_2e))*_2f;
_35=((_36+_37)*0.5)-(_2f-i);
}
}
x.style.backgroundColor=Blend(bk,_2d,_35);
if(top){
y.appendChild(x);
}else{
y.insertBefore(x,y.firstChild);
}
y=x;
var x=document.createElement("div");
x.style.height="1px";
x.style.overflow="hidden";
x.style.margin="0px 1px";
}
x.style.backgroundColor=_2d;
if(top){
y.appendChild(x);
}else{
y.insertBefore(x,y.firstChild);
}
_34=arc;
}
if(top){
el.insertBefore(d,el.firstChild);
}else{
el.appendChild(d);
}
};
function getElements(_3d){
var _3e=[];
var el=document.getElementsByTagName("DIV");
var _40=new RegExp("\\b"+_3d+"\\b");
for(var i=0;i<el.length;i++){
if(_40.test(el[i].className)){
_3e.push(el[i]);
}
}
return _3e;
};
function get_current_style(_42,_43,_44){
var ee,i,val,apr;
try{
var cs=document.defaultView.getComputedStyle(_42,"");
val=cs.getPropertyValue(_43);
}
catch(ee){
if(_42.currentStyle){
apr=_43.split("-");
for(i=1;i<apr.length;i++){
apr[i]=apr[i].toUpperCase();
}
apr=apr.join("");
val=_42.currentStyle.getAttribute(apr);
}
}
if((val.indexOf("rgba")>-1||val==_44)&&_42.parentNode){
if(_42.parentNode!=document){
val=get_current_style(_42.parentNode,_43,_44);
}else{
val="#FFFFFF";
}
}
if(val.indexOf("rgb")>-1&&val.indexOf("rgba")==-1){
val=rgb2hex(val);
}
if(val.length==4){
val="#"+val.substring(1,1)+val.substring(1,1)+val.substring(2,1)+val.substring(2,1)+val.substring(3,1)+val.substring(3,1);
}
return val;
};
function rgb2hex(_4a){
var x=255;
var hex="";
var i;
var _4e=/([0-9]+)[, ]+([0-9]+)[, ]+([0-9]+)/;
var _4f=_4e.exec(_4a);
for(i=1;i<4;i++){
hex+=("0"+parseInt(_4f[i]).toString(16)).slice(-2);
}
return "#"+hex;
};

init();