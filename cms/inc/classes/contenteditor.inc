<?php
abstract class ContentEditor
{
	public static function run($value)
	{
		//Get TinyMCE file
		$value = Settings::Singleton()->get_setting('cms_root').'js'.DS.'tiny_mce'.DS.$value;
		if($value == Settings::Singleton()->get_setting('cms_root').'js'.DS.'tiny_mce'.DS.'tiny_mce_gzip.php')
		{
			//TinyMCE Compressor so just let it run
			require($value);
			die('Compressor');
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