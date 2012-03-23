<?php
/**
 * Main database class
 * Included from database.inc to ensure the required defines and values are configured
 * 
 * 
 * Requirements: PHP5
 * Copyright (c) 2011 Paul Fryer (www.fryer.org.uk)
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
 * @package PDA (PHP Database Abstraction)
 * @author Paul Fryer <paul@fryer.org.uk>
 * @license http://www.opensource.org/licenses/lgpl-3.0.html LGPL
 *
 */

class database
{
	private $options = array('allow_external_calls' => true);
	private static $instance;

	protected $prefix;
	private $error_file = false;

	private $sql_cache_enabled = true;
	private $sql_cache = array();
	private $sql_cache_max_results = 50; //For local SQL caching not used if external caching is enabled
	private $sql_cache_max_freshness = 3000; //Max time before DB is re-quired, if the connection is dead and a cached result is available (max_time) the stale result is still used 
	private $sql_cache_max_time = 1500; //Max cache time in seconds
	private $sql_cache_hit = false;
	private $sql_cache_fresh = false;
	private $sql_cache_hits = 0;

	private $external_cache = true;
	protected $full_text_search = false;

	private $sql_log_array = array();
	protected $site_sql_count = 0;
	private $sql_time = 0;

	private $use_external_log = true;
	private $show_error_sql = false;
	private $strip_slashes = true;
	protected $user_support = true;
	protected $escape_method = 'addslashes';
	protected $allow_extenal_escaping = false; //False uses the internal method's only which prevent's opening a DB connection unless required.
	
	protected $col_quote = '`';
	protected $table_quote = '`';
	protected $value_quote = "'";
	protected $database_quote = '';
	
	private $db_object;
	
	/**
	 * Prepared Statment work
	 */
	protected $sql_use_prepared_statments_if_available = true; //Option
	protected $sql_can_prepare_statments = false; //DB specific automatically set option
	protected $sql_use_prepared_statments = false; //Calculated value
	protected $sql_prepared_statment_placeholder_name = 'v';
	protected $sql_prepared_statment_placeholder_prefix = ':';
	protected $sql_prepared_statment_use_short_holder = false;
	private $sql_values = null;
	private $sql_types = null;
	private $sql_prepared_count = 1;
	private $sql_values_id = 0;

	/**
	 * Privately cached vars
	 */
	private $time;
	private $microtime;
	
	/**
	 * Handle for custom backends
	 * $db
	 */
	protected $db = false;

	/**
	 * 
	 * Database Singleton for accessing the default database
	 * @param array $dsn
	 */
	public static function Singleton($dsn_read = null, $rebuild = false, $dsn_write = null)
	{			
		if($rebuild){
			self::$instance == null;
		}

		if (!isset(self::$instance) || isset($dsn_read['phptype']))
		{
			
			/**
			 * Build the default DSN
			 */
			if(is_null($dsn_read) || !isset($dsn_read['phptype'])){
				$dsn_read = array(
				    'phptype'  => $GLOBALS['database_driver'],
				);
				
				if(isset($GLOBALS['database_user_read'])){
					$dsn_read['username'] = $GLOBALS['database_user_read'];
				}
				
				if(isset($GLOBALS['database_password'])){
					$dsn_read['password'] = $GLOBALS['database_password'];
				}
				
				if(isset($GLOBALS['database_host_read'])){
					$dsn_read['hostspec'] = $GLOBALS['database_host_read'];
				}
				
				if(isset($GLOBALS['database_name'])){
					$dsn_read['database'] = $GLOBALS['database_name'];
				}
				
				if(isset($GLOBALS['prefix']) && !isset($dsn_read['prefix']))
				{
					$dsn_read['prefix'] = $GLOBALS['prefix'];
				}
				
				if(isset($GLOBALS['database_mode']) && !isset($dsn_read['database_mode']))
				{
					$dsn_read['database_mode'] = $GLOBALS['database_mode'];
				}
				
				if(isset($dsn_read['database_mode']) && $dsn_read['phptype'] == 'sqlite')
				{
					$dsn_read['database_mode'] = '0660';
				}
			}	
			
			if(is_null($dsn_write) || !isset($dsn_write['phptype'])){
				$dsn_write = $dsn_read;
				
				if(isset($GLOBALS['database_user_readwrite']))
				{
					$dsn_write['username'] = $GLOBALS['database_user_readwrite'];
				}
				
				if(isset($GLOBALS['database_password_write']))
				{
					$dsn_write['password'] = $GLOBALS['database_password_write'];
				}
				
				if(isset($GLOBALS['database_host_write']))
				{
					$dsn_write['hostspec'] = $GLOBALS['database_host_write'];
				}
				
				if(isset($GLOBALS['database_mode']))
				{
					$dsn_write['database_mode'] = $GLOBALS['database_mode'];
				}
				
				if(isset($dsn_write['database_mode']) && $dsn_write['phptype'] == 'sqlite')
				{
					$dsn_write['database_mode'] = '0660';
				}
			}
			
			self::$instance = database::factory($dsn_read, NULL, NULL, $dsn_write);
		}
		
		return self::$instance;
	}
	
	/**
	 * 
	 * For creating the correct SQL layer to interact with the database
	 * Returns an object of the correct type
	 * 
	 * @param array $dsn
	 */
	public static function factory($dsn_read = null, $options = NULL, $mods = NULL, $dsn_write = null)
	{
		if(isset($dsn_read['phptype'])){
			$c = 'database_'.$dsn_read['phptype'];
			$driver =& $dsn_read['phptype'];
		}else{
			$c = 'database_'.DATABASE_DRIVER;
			$driver = DATABASE_DRIVER;
		}
		
		include_once(DATABASE_LAYER_PATH . DS . 'server_types' . DS . $driver.'.inc');
	   	if(!class_exists($c))
	   	{
	   		die('Unable to create correct database layer type ('.$c.')');
	   	}
	   	
	   	if(isset($dsn_read['phptype'])){
	   		return new $c($dsn_read, $options, $mods, $dsn_write);
	   	}
	   	
	   	return new $c;
	}

