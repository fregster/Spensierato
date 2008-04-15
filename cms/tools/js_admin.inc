<? Cache::headers_allow_cache(__FILE__); ?>
<script type="text/javascript" src="<? echo(Settings::Singleton()->get_setting('document_root')); ?>/tools/js_loggedin"></script>
<script type="text/javascript" src="tiny_mce/tiny_mce_gzip.js"></script>
<script type="text/javascript">
tinyMCE.init({
	mode : "textareas",
	theme : "advanced"
});

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
</script>