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
		$file = Settings::Singleton()->get_setting('cms_root') . DS .'publicfiles' . DS . str_ireplace('..', '',  $value);
		$this->get_file($file, true, false);
	}

	public function get_download($id, $cache = true)
	{
		$path = Settings::singleton()->get_setting('cms_folder_downloads').DS.$id;
		$this->get_file($path, $cache);
	}

	public function get_image($id)
	{
		$file = Settings::singleton()->get_setting('cms_folder_downloads').DS.$id;
		$this->get_file($file, true, false, null, null, null, false);
	}

	public function get_image_orig($id)
	{
		$file = Settings::singleton()->get_setting('cms_folder_orig_images').DS.$id;
		$this->get_file($file, true, true, true, null, null, false);
	}

	public function get_preview($id)
	{
		$file = Settings::singleton()->get_setting('cms_folder_downloads').DS.$id.'-preview';
		if(!file_exists($file) and file_exists(substr($file, 0, -8)) ) {$file = substr($file, 0, -8); }
		$this->get_file($file, true, false, true);
	}

	public function get_file($file, $cache = false, $use_rate_limit = true, $work_out_mime = false, $no_db_lookup = false, $send_disposition = true)
	{
		if(file_exists($file))
		{
			$file_stat = stat($file);

			if($cache == true)
			{
				//Allow cache for 31 days
				Cache::headers_allow_cache($file, true, 2678400, 2678400);
			}
			else
			{
				Cache::headers_no_cache($file);
			}
			
			//Some times ie in the install process DB lookup's are not available so prevent any calls that may lookup information
			if($no_db_lookup == true)
			{
				$file_info = array();
				$work_out_mime = true;
				$use_rate_limit = false;
			}
			else
			{
				$file_info = database::Singleton()->sql_select_first(array('data'), 'tbl_elements_data', array('id'), array(basename($file)));
				$file_info = json_decode($file_info['data'], TRUE);
			}
			
			if(!isset($file_info['filename']))
			{
				$filename = basename($file);
			}
			else
			{
				$filename = $file_info['filename'];
			}
			
			if(isset($file_info['mime'])){
				$mime =& $file_info['mime'];
			}

			if($work_out_mime || !isset($mime)){
				$mime = common::returnMIMEType($filename);	
			}

			if($work_out_mime and function_exists('mime_content_type') or $mime === false)
			{
				$mime = mime_content_type($file);
			}
			
			@ob_end_clean(); //turn off output buffering to decrease cpu usage
			
			// multipart-download and download resuming support
			$size = filesize($file);

		 	if(isset($_SERVER['HTTP_RANGE']))
			 {
				list($a, $range) = explode("=",$_SERVER['HTTP_RANGE'],2);
				list($range) = explode(",",$range,2);
				list($range, $range_end) = explode("-", $range);
				$range=intval($range);
				if(!$range_end) {
					$range_end=$size-1;
				} else {
					$range_end=intval($range_end);
				}
			 
				$new_length = $range_end-$range+1;
				header("HTTP/1.1 206 Partial Content");
				header("Content-Length: $new_length");
				header("Content-Range: bytes $range-$range_end/$size");
			 } else {
				$new_length=&$size;
				header('Content-Length: ' . $size);
			 }

			header('Content-type: ' . $mime);
			header('Content-Transfer-Encoding: binary');
			header('Accept-Ranges: bytes');
			if($send_disposition){
				header('Content-Disposition: attachment; filename='.$filename);
			}	

			if($use_rate_limit)
			{
				set_time_limit(0); // Remove the time limit for command-line usage;
			 
				if(is_numeric(Settings::singleton()->get_setting('download_rate')))
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
					
				    $bytes_send = 0;
				    // open file stream
				    $file = fopen($local_file, "r");
				    while(!feof($file)  && !connection_aborted() && ($bytes_send<$new_length))
				    {
					    // send the current file part to the browser
					    print fread($file, $rate);
	
					    // flush the content to the browser
					    flush();
					    
					    $bytes_send += strlen($buffer);
	
					    // Sleep for 250 miliseconds;
					    usleep(250000); 
				    }
	
				    // close file stream
				    fclose($file);
				    exit;
				}
			}

			/* Simple download */
			session_write_close();
			readfile($file);
			die();
		}
		else
		{
			die('file '.basename($file).' not found');
		}
	}

	public function save_file($id, $revision)
	{
		$target = Settings::singleton()->get_setting('cms_folder_downloads').DS.$id;
		$path = Settings::singleton()->get_setting('cms_folder_uploads').DS.$id.'-'.$revision;
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
				if(rename($path, $target))
				{
					//Set the file permissions, read / write for owner read for group.
					//This may need to be modified depending on the backup soloution.
					//TODO: Check this works on a WAMP stack
					return(chmod($target, 0640));
				}
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