<?php
/**
* PHP Persistant Caching Interface
*
* Requirements: PHP5
* Sugested: php5-memcache and memcached
*
* Copyright (c) 2009 Paul Fryer (www.fryer.org.uk)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the The GNU Lesser General Public License as published by
* the Free Software Foundation; version 3 or any latter version of the license.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* The GNU Lesser General Public License (LGPLv3) for more details.
*
*
* @package PHPPersistantCache
* @author Paul Fryer <paul@fryer.org.uk>
* @license http://www.opensource.org/licenses/lgpl-3.0.html LGPL
* @link http://www.fryer.org.uk/
* 
* Version 1 Released Jan 2009
*/

define('PREFIX', 'cache_kljliouo_'); //Prefix the key names to help prevent collisions
class persistantcache
{
	private $servers = array(array('host' => 'localhost', 'port'=>11211)); // enter an array for each of your memcache servers
	private $maxsession_cache_time = 0; //0 sets to never expire
	private $max_object_size = 20480; //The string length of the serialized array, your memcache server may have a diffrent setting
	private $retry_time = 30; //Time in seconds before trying to reconnect to memcache if all the servers failed
	
	/**
	 * Session caching
	 * CAUTION Only use this on site where your going to have a small number of users
	 */
	
	private $allow_session_cache = true;
	private $max_objects_in_session_cache = 50; //Prevent the sessions getting to big note there is no way of knowing how many users there are at once.
	
	/**
	 * DO NOT CHANGE THESE
	 */
	private $enabled = false; //Don't change this it is set in the constructors if connected
	private $store; //Memcache object
	private $time; //Local cache of the time
	
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
		//Internaly cache time
		$this->time = time();
		
		//Are we using memcached and if so ensure if it recently failed we don't try to reconnect to quickly
		if(extension_loaded('memcache') && ($_SESSION['diable_memcached'] + $retry_time) < time())
		{
			$this->store = new Memcache;
			
			//Run though the array of servers adding each one to the connection pool
			foreach($this->servers as $server)
			{
				$this->store->addServer($server['host'], $server['port'], TRUE, 1, 1, 1, TRUE, 'memcached_server_fail');
			}
			
			//Mark that we are using an advanced caching mech, purge any session cache
			$this->enabled = true;
			if(isset($_SESSION['cache'])) { unset($_SESSION['cache']); }
		}
		
		//Advanced caching is disabled but poor man caching is enabled
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
	
	/**
	 * Return true if an advanced caching system is in use else false for session or no cache
	 */
	public function advanced()
	{
		return $this->enabled;
	}
	
	/**
	 * Register that a server has gone offline
	 * Needs work
	 */
	public function server_fail($server = null, $port = null)
	{
//		print_r($this->store->getStats()); die();
		$this->disable_memcached();
	}
	
	/**
	 * All servers off line, disable and make offline timestamp
	 */
	private function disable_memcached()
	{
		$this->enabled = false;
		$_SESSION['disable_memcached'] = $this->time();
	}
	
	/**
	 * Build the key name
	 * As memcache can support a key with a max length of 240 bytes build a sha1 hash (256 bits)
	 * Should be safe as there are no currently known collisions in sha1
	 */
	private function build_key($name)
	{
		return sha1(PREFIX.$name);
	}
	
	/**
	 * Public method for saveing data
	 * name
	 * value is what you want to cache
	 * time max storage time
	 */
	public function save($name, $value, $time = 0)
	{
		$name = $this->build_key($name);
		$expire_time = $this->time + $this->maxsession_cache_time;
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
				//Because the session cache can't self clean, ensure the cache is not to big, if it is drop the oldest one.
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
	
	/**
	 * Get an object from the cache by key name
	 */
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
		
		if($result['time'] > $this->time)
		{
			return unserialize($result['value']);
		}
		
		//The data in the session cache is outdated so delete it
		if($this->allow_session_cache)
		{
			unset($_SESSION['cache'][$name]);
		}
		
		return false;
	}
	
	/**
	 * Delete an object from the cache
	 */
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
			--$_SESSION['cache_count'];
		}
	}
	
	/**
	 * Delete every thing from the cache, useful when the database is known to have been updated.
	 */
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

/**
 * Memcached call back function on server fail
 *
 * @param unknown_type $server
 * @param unknown_type $port
 */
function memcached_server_fail($server = NULL, $port = NULL)
{
	persistantcache::Singleton()->server_fail($server = NULL, $port = NULL);
}
?>