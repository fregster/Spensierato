<?php
class Database
{
	private $options = array('allow_external_calls' => true);
	private static $instance;

	private $prefix;
	private $sql_db_type;
	private $error_file = false;

	private $sql_cache_enabled = true;
	private $sql_cache = array();
	private $sql_cache_max_results = 50;
	private $sql_cache_max_time = 30;
	private $sql_cache_hit = false;
	private $sql_cache_hits = 0;

	private $memcached = false;

	private $sql_log_array = array();
	protected $site_sql_count = 0;
	private $sql_time = 0;

	private $use_external_log = true;
	private $show_error_sql = false;


	/**
	 * Privately cached vars
	 */
	private $time;
	private $microtime;

	public static function Singleton()
	{
	   if (!isset(self::$instance))
	   {
			$c = 'database_'.DATABASE_DRIVER;
			include(DATABASE_LAYER_PATH.'/server_types/'.DATABASE_DRIVER.'.inc');
	   		if(!class_exists($c))
	   		{
	   			die('Unable to create correct database layer type ('.$c.')');
	   		}

		   self::$instance = new $c;
	   }
	   return self::$instance;
	}

	/**
	 * Database construct
	 * If the database class has been called directly throught the singleton then connect to the default database
	 *
	 * A connection to a seperate DB can be started with $new_db = Database::Singleton($dsn);
	 * The call $result = $new_bd()->sql_select_first(......);
	 *
	 * @param unknown_type $dsn
	 */
	protected function __construct()
	{
		$this->time = time();
		$this->microtime = microtime();
		$this->prefix = DATABASE_TABLE_PREFIX;

		if(!$this->error_file)
		{
			$this->error_file = DATABASE_LAYER_PATH.'/dberror.html';
		}

	}

	public static function custom_db_object($dsn, $options = NULL, $mods = NULL)
	{
		//A DSN has been set so create the connection with the specified options
		return database_execution::singleton()->custom_db_object($dsn, $options, $mods);
	}

	/**
	 * Here for legacy purposes
	 * @return unknown_type
	 */
	public function force_write_user()
	{
		database_execution::singleton()->force_write_user();
	}

	public function destroy_write_user()
	{
		database_execution::singleton()->destroy_write_user();
	}

	public function __clone()
	{
	   trigger_error('Clone is not allowed.', E_USER_ERROR);
	}

	public function sql_cache_disable()
	{
		//Disable cacheing
		$this->sql_cache_enabled = false;
		//Clear the cache
		session_unregister('sql_cache');
	}

	public function sql_show_log()
	{
		return $this->sql_log_array;
	}

	public function sql_time()
	{
		return $this->sql_time;
	}

	public function sql_count()
	{
		$percent = 0;
		if($this->site_sql_count > 0 && $this->sql_cache_hits > 0)
		{
			$percent = 100 / ($this->site_sql_count + $this->sql_cache_hits) * $this->sql_cache_hits;
		}
		return array($this->site_sql_count, $percent, $this->sql_cache_hits);
	}

	public function use_memcached()
	{
		if($this->options['allow_external_calls'])
		{
			$this->memcached = true;
		}
	}

	private function sql_clean_input($var)
	{
		//If the input is just a number just return it
		if(is_numeric($var)) { return $var; }

		return addslashes($var);
	}

	//SQL Logging function
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
					if($sql_result != NULL) { $details['result'] = $sql_result; }
					array_push($this->sql_log_array, $details);
					$this->sql_time = $this->sql_time + $sql_exc_time;

