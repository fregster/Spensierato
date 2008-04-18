<?php
abstract class ContentEditor
{
	static function run($value)
	{
		//Get TinyMCE file
		$value = Settings::Singleton()->get_setting('cms_root').'/js/tiny_mce/'.$value;
		
		if($value == Settings::Singleton()->get_setting('cms_root').'/js/tiny_mce/tiny_mce_gzip.php')
		{
			//TinyMCE Compressor so just let it run
			include($value);
		}
		
		if(file_exists($value))
		{
			$ext = pathinfo($value);
			switch($ext['extension'])
			{
				case 'css':
					$mime = 'text/css';
				break;
				
				case 'js':
					$mime = 'text/javascript';
				break;
				
				default:
					$mime = mime_content_type($value);
				break;
			}
			
			Cache::headers_allow_cache($value, true, 36000, 36000);
			header('Content-type: ' . $mime);
			header('Content-Length: ' . filesize($value));
			header('Content-Description: TinyMCE File ');
			//header('Content-Transfer-Encoding: binary');
				
			session_write_close();
			echo(remove_extra_lines(file_get_contents($value)));
		}
		else
		{
			die($value);
		}
	}
}
?>