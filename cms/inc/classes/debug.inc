<?php
class Debug
{
	private static $instance;
	private $debug_level = 0;
	private $debug_log = array();
	private $debug_log_folder = '/var/log/cms';
	private $profile_loaded = false;

	public static function Singleton()
	{
	   if (!isset(self::$instance))
	   {
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
		if(Settings::Singleton()->get_setting('debug') > 0)
		{
			if(is_numeric(Settings::Singleton()->get_setting('debug')))
			{
				$this->debug_level = Settings::Singleton()->get_setting('debug');
			}
			else
			{
				//Debug setting is not numeric assume basic debug
				$this->debug_level = 1;
			}
		}
	}

	public function record($what, $details)
	{
		if($this->debug_level > 0 or (get::variable('debug','is_set') and User::Singleton()->is_admin()))
		{
			$log = array('time' => time(), 'name' => $what, 'data' => $details);
			array_push($this->debug_log, $log);
			$this->log_to_file($log);
		}
	}

	public function render()
	{

		//Options that trigger debug rendering
		if($this->debug_level >= 5 or (get::variable('debug','is_set') and User::Singleton()->is_admin()))
		{
			global $class_profile;
			if(isset($class_profile[0])){ $this->record('Profile Trace', $class_profile); } // Add the autoloader profiler
			return( xml::array_to_xml_simple($this->debug_log, 'xml', 'item'));
			// TODO cookie::Singleton()->debug();
		}

		return '<xml />';
	}

	private function log_to_file($details)
	{
		//Write to log file
		//TODO write log to file code
		if($this->debug_level >= 10)
		{

		}
	}
}
?>