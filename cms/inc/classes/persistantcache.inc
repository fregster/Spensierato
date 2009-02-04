<?php
class persistantcache
{
	
	private $store;
	private $enabled = false;
	private $prefix;
	private $servers = array(array('host' => 'localhost', 'port'=>11211));
	private $maxsession_cache_time = 30;
	private $retry_time = 30;
	private $allow_session_cache = true;
	
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

		if(extension_loaded('memcache') && ($_SESSION['diable_memcached'] + $retry_time) < time())
		{
			$this->store = new Memcache;
			foreach($this->servers as $server)
			{
				$this->store->addServer($server['host'], $server['port'], TRUE, 1, 1, 1, TRUE, 'memcached_server_fail');
			}

			$this->enabled = true;
			if(isset($_SESSION['cache'])) { unset($_SESSION['cache']); }
		}
		
		if($this->enabled == false && $this->allow_session_cache)
		{
			$_SESSION['cache'] = array();
		}
	}
	
	public function version()
	{
		return $this->store->getVersion();
	}
	
	public function advanced()
	{
		return $this->enabled;
	}
	
	public function server_fail($server = null, $port = null)
	{
//		print_r($this->store->getStats()); die();
		$this->disable_memcached();
	}
	
	private function disable_memcached()
	{
		$this->enabled = false;
		$_SESSION['disable_memcached'] = cms::time();
	}
	
	public function save($name, $value, $time = 0)
	{
		$value = array('value' => $value, 'time' => cms::time());
		if($this->enabled)
		{
			$result = $this->store->replace($this->prefix.$name, $value, MEMCACHE_COMPRESSED, $time);
			if( $result == false )
			{
			    $result = $this->store->set($this->prefix.$name, $value, MEMCACHE_COMPRESSED, $time);
			}
			return result;
		}
		else if($this->allow_session_cache)
		{
			$_SESSION['cache'][$this->prefix.$name] = $value;
			return true;
		}
		
		return false;
	}
	
	public function get($name)
	{
		if($this->enabled)
		{
			$result = $this->store->get($this->prefix.$name);
		}
		else if($this->allow_session_cache)
		{
			$result = $_SESSION['cache'][$this->prefix.$name];
		}
		
		if($result['time'] + $this->maxsession_cache_time > cms::time())
		{
			return $result['value'];
		}
		
		return false;
	}
	
	public function drop($name)
	{
		if($this->enabled)
		{
			$this->store->delete($this->prefix.$name);
		}
		else if($this->allow_session_cache)
		{
			unset($_SESSION['cache'][$this->prefix.$name]);
		}
	}
	
	public function purge()
	{
		if($this->enabled)
		{
			$this->store->flush();
		}
		else
		{
			if(isset($_SESSION['cache'])) 
			{ 
				unset($_SESSION['cache']); 
			}
			
			if($this->allow_session_cache)
			{
				$_SESSION['cache'] = array();
			}
		}
	}
}

function memcached_server_fail($server = NULL, $port = NULL)
{
	persistantcache::Singleton()->server_fail($server = NULL, $port = NULL);
}
?>