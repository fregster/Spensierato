<?php
interface database_maintenance
{
	public function __construct()
	{
		
	}
	
	public static function Singleton()
	{
	   if (!isset(self::$instance)) {
		   
	   		if(file_exists('.'.DS.'extra'.DS.DATABASE_DRIVER.'.inc'))
	   		{
	   			$c = 'database_maintenance_'.DATABASE_DRIVER;
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