	/**
	 * Database construct
	 * If the database class has been called directly throught the singleton then connect to the default database
	 *
	 * It is intended a new connection to a seperate DB can be started with $new_db = Database::custom_dsn($dsn);
	 * The call $result = $new_db()->sql_select_first(......);
	 *
	 * @param Databse_execution object $db
	 */
	protected function __construct($dsn_read = null, $options = NULL, $mods = NULL, $dsn_write = null)
	{
		$this->time = time();
		$this->microtime = microtime();
		$this->prefix = DATABASE_TABLE_PREFIX;
		if(isset($dsn_read['prefix'])){
			$this->prefix = $dsn_read['prefix'];
		}
		
		if(!class_exists('settings')){
			$this->options['allow_external_calls'] = false;
		}
		
		if(is_null($dsn_write) || !isset($dsn_write['phptype'])){
			$dsn_write = $dsn_read;
		}
		
		if(isset($db) && !is_null($db))
		{
			$this->db = $db;
		}
		if(!$this->db) 
		{
			$this->db = database_execution::factory($dsn_read);
			$this->db->set_dsn($dsn_read, $dsn_write);
		}
		
		if($this->db->supports_prepared_statments() && !defined('DATABASE_DISABLE_PREPARED_SQL_STATMENTS'))
		{
			$this->sql_can_prepare_statments = true;
			$this->sql_use_prepared_statments = true;
			$this->sql_values = array();
			$this->sql_types = null;
			$this->sql_prepared_statment_placeholder_prefix = $this->db->prepared_statment_placeholder();
		}
		
		if(defined('DATABASE_INSTALL_MODE') || defined('DATABASE_DISABLE_SQL_CACHE') || ($this->options['allow_external_calls'] && !Settings::Singleton()->get_setting('cache_database_queries')))
		{
			$this->sql_cache_enabled = false;
		}
		
		if(!function_exists($this->escape_method))
		{
			$this->escape_method = 'addslashes';
		}

		if(!$this->error_file)
		{
			$this->error_file = DATABASE_LAYER_PATH . DS . 'dberror.html';
		}
	}
	
	protected function sql_id()
	{
		++$this->sql_values_id;
		$this->sql_values[$this->sql_values_id] = array();
		return $this->sql_values_id;
	}
	
	/**
	 * Force the DB to update
	 */
	public function commit(){
		$this->db->commit();
	}

	/**
	 * 
	 * Use to force the destruction of the DB connection and if the object is destroyed, destroy the related DB object
	 */
	public function __destruct()
	{
		$this->db->destroy();
	}
	
	public function type()
	{
		if(isset($this->db) && $this->db){
			return $this->db->type;
		}
		
		return $this->dsn['backend'];
	}

	/**
	 * 
	 * Create a custom DB interface object rather than the singleton
	 * @param array $dsn
	 * @param array $options
	 * @param array $mods
	 */
	public static function custom_db_object($dsn, $options = NULL, $mods = NULL)
	{
		//A DSN has been set so create the connection with the specified options
		if(!isset($dsn['driver'])){
			$dsn['driver'] = null;
		}		
		return database_execution::factory($dsn)->custom_db_object($dsn, $options, $mods);
	}
	
	/**
	 * 
	 * Return an array of supported databases
	 */
	public function supported_types()
	{
		return array('sqlite', 'mysqli', 'sqlsrv');
	}
	
	/**
	 * 
	 * Return an array of supported database drivers
	 */
	public function supported_drivers()
	{
		return array('pdo', 'mdb2', 'php');
	}

	/**
	 * Here for legacy purposes
	 * @return true / false from DB method
	 */
	public function force_write_user()
	{
		return $this->db->force_write_user();
	}

	/**
	 * Here for legacy purposes
	 * @return true / false from DB method
	 */
	public function destroy_write_user()
	{
		if($this->user_support){
			$this->db->destroy_write_user();
		}
	}

	/**
	 * 
	 * DB objects should be singular and are created on demand and so should not be cloned
	 */
	public function __clone()
	{
	   trigger_error('Clone is not allowed.', E_USER_ERROR);
	}

	/**
	 * 
	 * If internal db caching may cause problems IE time sensitive data then use this to disable PHP side DB caching
	 */
	public function sql_cache_disable()
	{
		//Disable cacheing
		$this->sql_cache_enabled = false;
		
		//Clear the cache
		if(isset($_SESSION['sql_cache'])){
			unset($_SESSION['sql_cache']);
		}
	}

	/**
	 * 
	 * Return the SQL query log and information as an array
	 */
	public function sql_show_log()
	{
		return $this->sql_log_array;
	}

	/**
	 * 
	 * Return int value of time spent waiting for SQL methods and queries
	 */
	public function sql_time()
	{
		return $this->sql_time;
	}

	/**
	 * 
	 * Return array int number of SQL queries since object created
	 * array(DB SQL, %percent Cached, Cached SQL);
	 */
	public function sql_count()
	{
		$percent = 0;
		if($this->site_sql_count > 0 && $this->sql_cache_hits > 0)
		{
			$percent = 100 / ($this->site_sql_count + $this->sql_cache_hits) * $this->sql_cache_hits;
		}
		return array(($this->site_sql_count - $this->sql_cache_hits), $percent, $this->sql_cache_hits);
	}

	/**
	 * 
	 * Return Bool if external caching is enabled.
	 * Currently writen to use PersistantCache class
	 */
	public function use_memcached()
	{
		if($this->options['allow_external_calls'])
		{
			$this->external_cache = true;
		}
	}
	
	/**
	 * Is Full Text Searching available true / false
	 */
	public function fts()
	{
		return $this->full_text_search;
	}
	
	/**
	 * 
	 * Return a cleaned value or the aprorate prepared statment place holder
	 * @param mixed $var
	 */
	protected function sql_clean_input_value($var, $id)
	{
		if($this->sql_use_prepared_statments)
		{
			if($this->sql_prepared_statment_use_short_holder)
			{
				$this->sql_values[$id][] = $var;
				++$this->sql_prepared_count;
				return '?';
				
			}
			$key = $this->sql_prepared_statment_placeholder_name.$this->sql_prepared_count;
			$this->sql_values[$id][$key] = $var;
			if(is_int($var)){
				$this->sql_types[] = 'integer';
			}
			else {
				$this->sql_types[] = 'text';
			}
			++$this->sql_prepared_count;
			return $this->sql_prepared_statment_placeholder_prefix.$key;
		}
		return $this->sql_clean_input($var);
	}

