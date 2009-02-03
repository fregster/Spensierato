<?php
class persistantcache
{
	
	private $store;
	private $enabled = false;
	private $prefix;
	
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
		$this->prefix = 'cms_'.$GLOBALS['table_prefix'].'_';
		if(extension_loaded('memcache'))
		{
			$this->store = new Memcache;
			$this->enabled = $this->store->connect('localhost', 11211);
		}
	}
	
	public function version()
	{
		return $this->store->getVersion();
	}
	
	public function load()
	{
		return $this->enabled;
	}
	
	public function save($name, $value, $time = 0)
	{
		if($this->enabled)
		{
			$result = $this->store->replace($this->prefix.$name, $value, MEMCACHE_COMPRESSED, $time);
			if( $result == false )
			{
			    $result = $this->store->set($this->prefix.$name, $value, MEMCACHE_COMPRESSED, $time);
			}
		}
		else
		{
			$_SESSION[$this->prefix.$name] = $value;
		}
	}
	
	public function get($name)
	{
		if($this->enabled)
		{
			return $this->store->get($this->prefix.$name);
		}
		else
		{
			return $_SESSION[$this->prefix.$name];
		}
	}
	
	public function drop($name)
	{
		if($this->enabled)
		{
			$this->store->delete($this->prefix.$name);
		}
		else
		{
			unset($_SESSION[$this->prefix.$name]);
		}
	}
}
?>