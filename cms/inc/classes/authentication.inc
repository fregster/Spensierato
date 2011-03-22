<?php
class authentication
{
	private static $instance;
	
	public static function Singleton($force_type = null)
	{
	   if (!isset(self::$instance))
	   {
	   	
	   		if($force_type == null)
	   		{
	   			$force_type = Settings::singleton()->get_setting('authentication_mechanism');
	   		}
			$c = __CLASS__.'_'.$force_type;
			
	   		if(!class_exists($c))
	   		{
	   			$c = __CLASS__;
	   		}

		   self::$instance = new $c;
	   }

	   return self::$instance;
	}
	
	public function auth()
	{
		return false;
	}
	
	public function login_html()
	{
		return null;
	}
}