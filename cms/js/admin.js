if(typeof window.imageFlowLoad != 'function'){
function getElementsByClass(searchClass,node,tag) {
        var classElements = new Array();
        if ( node == null )
                node = document;
        if ( tag == null )
                tag = '*';
        var els = node.getElementsByTagName(tag);
        var elsLen = els.length;
        var pattern = new RegExp("(^|\\s)"+searchClass+"(\\s|$)");
        for (i = 0, j = 0; i < elsLen; i++) {
                if ( pattern.test(els[i].className) ) {
                        classElements[j] = els[i];
                        j++;
                }
        }
        return classElements;
}
}
function ajaxLoad(revision){
clearTimeout(t);
var ed=tinyMCE.get("content");
ed.setProgressState(1);
var url = '';
if (typeof revision != 'undefined')
{
url = ajax_host + "/revision/" + revision + "/";
}else{
url = ajax_host + "/edit/";
}
ajaxGet(url,element_id);
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
ajaxPost(ajax_host+"/edit/"+element_id,"html="+encodeURI(ed.getContent())+"&title="+encodeURI(document.getElementById('title').value));
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

function dhtmlLoadCSS(_c){
var e=document.createElement("link");
e.href=_c;
e.type="text/css";
e.rel="stylesheet";
document.getElementsByTagName("head")[0].appendChild(e);
};
function dynamicjsloader(){
//dhtmlLoadCSS(js_host+"/skin/subModal.css");
//dhtmlLoadScript(js_host+"/js/submodal/submodal.js");
};
var sortInput;
var initMultiBoxAdmin;
var fManager; 
var assetBasePath = document_root+'/js/fileman/';
function adminInit(){
dynamicjsloader();
sortInput = new String();
updater();
initMultiBoxAdmin = new multiBox({
mbClass: '.submodal',
container: $(document.body),
maxSize: {w:700, h:500},
recalcTop: true
});
//if(typeof FileManager == 'object'){
fManager = new FileManager({
    hideOnClick: true,
    upload: true,
    download: true,
    destroy: true,
    rename: true,
    createFolders: true,
	url: document_root+'/tools/filemanager',
	baseURL: document_root+'/',
	assetBasePath: assetBasePath,
	language: 'en',
	uploadAuthData: {session: ESESSIN}
	});
	$('mediaManagerLink').addEvent('click', fManager.show.bind(fManager));
//}
};

function get_diff(name){
	var approvals = document.getElementsByName(name);
	var post_params = '';
	for(i = 0; i < approvals.length; i++){
		if (approvals[i].checked == true) {
			post_params = post_params + 'approvals[]=' + approvals[i].value + '&';
		}
	}

	ajaxPost(ajax_host+"/ajax/approvals/"+element_id, encodeURI(post_params));

	http.onreadystatechange=function(){
		if(http.readyState==4){
			var ed=tinyMCE.get("content");
			var diff=http.responseText;
			ed.setContent(diff);
		}
	};
};

function show_old_approvals(toggle, name){
	var old_approvals = document.getElementsByName(name);
	var display_style;

	if (toggle.checked == true) {
		display_style = 'block';
	}
	else{
		display_style = 'none';
	}

	for (i = 0; i < old_approvals.length; ++i){
		old_approvals[i].style.display = display_style;
	}
};

/* when the DOM is ready */
function updater(){
	/* grab important elements */
	var list = document.getElementById('page_section_body');

	/* worker function */
	var fnSubmit = function(){
		
		var sortOrder = new Array();
		var objects = new Array();
		objects = list.getElementsByClassName('object');
		
		var i = 0;
		while(i < objects.length) {
			eid = objects[i].getElementsByClassName('elementadmin');
			sortOrder.push(eid[0]['id'].substring(4));
			i++;
		};
		sortInput = sortOrder.join(',');
	
		if(document.getElementById("saveSortOrder").disabled == true){
			document.getElementById("saveSortOrder").disabled=false;
			var moveIcons = new Array();
			moveIcons = list.getElementsByClassName('move');
			var i2 = 0;
			while(i2 < moveIcons.length) {
				moveIcons[i2].style.display = 'none';
				i2++;
			};
		}
	};
	
	/* sortables that also *may* */
	new Sortables(list,{
		constrain: true,
		clone: true,
		revert: true,
		onLoad: function(el,clone) {
			fnSubmit();
		},
		onComplete: function(el,clone) {
			fnSubmit();
		}
	});
};

function saveSortOrder(){
	/* get the request object ready;  re-use the same Request */
	var messageBox = document.getElementById('notifications');
	messageBox.set('text','Updating the sort order in the database.');
	ajaxPost(ajax_host+"/ajax/reorder/", 'order='+sortInput);
	http.onreadystatechange=function(){
		if(http.readyState==4){
			alert(http);
		}
	}
};