<?php
abstract class ContentEditor
{
	public static function run($value)
	{
		//Get TinyMCE file
		$value = Settings::Singleton()->get_setting('cms_root').'/js/tiny_mce/'.$value;

		if($value == Settings::Singleton()->get_setting('cms_root').'/js/tiny_mce/tiny_mce_gzip.php')
		{
			//TinyMCE Compressor so just let it run
			include($value);
			die();
		}

		if(file_exists($value))
		{
			header('Content-Description: TinyMCE File ');
			filetransfer::singleton()->get_file($value, true);
		}
		else
		{
			die($value);
		}
	}
}
?>