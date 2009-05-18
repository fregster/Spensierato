<?php
class Search
{
	private static $instance;

	public static function Singleton()
	{
	   if (!isset(self::$instance))
	   {
		   $c = __CLASS__;
		   self::$instance = new $c;
	   }

	   return self::$instance;
	}

	protected function __construct(){

	}

	public function __clone()
	{
	   trigger_error('Clone is not allowed.', E_USER_ERROR);
	}

	public function search($string)
	{

	}

	private function highlight($text)
	{

	}

	private function get_results($string)
	{

	}
}
?>