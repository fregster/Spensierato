<?php
class persistantcache
{
	
	private $store;
	private $enabled = false;
	private $prefix;
	private $servers = array(array('host' => 'localhost', 'port'=>11211));
	private $maxsession_cache_time = 30; //0 sets to never expire
	private $max_objects_in_session_cache = 50;
	private $max_object_size = 2048;
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
			$_SESSION['cache_count'] = 0;
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
	
	private function build_key($name)
	{
		return sha1($this->prefix = 'cms_'.$GLOBALS['table_prefix'].'_'.$name);
	}
	
	public function save($name, $value, $time = 0)
	{
		$name = $this->build_key($name);
		$expire_time = cms::time() + $this->maxsession_cache_time;
		if($time != 0)
		{
			$expire_time = $expire_time + $time;
		}
		else
		{
			$expire_time = $expire_time + 999999999999999999999999;
		}
		$value = array('value' => serialize($value), 'time' => $expire_time);
		if(strlen($value['value']) < $this->max_object_size)
		{
			if($this->enabled)
			{
				$result = $this->store->replace($name, $value, MEMCACHE_COMPRESSED, $time);
				if( $result == false )
				{
				    $result = $this->store->set($name, $value, MEMCACHE_COMPRESSED, $time);
				}
				return result;
			}
			else if($this->allow_session_cache)
			{
				if($_SESSION['cache_count'] > $max_objects_in_session_cache)
				{
					$_SESSION['cache'] = array_reverse($_SESSION['cache']);
					array_pop($_SESSION['cache']);
					$_SESSION['cache'] = array_reverse($_SESSION['cache']);
				}
				$_SESSION['cache'][$name] = $value;
				++$_SESSION['cache_count'];
				return true;
			}
		}		
		return false;
	}
	
	public function get($name)
	{
		$name = $this->build_key($name);
		if($this->enabled)
		{
			$result = $this->store->get($name);
		}
		else if($this->allow_session_cache)
		{
			$result = $_SESSION['cache'][$name];
		}
		
		if($result['time'] > cms::time())
		{
			return unserialize($result['value']);
		}
		
		return false;
	}
	
	public function drop($name)
	{
		$name = $this->build_key($name);
		if($this->enabled)
		{
			$this->store->delete($name);
		}
		else if($this->allow_session_cache)
		{
			unset($_SESSION['cache'][$name]);
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
				$_SESSION['cache_count'] = 0;
			}
		}
	}
}

function memcached_server_fail($server = NULL, $port = NULL)
{
	persistantcache::Singleton()->server_fail($server = NULL, $port = NULL);
}
?>