	/**
	 * 
	 * Return a clean SQL version of an input including the quotes
	 * Works out the best available cleaning method and returns a string
	 * @param $var
	 * @param $quote
	 */
	protected function sql_clean_input($var, $quote = null)
	{
		//If the input is just a number just return it
		if(is_numeric($var)) { return $var; }
		if($quote === null) { $quote = $this->value_quote; }
		
		if($this->allow_extenal_escaping)
		{
			return $quote.call_user_func($this->escape_method,$var).$quote;
		}
		return $quote.addslashes($var).$quote;
	}

	/**
	 * 
	 * Internal logging method, records SQL count, SQL, time initial SQL method was called, weather it was a cached result and an optional SQL result
	 * @param $sql_count
	 * @param $sql_query
	 * @param $sql_start_time
	 * @param $sql_cache_hit
	 * @param $sql_result
	 */
	private function sql_log($sql_count, $sql_query, $sql_start_time, $sql_cache_hit, $sql_result = NULL)
	{
		if($this->options['allow_external_calls'])
		{
			if(Settings::Singleton()->get_setting('debug') > 0)
			{
				if($sql_start_time != 0)
				{
					$sql_exc_time = microtime() - $sql_start_time;
				}
				else
				{
					$sql_exc_time = 0;
				}

				if ( ($sql_cache_hit == false) or (Settings::Singleton()->get_setting('debug') > 1)	)
				{
					$details = array('count' => $sql_count, 'query' => $sql_query, 'time' => $sql_exc_time, 'cached' => $sql_cache_hit);
					if(isset($this->sql_values[0])) { $details['values'] = $this->sql_values; }
					array_push($this->sql_log_array, $details);
					$this->sql_time = $this->sql_time + $sql_exc_time;

					if(Settings::Singleton()->get_setting('debug') > 2)
					{
						$details['called'] = debug_backtrace();
						$i = 1;
						while((basename($details['called'][$i]['file']) == 'layer.inc' || (isset($details['called'][$i]['class']) && basename($details['called'][$i]['class']) == 'database') || @basename($details['called'][($i+1)]['file']) == 'layer.inc' || @basename($details['called'][($i+2)]['file']) == 'layer.inc' || @basename($details['called'][($i+3)]['file']) == 'layer.inc' && $i < 6)){
							++$i;
						}
						
						$details['called'] = $details['called'][$i]['file']. ' '.$details['called'][$i]['line'];
					}
					
					Debug::Singleton()->record('SQL', $details);
				}
			}
		}
	}

	//Proxy to standard sql select forces limit to 1
	/**
	 * 
	 * Standard SQL Select creation method with LIMIT 1 forced
	 * @param $fields
	 * @param $table
	 * @param $where_fields
	 * @param $where_values
	 * @param $order_fields
	 * @param $order_directions
	 * @param $FORCE_NO_CACHE
	 */
	public function sql_select_first($fields = NULL, $table, $where_fields = NULL, $where_values = NULL, $order_fields = NULL, $order_directions = NULL, $FORCE_NO_CACHE = FALSE)
	{
		$result = $this->sql_select($fields, $table, $where_fields, $where_values, $order_fields, $order_directions, $FORCE_NO_CACHE, 1);
		if(!isset($result[0])) { $result[0] = null; }
		return  $result[0];
	}

	// Standard Select function
	// sql_select(Fields(if Null *), Table, Where (array), Where values (array must match number of fields), Order fields (array), Prder direction (if not sec ASC)
	/**
	 * 
	 * Standard SQL Select build statment
	 * @param string $fields
	 * @param string $table
	 * @param array $where_fields
	 * @param array $where_values
	 * @param array $order_fields
	 * @param ASC or DESC $order_directions
	 * @param bool $FORCE_NO_CACHE
	 * @param int $limit
	 */
	public function sql_select($fields = NULL, $table, $where_fields = NULL, $where_values = NULL, $order_fields = NULL, $order_directions = NULL, $FORCE_NO_CACHE = FALSE, $limit = NULL)
	{
		$id = $this->sql_id();
		$sql = $this->sql_select_build($fields, $table, $where_fields, $where_values, $order_fields, $order_directions, $id);
		return $this->sql_select_exe($sql, NULL, $FORCE_NO_CACHE, $table, $id);
	}
	
	/**
	 * 
	 * This is a self joining SQL statment to allow selection recursivly where hierarchical or linked data is stored in a single table
	 * @param string | array $fields
	 * @param string $table
	 * @param string $join_col
	 * @param string | int $join_value
	 * @param string $where_id_field
	 * @param string | int $where_id_value
	 * @param int $max_recursion
	 * @param bool $FORCE_NO_CACHE
	 */
	public function sql_select_recursive($fields = NULL, $table, $join_col, $join_value, $where_id_field = null, $where_id_value = null, $max_recursion = 5, $FORCE_NO_CACHE = FALSE)
	{
		$id = $this->sql_id();
		$sql = $this->sql_select_recursive_build($fields, $table, $join_col, $join_value, $where_id_field, $where_id_value, $max_recursion, $id);
		return $this->sql_select_exe($sql, NULL, $FORCE_NO_CACHE, $table, $id);
	}
	
