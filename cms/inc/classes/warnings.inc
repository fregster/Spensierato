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
	private $warnings = array();
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
		//Do standard warning checks here for easy of code finding.
		
		
	}
	
	public function register($warning_title, $description, $level = 2)
	{
		
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
		$serializer_options = NULL;
		return common::array_to_xml($this->get($level), $serializer_options);
	}
}
?>