//Javascript Document

function ajaxLoad() {
	var ed = tinyMCE.get('editor1');

	// Do you ajax call here, window.setTimeout fakes ajax call
	ed.setProgressState(1); // Show progress
	window.setTimeout(function() {
		ed.setProgressState(0); // Hide progress
		ed.setContent('HTML content that got passed from server.');
	}, 3000);
}

function ajaxSave() {
	var ed = tinyMCE.get('editor1');

	// Do you ajax call here, window.setTimeout fakes ajax call
	ed.setProgressState(1); // Show progress
	window.setTimeout(function() {
		ed.setProgressState(0); // Hide progress
		alert(ed.getContent());
	}, 3000);
}

function editContent(id) {
	//Replace content with textarea which should then get replaced by TinyMCE
	currentContent = document.getElementById(id).innerHTML;
	var cols = 80;
	var rows = Math.round((currentContent.length / cols) + 5);
	//document.getElementById(id).innerHTML = '<form action="" name="edit_'+id+'"><div><textarea style="width:100%" cols="'+cols+'" rows="'+rows+'" name="content_'+id+'" id="content_'+id+'">'+currentContent+'</textarea></div></form>';
	document.getElementById(id).innerHTML = '<div id="editwindow"><object classid="clsid:25336920-03F9-11CF-8FD0-00AA00686F13" type="text/html" data="edit='+id+'"></object></div>

}

function closeEditor(id) {
	//This needs to be updated to do an ajax call to always get the latest saved content when closed
	document.getElementById(id).innerHTML = document.forms['edit_'+id].elements["content"].value;
}