	private function sql_select_recursive_build($fields = NULL, $table, $join_col, $join_value, $where_id_field, $where_id_value, $max_recursion, $id)
	{
		if($where_id_field == null){
			$where_id_field = $join_col;
		}
		
		if($where_id_value == null){
			$where_id_value = $join_value;
		}
		
		$sql_temp_select = '';

		if(!is_int($max_recursion) || $max_recursion < 1)
		{
			return false;
		}
		
		$recursion = 0;
		while ($recursion <= $max_recursion)
		{
			if($fields == NULL)
			{
				$sql_temp_select .= " t$recursion.*";
				if($recursion < $max_recursion)
				{
					$sql_temp_select .= ', ';
				}
			}
			else if (!is_array($fields) )
			{
				$sql_temp_select .= ' ' . $this->sql_clean_input('t'.$recursion.'.'.$fields, ''). ' AS '.$this->sql_clean_input($fields, '').$recursion;
				if($recursion < $max_recursion)
				{
					$sql_temp_select .= ', ';
				}
			}
			else
			{
				$i = 0;
				$count_fields = count($fields);
				while($i <= ($count_fields - 1))
				{
					$sql_temp_select .= $this->sql_clean_input('t'.$recursion.'.'.$fields[$i], '') . ' AS '.$this->sql_clean_input($fields[$i], '').$recursion;
	
					if($i < ($count_fields - 1) || $recursion < $max_recursion)
					{
						$sql_temp_select .= ', ';
					}
	
					$i++;
				}
			}
			$recursion++;
		}
		
		$recursion = 0;
		
		$sql_temp_join = '';
		
		while($recursion < $max_recursion)
		{
			$sql_temp_join .= ' LEFT JOIN '.$this->sql_clean_input($this->prefix.$table, $this->table_quote).' AS t'.($recursion+1).' ON t'.($recursion+1).'.'.$where_id_field . ' = t'.($recursion).'.'.$join_col  ;
			$recursion++;
		}

		return "SELECT $sql_temp_select FROM ".$this->sql_clean_input($this->prefix.$table, $this->table_quote).' AS t0 '.$sql_temp_join.' WHERE t0.'.$where_id_field.' = '.$this->sql_clean_input_value($where_id_value, $id);

	}
	
	/**
	 * 
	 * SQL Count statment
	 * @param string $fields
	 * @param string $table
	 * @param string $group_col
	 * @param array $where_fields
	 * @param array $where_values
	 * @param bool $ORDER_BY_COUNT
	 * @param int $HAVING_COUNT
	 */
	public function sql_select_count_group_by_col($fields = NULL, $table, $group_col, $where_fields= NULL, $where_values= NULL, $ORDER_BY_COUNT = TRUE, $HAVING_COUNT = FALSE)
	{
		$id = $this->sql_id();
		$sql_temp_select = '';

		if($fields == NULL)
		{
			$sql_temp_select = array('*');
		}
		else if (!is_array($fields) )
		{
			$sql_temp_select = $this->sql_clean_input($fields, $this->col_quote);
		}
		else
		{
			$i = 0;
			$count_fields = count($fields);
			while($i <= ($count_fields - 1))
			{
				$sql_temp_select = $sql_temp_select . $this->sql_clean_input($fields[$i], $this->col_quote);

				if($i < ($count_fields - 1))
				{
					$sql_temp_select = $sql_temp_select . ', ';
				}

				$i++;
			}
		}


		$sql = "SELECT $sql_temp_select, COUNT(*) as count FROM ".$this->sql_clean_input($this->prefix.$table, $this->table_quote).$this->sql_build_where_string($where_fields, $where_values, $id).' GROUP BY '.$this->sql_clean_input($group_col, $this->col_quote);
		if($ORDER_BY_COUNT)
		{
			$sql .= ' ORDER BY COUNT DESC';
		}
		else if(is_numeric($HAVING_COUNT)) //You can not use order by and having so they are exclusive
		{
			$sql .= ' HAVING COUNT(*) = '.$this->sql_clean_input_value($HAVING_COUNT, $id);
		}
		
		return $this->sql_select_exe($sql, NULL, $table, $id);
	}

	/**
	 * 
	 * Internal build method for building standard parts of SQL quieries
	 * @param array $fields
	 * @param string $table
	 * @param array $where_fields
	 * @param array $where_values
	 * @param array $order_fields
	 * @param string (ASC|DESC) $order_directions
	 */
	protected function sql_select_build($fields = NULL, $table, $where_fields = NULL, $where_values = NULL, $order_fields = NULL, $order_directions = NULL, $id)
	{
		$sql_temp_select = '';

		if($fields == NULL)
		{
			$sql_temp_select = '*';
		}
		else if (!is_array($fields) )
		{
			if($fields === '*')
			{
				$sql_temp_select = '*';
			}
			else 
			{
				$sql_temp_select = $this->sql_clean_input($fields, $this->col_quote);
			}
		}
		else
		{
			$i = 0;
			$count_fields = count($fields);
			while($i <= ($count_fields - 1))
			{
				if($fields[$i] === '*')
				{
					$sql_temp_select = $sql_temp_select . '*';
				}
				else 
				{
					$sql_temp_select = $sql_temp_select . $this->sql_clean_input($fields[$i], $this->col_quote);
				}
				
				if($i < ($count_fields - 1))
				{
					$sql_temp_select = $sql_temp_select . ', ';
				}

				$i++;
			}
		}


		$sql = "SELECT $sql_temp_select FROM ".$this->sql_clean_input($this->prefix.$table, $this->table_quote).$this->sql_build_where_string($where_fields, $where_values, $id);


		//Include Order By if required
		if($order_fields != NULL)
		{
			$i = 0;
			$count_order = count($order_fields);
			$sql_temp_order = '';

			while($i <= ($count_order - 1))
			{
				if($order_directions[$i] != 'DESC')
				{
					$order = 'ASC';
				}
				else
				{
					$order = 'DESC';
				}

				$sql_temp_order = $sql_temp_order . ' '.$this->sql_clean_input($order_fields[$i], $this->col_quote).' '. $order .' ';

				if($i < ($count_order - 1))
				{
					$sql_temp_order = $sql_temp_order . ' , ';
				}

				$i++;
			}
			$sql = $sql	. ' ORDER BY ' . $sql_temp_order;
		}

		return $sql;
	}

	/**
	 * 
	 * SQL Select the maximum value from a field
	 * @param string $table
	 * @param string $field
	 * @param array $where_fields
	 * @param array $where_values
	 */
	public function sql_max_field($table, $field, $where_fields = NULL, $where_values = NULL)
	{
		$id = $this->sql_id();
		$sql = $this->sql_max_field_build($table, $field, $where_fields, $where_values, $id);
		$result = $this->sql_select_exe($sql, $this->sql_values, TRUE, $table, $id);
		if ($result == NULL || !isset($result[0])) {
			return 0;
		}
		else {
			//FIXME: Dirty hack to get the first result as the key may change depending on the DB quote types.
			foreach($result[0] as $result){
				return $result;
			}
		}
	}

