<?php

/**
 * The warnings class is where non critical problems are registered
 * These warnings are then displayed when an Administrator logs in.
 *
 * Level 3 - log
 * Level 2 - Show warning to Admins (information)
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

	private function __construct()
	{
		$this->warnings = array();
		//Do standard warning checks here for easy of code finding.
		
		//Checks only to be done if in admin mode
		if(Options::Singleton()->get_option('admin_mode'))
		{
			//Check paths are writable
			$paths = array(Settings::singleton()->get_setting('site_folder_uploads'),
							Settings::singleton()->get_setting('site_folder_downloads')
						);
			
			foreach($paths as $path)
			{
				if (!is_dir($path)) 
				{
    				$this->register('Folder does not exsist', 'Please create the folder '.$path);
				}
				
				if (!is_readable($path)) 
				{
    				$this->register('Folder is not readable', 'The folder '.$path.' does not have read permissions');
				}
				
				if (!is_writable($path)) 
				{
    				$this->register('Folder is not writable', 'The folder '.$path.' does not have write permissions');
				}
			}
		}
		
	}
	
	public function register($warning_title, $description, $level = 2)
	{
		if(!is_array($this->warnings[$level]))
		{
			$this->warnings[$level] = array();
		}
		array_push($this->warnings[$level], array('title' => $warning_title, 'description' => $description));
	}
	
	public function get($level = 2)
	{
		$warnings = array();
		
		while ($level >= 0) 
		{
			array_push($warnings, $this->warnings[$level]);
			$level--;
		}
		
		return $warnings;
	}
	
	public function get_as_xml($level = 2)
	{
		$serializer_options = array ( 
			   'addDecl' => FALSE, 
			   'encoding' => Settings::Singleton()->get_setting('char_set'), 
			   'indent' => '  ',
			   'rootName'  => 'warnings',
			   'defaultTagName'  => 'warning'
			);
		return common::array_to_xml($this->get($level), $serializer_options);
	}
}
?>