<?php
/**
 * The warnings class is where non critical problems are registered
 * These warnings are then displayed when an Administrator logs in.
 *
 * Level 4 - null
 * Level 3 - log
 * Level 2 - Show warning to Admins (information, security warnings should go here else you will be advertising faults), default level
 * Level 1 - Show warning to Editors (posible probelms that might stop something working, ie uploads dir not wrtiable)
 * Level 0 - Show warning to All (Super critical problems ie nuncular war)
 */
class warnings
{
	private $warnings;
	private static $instance;

	public static function Singleton()

	{
	   if (!isset(self::$instance)) {
		   $c = __CLASS__;
		   self::$instance = new $c;
	   }

	   return self::$instance;
	}

	public function __clone()
	{
	   trigger_error('Clone is not allowed.', E_USER_ERROR);
	}

	protected function __construct()
	{
		if(!isset($_SESSION['warnings'])){
			$_SESSION['warnings'] = array();
		}
//		$this->warnings =& $_SESSION['warnings'];
		//Do standard warning checks here for easy of code finding.

		//Check paths are writable
		$paths = array( Settings::singleton()->get_setting('cms_folder_downloads'),
						Settings::singleton()->get_setting('cms_folder_uploads'),
						Settings::singleton()->get_setting('cms_folder_orig_images'),
					);

		foreach($paths as $path)
		{
			if (!is_dir($path))
			{
				//If the parent folder is writable auto create the folder
				if (is_writable(dirname($path)))
				{
					mkdir($path, 0750);
					$this->register('Folder did not exsist', 'Auto created '.$path);
				}
				else 
				{
					$this->register('Folder does not exsist', 'Please create the folder '.$path.' as uploads, downloads and local caching may be disabled');	
				}
			}

			if (!is_readable($path))
			{
    			$this->register('Folder is not readable', 'The folder '.$path.' does not have read permissions, downloads are disabled', 1);
			}

			if (!is_writable($path))
			{
    			$this->register('Folder is not writable', 'The folder '.$path.' does not have write permissions uploads are disabled', 1);
    			Settings::Singleton()->overwrite_setting('uploads_enabled', false);
			}
		}

		//Checks only to be done if in admin mode
		if(Options::Singleton()->get_option('admin_mode'))
		{
			$log_folder = Settings::singleton()->get_setting('cms_folder_logs');
			if(!is_writable($log_folder))
			{
				Settings::Singleton()->overwrite_setting('update_version_check', NULL, true);
				if (is_writable(dirname($log_folder)))
				{
					mkdir($log_folder, 0750);
					$this->register('Logs folder did not exsist', 'Auto created '.$log_folder.' Update checks will be done on the next reload');
				}
				else 
				{
					$this->register('Logs are not writable', 'The folder '.$log_folder.' does not have write permissions logging and autoupdates are now disabled', 1);
				}
			}
			else
			{
				$update_log = Settings::singleton()->get_setting('cms_root') . '/writable/logs/update.log';

				if(is_file($update_log))
				{
					if(fopen($update_log, 'w'))
					{
						fclose($update_log);
					}
					else
					{
						Settings::Singleton()->overwrite_setting('update_version_check', NULL, true);
					}
				}
			}
		}
		
		$this->track_errors();

	}

	public function __destruct()
	{
		options::singleton()->set_option('display_warnings', false);
	}
	
	private function track_errors(){
		$this->register('Tracking Errors', 'There have been X errors in the last X days', 'track');
	}

	public function register($warning_title, $description, $level = 2)
	{
		if(!isset($this->warnings[$level][0]))
		{
			$this->warnings[$level] = array();
		}
		array_push($this->warnings[$level], array('title' => $warning_title, 'description' => $description));
		options::singleton()->set_option('display_warnings', true);
	}

	public function get($level = 0)
	{
		$warnings = array();

		while ($level >= 0)
		{
			if(isset($this->warnings[$level]) && count($this->warnings[$level]) > 0)
			{
				array_push($warnings, $this->warnings[$level]);
			}
			$level--;
		}

		if(count($warnings) > 0)
		{
			return $warnings;
		}

		return null;
	}
	
	public function drop($warning_title, $level = null)
	{
		if(isset($level) && $level != null && is_int($level)){
			unset($this->warnings[$level][$warning_title]);
			return true;
		}
		
		$i = 0;
		while ($i < 6)
		{
			if(isset($this->warnings[$i][$warning_title])){
				unset($this->warnings[$i][$warning_title]);
				return true;
			}
			++$i;
		}
		return false;
	}

	public function get_as_xml($level = 0)
	{
		if(Security::Singleton()->can_edit()) { $level = 1; }
		if(User::Singleton()->is_admin()) { $level = 2;}

		return xml::array_to_xml_simple($this->get($level), 'warnings', 'warning');
	}
}