	/**
	 * 
	 * Internal method to build the SQL statment for SQL Select max from a field
	 * @param string $table
	 * @param string $field
	 * @param array $where_fields
	 * @param array $where_values
	 */
	protected function sql_max_field_build($table, $field, $where_fields = NULL, $where_values = NULL, $id)
	{
		return 'SELECT max( '.$this->sql_clean_input($field, $this->col_quote).' ) FROM '.$this->sql_clean_input($this->prefix.$table, $this->table_quote).$this->sql_build_where_string($where_fields, $where_values, $id);
	}

	/**
	 * 
	 * SQL Select the minimum from a field
	 * @param string $table
	 * @param string $field
	 * @param array $where_fields
	 * @param array $where_values
	 */
	public function sql_min_field($table, $field, $where_fields = NULL, $where_values = NULL)
	{
		$id = $this->sql_id();
		$sql = $this->sql_min_field_build($table, $field, $where_fields, $where_values, $id);
		$result = $this->sql_select_exe($sql, $this->sql_values, TRUE, $table, $id);
		return $result[0]['min( sort )'];
	}

	/**
	 * 
	 * Internal method to build the SQL statment for SQL Select min from a field
	 * @param string $table
	 * @param string $field
	 * @param array $where_fields
	 * @param array $where_values
	 */
	protected function sql_min_field_build($table, $field, $where_fields = NULL, $where_values = NULL, $id)
	{
		return 'SELECT min( '.$this->sql_clean_input_value($field, $id).' ) FROM '.$this->table_quote.$this->prefix. $this->sql_clean_input($table).$this->table_quote.$this->sql_build_where_string($where_fields, $where_values, $id);
	}

	/**
	 * 
	 * SQL Update timestamp standard filed where ID
	 * @param string $table
	 * @param int $id
	 * @param bool $destroy_cache
	 */
	public function sql_update_time($table, $id, $destroy_cache = false)
	{
		$sql = $this->sql_update_time_build($table, $id);

		//Run the SQL
		return $this->sql_update_exe($sql, null, $destroy_cache, $table);
	}

	/**
	 * 
	 * Update timestamp in table based on ID, optional id col, optional timestamp col
	 * @param string $table
	 * @param int $id
	 * @param string $id_col
	 * @param string $ts_field
	 */
	protected function sql_update_time_build($table, $id, $id_col = 'id', $ts_field = 'update_ts')
	{
		return 'UPDATE '. $this->sql_clean_input($this->prefix.$table, $this->table_quote) . ' SET '.$this->sql_clean_input($ts_field, $this->col_quote).' = ' . $this->sql_clean_input($this->time) . ' WHERE '.$this->sql_clean_input($id_col, $this->col_quote).' = ' . $this->sql_clean_input($id) . ' ';
	}

	/**
	 * 
	 * SQL toggle a int between 0 and 1
	 * @param int $id
	 * @param string $table
	 * @param string $field
	 * @param bool $destroy_cache
	 */
	public function sql_toggle_field_by_id($id, $table, $field, $destroy_cache = false)
	{
		$current_value = $this->sql_select_first(array($field), $table, array('id'), array($id));
		if($current_value[$field] == 0)
		{
			$new_value = 1;
		}
		else
		{
			$new_value = 0;
		}
		$this->sql_update_by_id($id, $table, array($field), array($new_value), $destroy_cache);
		return $new_value;
	}

	/**
	 * 
	 * SQL update row fields by ID coloum
	 * @param int $id
	 * @param string $table
	 * @param array $fields
	 * @param array $values
	 * @param bool $destroy_cache
	 */
	public function sql_update_by_id($id, $table, $fields = array(), $values = array(), $destroy_cache = false){
		return $this->sql_update_by_col($table, array('ID'), array($id), $fields, $values, $destroy_cache);
	}

	/**
	 * 
	 * SQL update row fields with values by coloum where values
	 * @param string $table
	 * @param array $where_fields
	 * @param array $where_values
	 * @param array $fields
	 * @param array $values
	 * @param bool $destroy_cache
	 */
	public function sql_update_by_col($table, $where_fields = array(), $where_values = array(), $fields = array(), $values = array(), $destroy_cache = false)
	{
		$id = $this->sql_id();
		$sql = $this->sql_update_by_col_build($table, $where_fields, $where_values, $fields, $values, $id);
		return $this->sql_update_exe($sql, NULL, $destroy_cache, $table, $id);
	}

	protected function sql_update_by_col_build($table, $where_fields = array(), $where_values = array(), $fields = array(), $values = array(), $id)
	{
		if(	(count($fields)) != (count($values))	)
		{
			return false;
		}

		$sql = 'UPDATE '.$this->sql_clean_input($this->prefix.$table, $this->table_quote).' SET ';

		$i = 0;
		$sql_temp_fields = '';
		$sql_temp_values = '';

		$count_fields = count($fields);
		while($i <= ($count_fields - 1))
		{
			$sql_temp_fields .= $this->sql_clean_input($fields[$i], $this->col_quote).' = '. $this->sql_clean_input_value($values[$i], $id) . ' ';
			if($i < ($count_fields -1))
			{
				//not the last one so add a ,
				 $sql_temp_fields .=', ';
			}
			++$i;
		}

		return $sql	. $sql_temp_fields . $this->sql_build_where_string($where_fields, $where_values, $id);
	}

	/**
	 * 
	 * SQL Update or insert a row in the database as required
	 * @param string $table
	 * @param array $where_fields
	 * @param array $where_values
	 * @param array $fields
	 * @param array $values
	 * @param bool $destroy_cache
	 * @param string $alternet_id_col
	 */
	public function sql_update_or_insert($table, $where_fields = array(), $where_values = array(), $fields = array(), $values = array(), $destroy_cache = false, $alternet_id_col = false)
	{
		/**
		 * This currently does not work in MySQL and so 2 transactions are required unfortunatly
		 */
		if (!isset($where_fields))
		{ 
			return false;
		}
		else if(isset($where_fields) && !is_array($where_fields))
		{
			$id_col = $where_fields; 
		}
		else
		{
			if(!isset($where_fields[0])){ return false; } // passed in an empty array so quickly fail
			$id_col = $where_fields[0];
		}
		
		if($alternet_id_col)
		{
			$id_col = $alternet_id_col;
		}

		$exsists = $this->sql_select_first($id_col, $table, $where_fields, $where_values, null, null, true);

		if($exsists != NULL)
		{
			return $this->sql_update_by_col($table, array($id_col), array($exsists[$id_col]), $fields, $values);
		}
		else
		{
			return $this->sql_insert($table, $fields, $values);
		}
	}

