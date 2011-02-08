<!DOCTYPE unspecified PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
	<title>Spensierato Installer</title>
</head>
<body>
<?php
if(is_writable(dirname($_SERVER['SCRIPT_FILENAME'])))
{
	if(copy((INCLUDE_PATH.DS.'example.htaccess'), (dirname($_SERVER['SCRIPT_FILENAME']).DS.'.htaccess')))
	{
		echo('<p>File copy was sucsesful<p>');
	}
	else
	{
		echo('<p>The file copy failed, check the file system permissions and try again</p>');
	}
}
else {
	echo('The root directory is not writable, make it writable to the web user and rerun this script before removing the write permissions again');
}
?>
<a href="<?php echo(Common::document_root_folder_only()); ?>">Return to the Install Page</a>
</body>
</html>
<?php exit; ?>