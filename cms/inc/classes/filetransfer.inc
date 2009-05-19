<?php
class FileTransfer
{
	private static $instance;

	public static function Singleton()
	{
	   if (!isset(self::$instance)) {
		   $c = __CLASS__;
		   self::$instance = new $c;
	   }

	   return self::$instance;
	}

	protected function __construct(){

	}

	public function get_public_file($value)
	{
		$file = Settings::Singleton()->get_setting('cms_root') . '/publicfiles/' . str_ireplace('..', '',  $value);
		$this->get_file($file, true, false);
	}

	public function get_download($id, $cache = true)
	{
		$path = Settings::singleton()->get_setting('cms_folder_downloads').$id;
		$this->get_file($path, $cache);
	}

	public function get_image($id)
	{
		$file = Settings::singleton()->get_setting('cms_folder_downloads').$id;
		$this->get_file($file, true, false);
	}

	public function get_image_orig($id)
	{
		$file = Settings::singleton()->get_setting('cms_folder_orig_images').$id;
		$this->get_file($file, true, true, true);
	}

	public function get_preview($id)
	{
		$file = Settings::singleton()->get_setting('cms_folder_downloads').$id.'-preview';
		if(!file_exists($file) and file_exists(substr($file, 0, -8)) ) {$file = substr($file, 0, -8); }
		$this->get_file($file, true, false, true);
	}

	public function get_file($file, $cache = false, $use_rate_limit = true, $work_out_mime = false)
	{
		if(file_exists($file))
		{
			$file_stat = stat($file);

			if($cache = true)
			{
				//Allow cache for 31 days
				Cache::headers_allow_cache($file, true, 2678400, 2678400);
			}
			else
			{
				Cache::headers_no_cache($file);
			}

			$file_info = database::Singleton()->sql_select_first(array('data'), 'tbl_elements_data', array('id'), array(basename($file)));
			$file_info = json_decode($file_info['data'], TRUE);

			if(!isset($file_info['filename']))
			{
				$filename = basename($file);
			}
			else
			{
				$filename = $file_info['filename'];
			}

			$mime = common::returnMIMEType($filename);

			if($work_out_mime and function_exists('mime_content_type') or $mime === false)
			{
				$mime = mime_content_type($file);
			}

			header('Content-type: ' . $mime);
			header('Content-Length: ' . filesize($file));
			header('Content-Transfer-Encoding: binary');
			header('Content-Disposition: filename='.$filename);

			$rate = Settings::singleton()->get_setting('download_rate');
			if(is_numeric($rate) and $use_rate_limit)
			{
				/* Rate limited download */
				$rate = round($rate * 1024);

				if(class_exists('ffmpeg_movie') and $mime == 'video/flv')
				{
					/* If this is a movie download then the max rate is set to atleast the bits per second of the video + 50 */
						$movie = new ffmpeg_movie($file);
						$rate = max($rate, $movie->getBitRate()) + 50;
				}

				// flush content
			    flush();
			    // open file stream
			    $file = fopen($local_file, "r");
			    while(!feof($file))
			    {
				    // send the current file part to the browser
				    print fread($file, $rate);

				    // flush the content to the browser
				    flush();

				    // sleep one second
				    sleep(1);
			    }

			    // close file stream
			    fclose($file);
			}
			else
			{
				/* Simple download */
				session_write_close();
				readfile($file);
			}
			die();
		}
		else
		{
			die('file '.$file.' not found');
		}
	}

	public function save_file($id, $revision)
	{
		$target = Settings::singleton()->get_setting('cms_folder_downloads').$id;
		$path = Settings::singleton()->get_setting('cms_folder_uploads').$id.'-'.$revision;
		return $this->move_file($path, $target);
	}

	public function move_file($path, $target)
	{
		if(file_exists($path))
		{
			if(virusScan::Singleton()->is_not_virus($path))
			{
				if (file_exists($target)) {
					unlink($target);
				}
				return(rename($path, $target));
			}
		}

		return false;
	}
	
	public static function load_file_as_string($id)
	{
		$path = Settings::singleton()->get_setting('cms_folder_downloads').$id;
		
		$file_info = database::Singleton()->sql_select_first(array('data'), 'tbl_elements_data', array('id'), array(basename($file)));
		$file_info = json_decode($file_info['data'], TRUE);

		if(!isset($file_info['filename']))
		{
			$filename = basename($file);
		}
		else
		{
			$filename = $file_info['filename'];
		}

		$mime = common::returnMIMEType($filename);

		if($work_out_mime and function_exists('mime_content_type') or $mime === false)
		{
			$mime = mime_content_type($file);
		}
		return file_get_contents($path);	
	}
}
?>