	/**
	 * 
	 * SQL insert statment
	 * Insert in to a table an array of field names and an array of values
	 * @param string $table
	 * @param array $fields
	 * @param array $values
	 */
	public function sql_insert($table, $fields, $values)
	{
		$id = $this->sql_id();
		$this->sql_update_exe(
					$this->sql_insert_build($table, $fields, $values, $id),
					$this->sql_values[$id], 
					true, 
					$table, 
					$id);
		return $this->db->lastInsertID($table,'id');
	}

	protected function sql_insert_build($table, $fields, $values, $id)
	{
		if(	(count($fields)) != (count($values))	)
		{
			return false;
		}

		$i = 0;
		$count_fields = count($fields);
		$sql_temp_fields = '';
		while($i <= ($count_fields - 1))
		{
			$sql_temp_fields = $sql_temp_fields . $this->sql_clean_input($fields[$i], $this->col_quote).' ';
			if($i < ($count_fields - 1)) { $sql_temp_fields .=', '; }
			$i++;
		}

		//Check if were inserting mutiple lines in the same statment
		//FIXME: This does not as of yet work
		if(isset($values[0]) && is_array($values[0])){
			foreach($values as $row){
 				$i = 0;
 				$count_rows = count($row);
 				$sql_temp_values = '';
 				while($i <= ($count_rows - 1))
 				{
 					$sql_temp_values .= $this->sql_insert_build_value(array($row[$i]), $id).' ';
 					if($i < ($count_rows - 1)) {
 						$sql_temp_values .=', ';
 					}
 					$i++;
 				}
			}
		} else {
			//Single row insert
			$sql_temp_values = $this->sql_insert_build_value($values, $id);
		}
		
		return 'INSERT INTO '.$this->sql_clean_input($this->prefix.$table, $this->table_quote).' ('.$sql_temp_fields.') VALUES '.$sql_temp_values.' ';
	}
	
	private function sql_insert_build_value($values, $id)
	{
		$i = 0;
		$count_values = count($values);
		$sql_temp_values = '(';
		while($i <= ($count_values - 1))
		{
			$sql_temp_values .= $this->sql_clean_input_value($values[$i], $id) . ' ';
			if($i < ($count_values - 1)) {
				$sql_temp_values .=', ';
			}
			$i++;
		}
		$sql_temp_values .= ')';
		return $sql_temp_values;
	}

	public function sql_delete_by_colum_equals($table, $col, $equals, $destroy_cache = false)
	{
		$id = $this->sql_id();
		$sql = $this->sql_delete_by_colum_equals_build($table, $col, $equals, $id);
		echo($sql); var_dump($this->sql_values[$id]);
		return $this->sql_update_exe($sql, NULL, true, $table, $id);
	}

	protected function sql_delete_by_colum_equals_build($table, $col, $equals, $id = null)
	{
		if($id == null){
			$id = $this->sql_id();
		}
		$table = $this->sql_clean_input($this->prefix.$table, $this->table_quote);
		return 'DELETE FROM '.$table.' WHERE '.$this->sql_clean_input($col, $this->col_quote).' = ' . $this->sql_clean_input_value($equals, $id);
	}

	public function sql_delete_where($table, $where_fields, $where_values)
	{
		$id = $this->sql_id();
		return $this->sql_update_exe($this->sql_delete_where_build($table, $where_fields, $where_values, $id), NULL, true, $table, $id);
	}

	protected function sql_delete_where_build($table, $where_fields, $where_values, $id)
	{
		$table = $this->sql_clean_input($this->prefix.$table, $this->table_quote);
		return 'DELETE FROM '.$table.' '.$this->sql_build_where_string($where_fields, $where_values, $id);
	}

	/**
	 * sql_copy_from_insert_into($fields, $from, $to, $extra_insert = NULL)
	 * $fields = array of field names
	 * $from = table name
	 * $to = table name
	 * $where_fields = array of where fields
	 * $where_values = array of values
	 * $extra_insert = keyed array of extra stuff to do on the insert
	 * 					array('field' => value)
	 */
	public function sql_select_into($fields, $from, $to, $where_fields, $where_values, $extra_insert = NULL)
	{
		$id = $this->sql_id();
		$sql = $this->sql_select_into_build($fields, $from, $to, $where_fields, $where_values, $extra_insert, $id);
		$result = $this->sql_exe($sql, NULL, true, $from, $id);
		return $result;
	}

	protected function sql_select_into_build($fields, $from, $to, $where_fields, $where_values, $extra_insert = NULL, $id)
	{
		$from = $this->prefix.$this->sql_clean_input_value($from, $id);
		$to = $this->prefix.$this->sql_clean_input_value($to, $id);

		if($fields == NULL)
		{
			$select_fields = array('*');
		}
		else if (!is_array($fields) )
		{
			$select_fields = $this->sql_clean_input_value($fields, $id);
		}
		else
		{
			$i = 0;
			$count_fields = count($fields);
			while($i <= ($count_fields - 1))
			{
				$sql_temp_select = $sql_temp_select . $this->sql_clean_input_value($fields[$i], $id);

				if($i < ($count_fields - 1))
				{
					$sql_temp_select = $sql_temp_select . ', ';
				}

				$i++;
			}
		}

		return "SELECT $sql_temp_select INTO ".$this->table_quote. $to .$this->table_quote.' FROM '.$this->table_quote.$from.$this->table_quote.' WHERE '.$this->sql_build_where_string($where_fields, $where_values, $id);
	}

	/**
	 * sql_move_into($fields, $from, $to, $extra_insert = NULL)
	 * $fields = array of field names
	 * $from = table name
	 * $to = table name
	 * $where = array of where clauses array('id' => 6)
	 * $extra_insert = keyed array of extra stuff to do on the insert
	 * 					array('field' => value)
	 *
	 * deletes the complete rows from the first table
	 */
	public function sql_move_into($fields, $from, $to, $where_fields, $where_values, $extra_insert = NULL)
	{
		$this->sql_select_into($fields, $values, $from, $to, $where, $extra_insert);
		$this->sql_delete_where($from, $fields, $values);
	}

