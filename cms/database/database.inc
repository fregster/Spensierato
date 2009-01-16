<?php
require_once('MDB2.php');
require_once('tables.inc');
require_once('layer.inc');

class database_maintenance
{
	public function __construct()
	{
		
	}
	
	public static function Singleton()
	{
	   if (!isset(self::$instance)) {
		   
	   		if(file_exists('./extra/'.$GLOBALS['database_driver'].'.inc'))
	   		{
	   			$c = $GLOBALS['database_driver'];
	   		}
	   		else
	   		{
	   			$c = __CLASS__;
	   		}
	   				   
		   self::$instance = new $c;
	   }
	
	   return self::$instance;
	}
	
	public function optimize()
	{
		return true;
	}
}
?>