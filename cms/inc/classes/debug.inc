<?
class Debug
{
	private static $instance;
	private $debug_level = 0;
	private $debug_log = array();
	private $debug_log_folder = '/var/log/cms';
	
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
	
	function __construct()
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
		if($this->debug_level > 0)
		{
			$log = array(CMS::time(), $what, $details);
			array_push($this->debug_log, $log);
			$this->log_to_file($log);
		}
	}
	
	public function render()
	{
		//Options that trigger debug rendering
		if($this->debug_level >= 5)
		{
			$render = true;
		}
		
		if($_GET['debug'] and User::Singleton()->is_admin())
		{
			$render = true;
		}
		
		if($render)
		{
			return Common::array_to_xml($this->debug_log);
			// TODO cookie::Singleton()->debug();
		}
		else
		{
			return '<xml />';
		}
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