	/**
	 * This is database type specific by default returns null
	 * @param $table
	 * @param $col
	 * @param $query
	 * @param $select
	 * @param $boolean
	 * @param $where_fields
	 * @param $where_values
	 * @param $from
	 * @param $to
	 * @param $order
	 * @param $score
	 * @return unknown_type
	 */
	public function sql_search($table, $col, $query, $select = NULL, $boolean = false, $where_fields = array(), $where_values = array(), $from = 0 , $to = 100, $order = 'DESC', $score = 10)
	{
		return null;
	}

	/**
	 * 
	 * SQL Count rows from an SQL select statment
	 * @param string $table
	 * @param array $where_fields
	 * @param array $where_values
	 * @param bool $FORCE_NO_CACHE
	 */
	public function sql_count_where($table, $where_fields, $where_values, $FORCE_NO_CACHE = FALSE)
	{
		$id = $this->sql_id();
		$sql = $this->sql_count_where_build($table, $where_fields, $where_values, $id);

		$result = $this->sql_select_exe($sql, NULL, $FORCE_NO_CACHE, $table, $id);
		return $result[0]['count(*)'];
	}

	protected function sql_count_where_build($table, $where_fields, $where_values, $id)
	{
		if (!is_array($where_fields)) {
			$where_fields = array($where_fields);
		}

		if (!is_array($where_values)) {
			$where_values = array($where_values);
		}

		return 'SELECT count(*) FROM '. $this->sql_clean_input($this->prefix.$table,$this->table_quote).$this->sql_build_where_string($where_fields, $where_values, $id);

		$result = $this->sql_select_exe($sql, NULL, $FORCE_NO_CACHE, $table, $id);
		return $result[0]['count(*)'];
	}

	protected function sql_build_select_string($fields)
	{
		$sql_temp_select = '';
		if($fields == NULL)
		{
			$sql_temp_select = '*';
		}
		else if (!is_array($fields) )
		{
			$sql_temp_select = $this->sql_clean_input($fields, $this->col_quote);
		}
		else
		{
			$i = 0;
			$count_fields = count($fields);
			while($i <= ($count_fields - 1))
			{
				$sql_temp_select = $sql_temp_select . $this->sql_clean_input($fields[$i], $this->col_quote);

				if($i < ($count_fields - 1))
				{
					$sql_temp_select = $sql_temp_select . ', ';
				}

				$i++;
			}
		}
		return $sql_temp_select;
	}

	protected function sql_build_where_string($where_fields, $where_values, $id)
	{
		if(isset($where_fields) || is_array($where_fields) && !is_null($where_fields)){
			$part = $this->sql_build_where_string_part($where_fields, $where_values, $id);
			if(strlen($part) > 0)
			{
				return ' WHERE ('.$part.')';
			}
		}
	}

	protected function sql_build_where_string_part($where_fields, $where_values, $id)
	{		
		//Include Where clause into sql if set
		if(!is_array($where_fields))
		{
			$where_fields = array($where_fields);
		}
		if(!is_array($where_values))
		{
			$where_values = array($where_values);
		}
		$count_where = count($where_values) - 1;
		$count_where_fields = count($where_fields) - 1;
		
		//SQL count starts at 1 and array positions start at 0 hence the >= 0 and not > 0 and the above reduction in count by 1
		if($where_fields[0] != NULL and $where_values[0] != NULL and $count_where >= 0  and $count_where_fields >= 0 and ($count_where == $count_where_fields))
		{
			$i = 0;
			$sql_temp_where = '';

			while($i <= ($count_where))
			{
				$skip = false;
				//Check if this is an or statment
				if(is_array($where_values[$i]))
				{
					$count_or = count($where_values[$i]) -1;	
					if($count_or > 0) //Skip if an empty array has been passed in
					{
						$j = 0;
						$sql_temp_where_or = '';
						while($j <= ($count_or))
						{
							$sql_temp_where_or .= $this->sql_clean_input($where_fields[$i], $this->col_quote).' = ';
							//Quote non numeric values
							$sql_temp_where_or .= $this->sql_clean_input_value($where_values[$i][$j], $id);
	
							$j++;
							if(isset($where_values[$i][$j]))
							{
								$sql_temp_where_or .= ' OR ';
							}
	
						}
	
						$sql_temp_where .= ' (' . $sql_temp_where_or . ') ';
					}
					else
					{
						unset($skip);
					}
				}
				else
				{
					$sql_temp_where .= '('.$this->sql_clean_input($where_fields[$i], $this->col_quote).' = ';

					//Quote non numeric values
					if(is_numeric($where_values[$i]))
					{
						$sql_temp_where .= $this->sql_clean_input_value($where_values[$i], $id).')';
					}
					else
					{
						$sql_temp_where .= $this->sql_clean_input_value($where_values[$i], $id) .')';
					}
				}

				$i++;
				if(isset($where_values[$i]) && isset($skip))
				{
					$sql_temp_where .= ' AND ';
				}
			}

			return $sql_temp_where;
		}
	}
	
	public function backupDatabase()
	{
		return $this->db->backupDatabase();
	}

	protected function sql_exe($sql, $values, $writeuser = false, $destroy_cache = false, $id = null)
	{
		$this->db->sql_connect();

		if($destroy_cache && isset($_SESSION['sql_cache']))
		{
			unset($_SESSION['sql_cache']);
		}

		$sql_start_time = microtime();

		//Logging info
		$this->site_sql_count ++;
		if(strlen($sql) > $this->db->max_sql_length)
		{
			$results = array('Error' => 'SQL Length greater than max supported');
		}
		else
		{
			$results = $this->db->sql_run($sql, $values, $writeuser);
			if(!is_null($id)){
				unset($this->sql_values[$id]);
			}
			$this->sql_prepared_count = 1;
		}

		if($this->show_error_sql && isset($results['error']))
		{
				$error .= 'From : '. $sql;
				$this->sql_error($results['error']);
		}
		else if(isset($results['error']))
		{
			$this->sql_error($results['error']);
			exit;
		}

		$this->sql_log($this->site_sql_count, $sql, $sql_start_time, $this->sql_cache_hit, $results);
		
		return $results['resultset'];
	}

