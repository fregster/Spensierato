<?php
class virusScan
{
	protected static $instance;
	protected $command;
	
	public static function singleton()
	{
	   if (!isset(self::$instance)) {
		   
	   		if(class_exists('Settings') && isset(Settings::singleton()->get_setting['virus_scanner']))
	   		{
	   			$c = Settings::singleton()->get_setting['virus_scanner'];
	   		}
	   		else
	   		{
	   			$c = __CLASS__;
	   		}
	   				   
		   self::$instance = new $c;
	   }
	
	   return self::$instance;
	}
	
	protected function __construct()
	{
		
	}
	/**
	 * Default file virus scan method
	 *
	 * @param path to $file
	 * @return false if no virus found or a string if found
	 */
	public function scan_file($file)
	{
		return false;
	}
	
	/**
	 * This is just a proxy method to scan_file to return true or false to make if statments easy
	 *
	 * @param string $file
	 */
	public function is_not_virus($file)
	{
		if(!$this->scan_file($file))
		{
			return true;
		}
		return false;
	}
}
?>