					Debug::Singleton()->record('SQL', $details);
				}
			}
		}
	}

	//Proxy to standard sql select forces limit to 1
	public function sql_select_first($fields = NULL, $table, $where_fields = NULL, $where_values = NULL, $order_fields = NULL, $order_directions = NULL, $FORCE_NO_CACHE = FALSE, $limit = NULL)
	{
		$result = $this->sql_select($fields, $table, $where_fields, $where_values, $order_fields, $order_directions, $FORCE_NO_CACHE, 1);
		if(!isset($result[0])) { $result[0] = null; }
		return  $result[0];
	}

	// Standard Select function
	// sql_select(Fields(if Null *), Table, Where (array), Where values (array must match number of fields), Order fields (array), Prder direction (if not sec ASC)
	public function sql_select($fields = NULL, $table, $where_fields = NULL, $where_values = NULL, $order_fields = NULL, $order_directions = NULL, $FORCE_NO_CACHE = FALSE, $limit = NULL)
	{
		$sql = $this->sql_select_build($fields, $table, $where_fields, $where_values, $order_fields, $order_directions);
		return $this->sql_select_exe($sql, NULL, $FORCE_NO_CACHE);
	}

	protected function sql_select_build($fields = NULL, $table, $where_fields = NULL, $where_values = NULL, $order_fields = NULL, $order_directions = NULL)
	{
		$sql_temp_select = '';

		if($fields == NULL)
		{
			$sql_temp_select = array('*');
		}
		else if (!is_array($fields) )
		{
			$sql_temp_select = $this->sql_clean_input($fields);
		}
		else
		{
			$i = 0;
			$count_fields = count($fields);
			while($i <= ($count_fields - 1))
			{
				$sql_temp_select = $sql_temp_select . $this->sql_clean_input($fields[$i]);

				if($i < ($count_fields - 1))
				{
					$sql_temp_select = $sql_temp_select . ', ';
				}

				$i++;
			}
		}


		$sql = "SELECT $sql_temp_select FROM `".$this->prefix.$this->sql_clean_input($table).'`'.$this->sql_build_where_string($where_fields, $where_values);


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

				$sql_temp_order = $sql_temp_order . ' `' . $this->sql_clean_input($order_fields[$i]) .'` '. $order .' ';

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

	public function sql_max_field($table, $field, $where_fields = NULL, $where_values = NULL)
	{
		$sql = $this->sql_max_field_build($table, $field, $where_fields, $where_values);
		$result = $this->sql_select_exe($sql, NULL, TRUE);
		if ($result == NULL) {
			return 0;
		}
		else{
			return $result[0]['max( sort )'];
		}

	}

	protected function sql_max_field_build($table, $field, $where_fields = NULL, $where_values = NULL)
	{
		return 'SELECT max( '.$this->sql_clean_input($field).' ) FROM `'.$this->prefix. $this->sql_clean_input($table).'`'.$this->sql_build_where_string($where_fields, $where_values);;
	}

	public function sql_min_field($table, $field, $where_fields = NULL, $where_values = NULL)
	{
		$sql = $this->sql_min_field_build($table, $field, $where_fields, $where_values);
		$result = $this->sql_select_exe($sql, NULL, TRUE);
		return $result[0]['min( sort )'];
	}

	protected function sql_min_field_build($table, $field, $where_fields = NULL, $where_values = NULL)
	{
		return 'SELECT min( '.$this->sql_clean_input($field).' ) FROM `'.$this->prefix. $this->sql_clean_input($table).'`'.$this->sql_build_where_string($where_fields, $where_values);
	}

	//SQL Update time field query
	public function sql_update_time($table, $id, $destroy_cache = false)
	{
		$sql = $this->sql_update_time_build($table, $id);

		//Run the SQL
		return $this->sql_update_exe($sql, null, $destroy_cache);
	}

	protected function sql_update_time_build($table, $id)
	{
		return 'UPDATE '. $this->prefix. $this->sql_clean_input($table) . ' SET update_ts = \'' . $this->time . '\' WHERE id = \'' . $this->sql_clean_input($id) . '\'';
	}

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

	//SQL update fileds by id
	public function sql_update_by_id($id, $table, $fields = array(), $values = array(), $destroy_cache = false){
		return $this->sql_update_by_col($table, array('ID'), array($id), $fields, $values, $destroy_cache);
	}

	public function sql_update_by_col($table, $where_fields = array(), $where_values = array(), $fields = array(), $values = array(), $destroy_cache = false)
	{
		$sql = $this->sql_update_by_col_build($table, $where_fields, $where_values, $fields, $values);
		//Run the SQL
		return $this->sql_update_exe($sql, NULL, $destroy_cache);
	}

	protected function sql_update_by_col_build($table, $where_fields = array(), $where_values = array(), $fields = array(), $values = array())
	{
		if(	(count($fields)) != (count($values))	)
		{
			return false;
		}

		$sql = 'UPDATE `'. $this->prefix.$this->sql_clean_input($table) . '` SET ';

		$i = 0;
		$sql_temp_fields = '';
		$sql_temp_values = '';

		$count_fields = count($fields);
		while($i <= ($count_fields - 1))
		{
			$sql_temp_fields .= '`'.$this->sql_clean_input($fields[$i]).'` = \''.$this->sql_clean_input($values[$i]) .'\' ';
			if($i < ($count_fields -1))
			{
				//not the last one so add a ,
				 $sql_temp_fields .=', ';
			}
			++$i;
		}

		return $sql	. $sql_temp_fields . $this->sql_build_where_string($where_fields, $where_values);
	}

	public function sql_update_or_insert($table, $where_fields = array(), $where_values = array(), $fields = array(), $values = array(), $destroy_cache = false)
	{
		/**
		 * This currently does not work in MySQL and so 2 transactions are required unfortunatly
		 */

		$exsists = $this->sql_select_first(array($where_fields[0]), $table, $where_fields, $where_values);

		if($exsists != NULL)
		{
			return $this->sql_update_by_col($table, array($where_fields[0]), array($exsists[$where_fields[0]]), $fields, $values);
		}
		else
		{
			return $this->sql_insert($table, $fields, $values);
		}
	}

	public function sql_insert($table, $fields, $values)
	{
		$sql = $this->sql_insert_build($table, $fields, $values);
		$this->sql_exe($sql, NULL, true);
		return database_execution::Singleton()->lastInsertID($table,'id');
	}

	protected function sql_insert_build($table, $fields, $values)
	{
		$table = $this->prefix.$this->sql_clean_input($table);

		if(	(count($fields)) != (count($values))	)
		{
			return false;
		}

		$i = 0;
		$count_fields = count($fields);
		$sql_temp_fields = '';
		while($i <= ($count_fields - 1))
		{
			$sql_temp_fields = $sql_temp_fields . '`'.$this->sql_clean_input($fields[$i]) .'` ';
			if($i < ($count_fields - 1)) { $sql_temp_fields .=', '; }
			$i++;
		}

		$i = 0;
		$count_values = count($values);
		$sql_temp_values = '';
		while($i <= ($count_values - 1))
		{
			$sql_temp_values = $sql_temp_values . '"'.$this->sql_clean_input($values[$i]) .'" ';
			if($i < ($count_values - 1)) { $sql_temp_values .=', '; }
			$i++;
		}


		return 'INSERT INTO '. $table . ' ('.$sql_temp_fields.') VALUES ('.$sql_temp_values.') ';
	}

	public function sql_delete_by_colum_equals($table, $col, $equals, $destroy_cache = false)
	{
		$sql = $this->sql_delete_by_colum_equals_build($table, $col, $equals);
		return $this->sql_exe($sql, NULL, true);
	}

	protected function sql_delete_by_colum_equals_build($table, $col, $equals)
	{
		$table = $this->prefix.$this->sql_clean_input($table);
		return 'DELETE FROM '.$table.' WHERE `'.$this->sql_clean_input($col).'` = "'.$this->sql_clean_input($equals).'"';
	}

	public function sql_delete_where($table, $where_fields, $where_values)
	{
		$sql = $this->sql_delete_where_build($table, $where_fields, $where_values);
		return $this->sql_exe($sql, NULL, true);
	}

	protected function sql_delete_where_build($table, $where_fields, $where_values)
	{
		$table = $this->prefix.$this->sql_clean_input($table);
		return 'DELETE FROM '.$table.' '.$this->sql_build_where_string($where_fields, $where_values);
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
		$sql = $this->sql_select_into_build($fields, $from, $to, $where_fields, $where_values, $extra_insert);
		$result = $this->sql_exe($sql, NULL, true);
		return $result;
	}

	protected function sql_select_into_build($fields, $from, $to, $where_fields, $where_values, $extra_insert = NULL)
	{
		$from = $this->prefix.$this->sql_clean_input($from);
		$to = $this->prefix.$this->sql_clean_input($to);

		if($fields == NULL)
		{
			$select_fields = array('*');
		}
		else if (!is_array($fields) )
		{
			$select_fields = $this->sql_clean_input($fields);
		}
		else
		{
			$i = 0;
			$count_fields = count($fields);
			while($i <= ($count_fields - 1))
			{
				$sql_temp_select = $sql_temp_select . $this->sql_clean_input($fields[$i]);

				if($i < ($count_fields - 1))
				{
					$sql_temp_select = $sql_temp_select . ', ';
				}

				$i++;
			}
		}

		return "SELECT $sql_temp_select INTO `$to` FROM `$from` WHERE ".$this->sql_build_where_string($where_fields, $where_values);
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

	public function sql_count_where($table, $where_fields, $where_values, $FORCE_NO_CACHE = FALSE)
	{
		$sql = $this->sql_count_where_build($table, $where_fields, $where_values);

		$result = $this->sql_select_exe($sql, NULL, $FORCE_NO_CACHE);
		return $result[0]['count(*)'];
	}

	protected function sql_count_where_build($table, $where_fields, $where_values)
	{
		if (!is_array($where_fields)) {
			$where_fields = array($where_fields);
		}

		if (!is_array($where_values)) {
			$where_values = array($where_values);
		}

		return 'SELECT count(*) FROM `'.$this->prefix. $this->sql_clean_input($table).'`'.$this->sql_build_where_string($where_fields, $where_values);

		$result = $this->sql_select_exe($sql, NULL, $FORCE_NO_CACHE);
		return $result[0]['count(*)'];
	}

	private function sql_build_select_string($fields)
	{
		if($fields == NULL)
		{
			$sql_temp_select = '*';
		}
		else if (!is_array($fields) )
		{
			$sql_temp_select = $this->sql_clean_input($fields);
		}
		else
		{
			$i = 0;
			$count_fields = count($fields);
			while($i <= ($count_fields - 1))
			{
				$sql_temp_select = $sql_temp_select . $this->sql_clean_input($fields[$i]);

				if($i < ($count_fields - 1))
				{
					$sql_temp_select = $sql_temp_select . ', ';
				}

				$i++;
			}
		}
		return $sql_temp_select;
	}

	private function sql_build_where_string($where_fields, $where_values)
	{
		$part = $this->sql_build_where_string_part($where_fields, $where_values);
		if(strlen($part) > 0)
		{
			return ' WHERE ('.$part.')';
		}
	}

	private function sql_build_where_string_part($where_fields, $where_values)
	{
		//Include Where clause into sql if set
		$count_where = count($where_values) - 1;
		$count_where_fields = count($where_fields) - 1;

		if($where_fields != NULL and $where_values != NULL and ($count_where == $count_where_fields) )
		{
			$i = 0;
			$sql_temp_where = '';

			while($i <= ($count_where))
			{
				//Check if this is an or statment
				if(is_array($where_values[$i]))
				{
					$j = 0;
					$count_or = count($where_values[$i]) -1;
					$sql_temp_where_or = '';
					while($j <= ($count_or))
					{
						$sql_temp_where_or .= '`'.$this->sql_clean_input($where_fields[$i]) .'` = ';
						//Quote non numeric values
						if(is_numeric($where_values[$i][$j])) {	$sql_temp_where_or .= $where_values[$i][$j]; }else{ $sql_temp_where_or .= '"'. $this->sql_clean_input($where_values[$i][$j]) .'"'; }

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
					$sql_temp_where .= '(`' . $this->sql_clean_input($where_fields[$i]) .'` = ';

					//Quote non numeric values
					if(is_numeric($where_values[$i]))
					{
						$sql_temp_where .= $where_values[$i].')';
					}
					else
					{
						$sql_temp_where .= '"'. $this->sql_clean_input($where_values[$i]) .'")';
					}
				}

				$i++;
				if(isset($where_values[$i]))
				{
					$sql_temp_where .= ' AND ';
				}
			}

			return $sql_temp_where;
		}
	}

	//$Values is for when I get around to implimenting MDB2 properly!
	private function sql_exe($sql, $values, $writeuser = false, $destroy_cache = false)
	{
		database_execution::Singleton()->sql_connect();

		if($destroy_cache)
		{
			session_unregister('sql_cache');
		}

		$sql_start_time = microtime();

		//Logging info
		$this->site_sql_count ++;
		$results = database_execution::Singleton()->sql_run($sql, $values, $writeuser);

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

		$this->sql_log($this->site_sql_count, $sql, $sql_start_time, $this->sql_cache_hit, $results['resultset']);

		return $results['resultset'];
	}

	public function sql_error($message, $line = NULL, $file = NULL)
	{
		$fh = fopen($this->error_file, 'r');
		$theData = fread($fh, filesize($this->error_file));
		fclose($fh);

	    die(str_replace('***ERROR***', $message, $theData));
	}

	protected function sql_update_exe($sql, $values = NULL, $destroy_cache = false)
	{
		return $this->sql_exe($sql, $values, true, $destroy_cache);
	}

	protected function sql_select_exe($sql, $values = NULL, $no_cache = FALSE)
	{
		$results = false;
		//Check cache if not explecitly set not to
		if(!$no_cache)
		{
			$results = $this->sql_check_cache($sql, $values);
		}

		//If cache fails or not checked to SQL query
		if($results === false)
		{
			$results = database_execution::singleton()->sql_select_exe_run($this->sql_exe($sql, $values));

			//Cache the result
			$this->sql_cache($sql, $values, $results);
		}

		return $results;
	}

	private function sql_check_cache($sql, $values)
	{
		if($this->sql_cache_enabled)
		{
			//Set cache hit to false only set true if actually a hit
			$this->sql_cache_hit = false;
			$result = false;
			if($this->memcached)
			{
				$result = persistantcache::Singleton()->get($sql);
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
				if($values != NULL)
				{
					//This was a prepared statment return the correct cached result
					if(	(isset($result[$values])) and ($result[$values]['time'] >= ($this->time - 30)))
					{
						return $result[$values];
					}
					//SQL Query has been done before but with diffrent values
					return false;
				}

				if($result['time'] >= ($this->time - $this->sql_cache_max_time) )
				{
					//Result cached and current
					$this->sql_cache_hit = true;
					++$this->sql_cache_hits;
					$this->sql_log($this->site_sql_count, $sql, 0, true);

					if(!$result['result']) { $result['result'] = NULL; }
					return $result['result'];
				}
				else
				{
					//SQL Query is old so delete it and re-check the DB
					if($this->memcached)
					{
						$result = persistantcache::Singleton()->drop($sql);
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

	private function sql_cache($sql, $values, $result)
	{
		if($this->memcached)
		{
			persistantcache::Singleton()->save($sql, array('time' => $this->time, 'result' => $result), 30);
		}
		else
		{
			if($values == NULL)
			{
				$_SESSION['sql_cache'][$sql]['time'] = $this->time;
				$_SESSION['sql_cache'][$sql]['result'] = $result;
			}
			else
			{
				//This was a prepared statment so its results are cached slightly diffrently
				$_SESSION['sql_cache'][$sql][$values]['time'] = $this->time;
				$_SESSION['sql_cache'][$sql][$values] = $result;
			}
		}
	}

	/*
	 * Special SQL cases
	 */

	private function show_tables()
	{
		return($this->sql_exe('SHOW TABLES') );
	}

	public function create_database($database)
	{
		$this->sql_exe('CREATE DATABASE '.$this->sql_clean_input($database), NULL, true, true);
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

	public function direct_sql($sql)
	{
		$this->sql_exe($this->sql_clean_input($sql));
	}
}

?>