	public function sql_error($message, $line = NULL, $file = NULL)
	{
		$fh = fopen($this->error_file, 'r');
		$theData = fread($fh, filesize($this->error_file));
		fclose($fh);

		header('Content-Type: text/html');
		header('HTTP/1.0 500 Internal Server Error (Database)', true, 500);
		
		$strip_from_message = array($this->prefix); //Expand with usernames, hosts etc
		
	    die(str_replace('***ERROR***', $message, str_replace($strip_from_message, '', $theData)));
	}

	protected function sql_update_exe($sql, $values = NULL, $destroy_cache = false, $namespace = NULL, $id)
	{
		if($this->external_cache && !is_null($namespace))
		{
			persistantcache::Singleton()->cycle_namespace($namespace);
		}
		return $this->sql_exe($sql, $this->sql_values[$id], true, $destroy_cache, $id);
	}

	protected function sql_select_exe($sql, $values = NULL, $no_cache = FALSE, $namespace = NULL, $id = NULL)
	{		
		//die('SELECT:'.__LINE__);
		$results = false;
		$values = null;
		if(isset($this->sql_values[$id]))
		{
			$values =& $this->sql_values[$id];
		}
			
		//Check cache if not explecitly set not to
		if(!$no_cache && !is_null($id))
		{
			$results = $this->sql_check_cache($sql, $values, $namespace);
		}

		//If cache fails or not checked do SQL query
		if($results === false )
		{
			$results = $this->db->sql_select_exe_run($this->sql_exe($sql, $values, null, null, $id));

			//Cache the result
			$this->sql_cache($sql, $values, $results, $namespace);
		}

		unset($values);
		$this->sql_prepared_count = 1;
		return $results;
	}
	
	private function build_sql_cache_string($sql, $values)
	{
		if($values != null)
		{
			if(isset($values) && is_array($values))
			{
				$sql .= '-'.sha1(implode('-',$values));
			}
		}
		
		return $sql;
	}

	private function sql_check_cache($sql, $values, $namespace = null)
	{
		if($this->sql_cache_enabled)
		{
			//Set cache hit to false only set true if actually a hit
			$this->sql_cache_hit = false;
			$result = false;
			
			$sql = $this->build_sql_cache_string($sql, $values);
			
			if($this->external_cache)
			{
				$result = persistantcache::Singleton()->get($sql, $namespace);
			}
			else
			{
				if(isset($_SESSION['sql_cache'][$sql]))
				{
					$result = $_SESSION['sql_cache'][$sql];
				}
			}

			if($result != false)
			{
				if(($result['time'] >= ($this->time - $this->sql_cache_max_time) ) || (($result['time'] >= ($this->time - $this->sql_cache_max_freshness - $this->sql_cache_max_time)) && !($this->db->sql_connect(true))))
				{
					//Result cached and current
					$this->sql_cache_hit = true;
					++$this->sql_cache_hits;
					$this->site_sql_count ++;
					$this->sql_log($this->site_sql_count, $sql, 0, true);

					if(!$result['result']) { $result['result'] = NULL; }
					return $result['result'];
				}
				else
				{	
					//SQL Query is old so delete it and re-check the DB
					if($this->external_cache)
					{
						$result = persistantcache::Singleton()->drop($sql, $namespace);
					}
					else
					{
						unset($_SESSION['sql_cache'][$sql]);
					}
					return false;
				}
			}
		}
		//Result not cached
		return false;
	}

	private function sql_cache($sql, $values, $result, $namespace = null)
	{
		$sql = $this->build_sql_cache_string($sql, $values);

		if($this->external_cache)
		{
			persistantcache::Singleton()->save($sql, array('time' => $this->time, 'result' => $result), $this->sql_cache_max_time, $namespace);
		}
		else
		{
			$_SESSION['sql_cache'][$sql]['time'] = $this->time;
			$_SESSION['sql_cache'][$sql]['result'] = $result;
		}
	}

	/*
	 * Special SQL cases
	 */
	
	public function maintenance()
	{
		return database_maintenance::Singleton();
	}

	private function show_tables()
	{
		return($this->sql_exe('SHOW TABLES') );
	}

	public function drop_database($database)
	{
		$this->db->dropDatabase($this->sql_clean_input($database, $this->database_quote));
	}
	
	public function create_database($database)
	{
		$this->db->createDatabase($this->sql_clean_input($database, $this->database_quote));
	}

	public function create_table($table, $database = NULL, $cols = array(), $types = array(), $length = array())
	{
		if($database == NULL)
		{
			$database = $this->dsn['database'];
		}

		if((count($cols) != count($types)) and (count($cols) != count($length)))
		{
			return false;
		}

		foreach($this->show_tables() as $table_name)
		{
			if ($table == $table_name)
			{
				//Table already created
				return true;
			}
		}

		$sql = 'CREATE TABLE "'.$this->sql_clean_input($table).'" (';

		$i = 0;
		foreach($cols as $col)
		{
			$sql .= $this->sql_clean_input($col) .' '.$this->sql_clean_input($types[$i]).'('.$this->sql_clean_input($length[$i]).'),';
			$i++;
		}
		$sql = rtrim($sql, ',') .')';

		$this->sql_exe($sql, NULL, true, true);
	}
	
	/**
	 * 
	 */
	public function direct_sql($sql)
	{
		return $this->sql_exe($this->sql_clean_input($sql), NULL);
	}
	
	/**
	 * 
	 * Run a query directly against the DB object
	 * @param string $sql
	 */
	public function direct_query($sql)
	{
		$this->db->sql_connect();
		return $this->db->query($sql);
	}
	
	/**
	 * 
	 */
	public function get_schema()
	{

	}
	
	public function user_support()
	{
		return $this->user_support;
	}
	
	public static function does_x_support_users($type){
		return false;
	}
	
	public function using_prepared_statments()
	{
		return $this->sql_use_prepared_statments;
	}
	
	public function prefix(){
		return $this->prefix;
	}
}