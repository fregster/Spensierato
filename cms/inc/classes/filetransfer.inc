<?
class FileTransfer
{
	protected static $instance;
	
	public static function Singleton()
	{
	   if (!isset(self::$instance)) {
		   $c = __CLASS__;
		   self::$instance = new $c;
	   }
	
	   return self::$instance;
	}
	
	public function get_public_file($value)
	{
		$file = Settings::Singleton()->get_setting('cms_root') . '/publicfiles/' . str_ireplace('..', '',  $value);
		$this->get_file($file, true);
	}
	
	public function get_file($file, $cache = false)
	{
		if(file_exists($file))
		{
			$file_stat = stat($file);
			
			if($cache = true)
			{
				//Allow cache for 31 days
				Cache::headers_allow_cache($file, true, 2678400, 2678400, true);
			}
			else
			{
				Cache::headers_no_cache($file);
			}
			
			$mime = common::returnMIMEType($file);
			
			
			header('Content-type: ' . $mime);
			header('Content-Length: ' . filesize($file));
			header('Content-Transfer-Encoding: binary');
			
			session_write_close();
			readfile($file);
			die();
		}
		else
		{
			die('image '.$file.' not found');
		}
	}
}
?>