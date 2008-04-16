<? Cache::headers_allow_cache(__FILE__); ?>
<script type="text/javascript" src="<? echo(Settings::Singleton()->get_setting('document_root')); ?>/tools/js_loggedin"></script>
<script type="text/javascript" src="<? echo(Settings::Singleton()->get_setting('site_folder_js')); ?>/tiny_mce/tiny_mce_gzip.js"></script>
<script type="text/javascript" src="<? echo(Settings::Singleton()->get_setting('site_folder_js')); ?>/admin.js"></script>
<script type="text/javascript">
tinyMCE.init({
	mode : "textareas",
	theme : "advanced"
});
</script>