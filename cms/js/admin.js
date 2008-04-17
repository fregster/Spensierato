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
	currentContent = document.getElementById(id);
	 document.getElementById(id).innerHTML = '<form action="" name="edit_'+id+'"><div><textarea>'+currentContent+'</textarea></div></form>';

}