//Javascript Document

function ajaxLoad(id) {
	var ed = tinyMCE.get('content');

	// Do you ajax call here, window.setTimeout fakes ajax call
	ed.setProgressState(1); // Show progress
	window.setTimeout(function() {
		ed.setProgressState(0); // Hide progress
		ed.setContent('HTML content that got passed from server.');
	}, 3000);
}

function ajaxSave(id) {
	var ed = tinyMCE.get('content');

	ed.setProgressState(1); // Show progress
	
	ajaxFunction('http://www.fryer.org.uk/current/cms/ajax/'+id, ed.getContent(), 'POST' );

	ed.setProgressState(0); // Hide progress
	
	ajaxLoad();
}