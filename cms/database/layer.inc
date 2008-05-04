<?

class Database
{

	private static $instance;
	private $read;
	private $write = NULL;
	
	private $prefix;
	private $sql_db_type;
	private $dsn;
	
	private $sql_cache = array();
	private $sql_cache_max_results = 20;
	private $sql_cache_max_time = 30;
	private $sql_cache_hit = false;
	private $sql_cache_hits = 0;
	
	private $sql_log_array = array();
	private $site_sql_count = 0;
	private $sql_time = 0;

	public static function Singleton()
	{
	   if (!isset(self::$instance)) 
	   {
		   $c = __CLASS__;
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
	public function __construct()
	{
		$this->prefix = $GLOBALS['table_prefix'];

		$dsn = array(
		    'phptype'  => $GLOBALS['database_driver'],
		    'username' => $GLOBALS['database_user_read'],
		    'password' => $GLOBALS['database_password'],
		    'hostspec' => $GLOBALS['database_host'],
		    'database' => $GLOBALS['database_name'],
		);
	
		$this->read = $this->connect($dsn);
	
		//If logged in create db write user this should help prevent against SQL injection
		//As only logged in can use the db writeable user
//		if(user::Singleton()->is_logged_in())
		if($_SESSION['userid'] != 0)
		{
			//Create write user
			$dsn = array(
			    'phptype'  => $GLOBALS['database_driver'],
			    'username' => $GLOBALS['database_user_readwrite'],
			    'password' => $GLOBALS['database_password'],
			    'hostspec' => $GLOBALS['database_host'],
			    'database' => $GLOBALS['database_name'],
			);
			$this->write = $this->connect($dsn);
		}

		$this->dsn = $dsn;
	}
	
	public function custom_db_object($dsn)
	{
		$this->dsn = $dsn;
		
		//A DSN has been set so create the connection with the specified options
		//Create the read user
		return $this->connect($dsn);
	}
	
	private function connect($dsn)
	{	
		$mdb2 =& MDB2::singleton($dsn, $options);
		if (PEAR::isError($mdb2)) 
		{
		    die($mdb2->getMessage());
		}
		
		return $mdb2;
	}
	
	public function __clone()
	{
	   trigger_error('Clone is not allowed.', E_USER_ERROR);
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
		$percent = 100 / $this->site_sql_count * $this->sql_cache_hits;
		return array($this->site_sql_count, $percent, $this->sql_cache_hits);
	}
	
	
	private function sql_clean_input($var)
	{
		$sql_replace = array("'", '"');
		$clean = trim(str_replace($sql_replace, '', (stripslashes($var))));
		
		return($clean);
	}

	//SQL Logging function
	private function sql_log($sql_count, $sql_query, $sql_start_time, $sql_cache_hit)
	{
		if(Settings::Singleton()->get_setting('debug') > 0)
		{
			if($sql_start_time != 0)
			{
				$sql_exc_time = Common::micro_time() - $sql_start_time;
			}
			else
			{
				$sql_exc_time = 0;
			}
			
			if ( ($sql_cache_hit == false) or (Settings::Singleton()->get_setting('debug') > 1)	)
			{
				$details = array('count' => $sql_count, 'query' => $sql_query, 'time' => $sql_exc_time, 'cached' => $sql_cache_hit);
				array_push($this->sql_log_array, $details);
				$this->sql_time = $this->sql_time + $sql_exc_time;
				Debug::Singleton()->record('SQL', $details);
			}
		}
	}
	
	//Proxy to standard sql select forces limit to 1
	public function sql_select_first($fields = NULL, $table, $where_fields = NULL, $where_values = NULL, $order_fields = NULL, $order_directions = NULL, $FORCE_NO_CACHE = FALSE, $limit = NULL)
	{
		$result = $this->sql_select($fields, $table, $where_fields, $where_values, $order_fields, $order_directions, $FORCE_NO_CACHE, 1);
		return  $result[0];
	}
	
	// Standard Select function
	// sql_select(Fields(if Null *), Table, Where (array), Where values (array must match number of fields), Order fields (array), Prder direction (if not sec ASC)
	public function sql_select($fields = NULL, $table, $where_fields = NULL, $where_values = NULL, $order_fields = NULL, $order_directions = NULL, $FORCE_NO_CACHE = FALSE, $limit = NULL) 
	{	
		if($fields == NULL)
		{
			$select_fields = array('*');
		}
		else
		{
			$i = 0;
			while($i <= (count($fields) - 1))
			{
				$sql_temp_select = $sql_temp_select . $this->sql_clean_input($fields[$i]);
				
				if($i < (count($fields) - 1))
				{
					$sql_temp_select = $sql_temp_select . ', ';
				}
				
				$i++;
			}
		}
		
		
		$sql = "SELECT $sql_temp_select FROM `".$this->prefix.$this->sql_clean_input($table).'`';
		
		
		//Include Where clause into sql if set
		if($where_fields != NULL and $where_values != NULL and (count($where_values) == count($where_fields)) )
		{
			$i = 0;
			while($i <= (count($where_values) - 1))
			{
				
				//Check if this is an or statment
				if(is_array($where_values[$i]))
				{
					$or_values = array();
					foreach($where_values[$i] as $or)
					{
						array_push($or_values, $or);
					}
					
					$j = 0;
					while($j <= (count($or_values) - 1))
					{
						$sql_temp_where_or = $sql_temp_where_or . $this->sql_clean_input($where_fields[$i]) .' = "'. $this->sql_clean_input($or_values[$j]) .'"';
						//$sql_temp_where_or = $sql_temp_where_or . $this->sql_clean_input($where_fields[$i]) .' = ?';
						
						if($j < (count($or_values) - 1))
						{
							$sql_temp_where_or = $sql_temp_where_or . ' OR ';
						}
						$j++;
					}
					
					$sql_temp_where = ' (' . $sql_temp_where_or . ') ';
				}
				else
				{
					$sql_temp_where = $sql_temp_where . '(' . $this->sql_clean_input($where_fields[$i]) .' = "'. $this->sql_clean_input($where_values[$i]) .'")';
					//$sql_temp_where = $sql_temp_where . '(' . $this->sql_clean_input($where_fields[$i]) .' = ?';
				}
				
				
				if($i < (count($where_values) - 1))
				{
					$sql_temp_where = $sql_temp_where . ' AND ';
				}
				
				$i++;
			}
			$sql = $sql	. ' WHERE ' . $sql_temp_where;
		}
		
		
		//Include Order By if required
		if($order_fields != NULL)
		{
			$i = 0;
			while($i <= (count($order_fields) - 1))
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
				
				if($i < (count($order_fields) - 1))
				{
					$sql_temp_order = $sql_temp_order . ' , ';
				}
				
				$i++;
			}
			$sql = $sql	. ' ORDER BY ' . $sql_temp_order;
		}
		
		if( ($limit != NULL) and (is_numeric($limit)) )
		{
			//For databases that suport the LIMIT command
			if($this->dsn['phptype'] == 'mysqli')
			{
				$sql = $sql	. ' LIMIT ' . $limit;
			}
			
			//For MS SQL Server
			if($this->dsn['phptype'] == 'mssql')
			{
					$sql = str_replace('SELECT ', 'SELECT TOP '.$limit.' ', $sql);
			}
		}
		
		//Logging info
		$this->site_sql_count ++;
		
		//Check SQL
		//$resultset = $mdb2->query($sql);
		$results = $this->sql_select_exe($sql, NULL, $FORCE_NO_CACHE);
		
		return $results;
	
	
	}
	
	public function sql_max_field($table, $field, $where_fields = NULL, $where_values = NULL)
	{
		$sql = 'SELECT max( '.$this->sql_clean_input($field).' ) FROM `'.$this->prefix. $this->sql_clean_input($table).'`';
		
		//Include Where clause into sql if set
		if($where_fields != NULL and $where_values != NULL and (count($where_values) == count($where_fields)) )
		{
			$i = 0;
			while($i <= (count($where_values) - 1))
			{
				
				//Check if this is an or statment
				if(is_array($where_values[$i]))
				{
					$or_values = array();
					foreach($where_values[$i] as $or)
					{
						array_push($or_values, $or);
					}
					
					$j = 0;
					while($j <= (count($or_values) - 1))
					{
						$sql_temp_where_or = $sql_temp_where_or . $this->sql_clean_input($where_fields[$i]) .' = "'. $this->sql_clean_input($or_values[$j]) .'"';
						//$sql_temp_where_or = $sql_temp_where_or . $this->sql_clean_input($where_fields[$i]) .' = ?';
						
						if($j < (count($or_values) - 1))
						{
							$sql_temp_where_or = $sql_temp_where_or . ' OR ';
						}
						$j++;
					}
					
					$sql_temp_where = ' (' . $sql_temp_where_or . ') ';
				}
				else
				{
					$sql_temp_where = $sql_temp_where . '(' . $this->sql_clean_input($where_fields[$i]) .' = "'. $this->sql_clean_input($where_values[$i]) .'")';
					//$sql_temp_where = $sql_temp_where . '(' . $this->sql_clean_input($where_fields[$i]) .' = ?';
				}
				
				
				if($i < (count($where_values) - 1))
				{
					$sql_temp_where = $sql_temp_where . ' AND ';
				}
				
				$i++;
			}
			$sql = $sql	. ' WHERE ' . $sql_temp_where;
		}
		
		$result = $this->sql_select_exe($sql, NULL, TRUE);
		return $result[0]['max( sort )'];
	}
	
	//SQL Update time field query
	public function sql_update_time($table, $id)
	{
		$sql_start_time = Common::micro_time();
		
		$sql = 'UPDATE '. $this->prefix. $this->sql_clean_input($table) . ' SET update_ts = \'' . time() . '\' WHERE id = \'' . $this->sql_clean_input($id) . '\'';
		
		//Logging info
		$this->site_sql_count ++;
		
		//Run the SQL
		$this->sql_update_exe($sql);
	
		return true;
	}
	
	//SQL update fileds by id
	public function sql_update_by_id($id, $table, $fields = array(), $values = array())
	{
		if(	(count($fields)) != (count($values))	)
		{
			return false;
		}
		
		$sql = 'UPDATE `'. $this->sql_clean_input($table) . '` SET ';
		
		$i = 0;
		$sql_temp_fields = '';
		$sql_temp_values = '';
		
		while($i <= (count($fields) - 1))
		{
			$sql_temp_fields .= '`'.$this->sql_clean_input($fields[$i]).'` = \''.$this->sql_clean_input($values[$i]) .'\' ';
			if($i < (count($fields) -1))
			{
				//not the last one so add a ,
				 $sql_temp_fields .=', ';
			}
			++$i;
		}
		
		$sql = $sql	. $sql_temp_fields . ' WHERE ID = ' . $this->sql_clean_input($id);
		
		//Logging info
		$this->site_sql_count ++;
		
		//Run the SQL
		return $this->sql_update_exe($sql);
	}
	
	public function sql_update_or_insert($table, $where_fields = array(), $where_values = array(), $fields = array(), $values = array())
	{
		$table = $this->prefix.$this->sql_clean_input($table);
		if(	(count($where_fields)) != (count($where_values))	)
		{
			return false;
		}
		
		if(	(count($fields)) != (count($values))	)
		{
			return false;
		}
		
		$sql = 'IF EXISTS (SELECT * FROM '. $table . ' AS t1 ';
		
		
		$i = 0;
		while($i <= (count($where_values) - 1))
		{
			$sql_temp_where = $sql_temp_where . '(`' . $this->sql_clean_input($where_fields[$i]) .'` = "'. $this->sql_clean_input($where_values[$i]) .'")';
			
			if($i < (count($where_values) - 1))
			{
				$sql_temp_where = $sql_temp_where . ' AND ';
			}
				
			$i++;
		}
		
		$where = ' WHERE (' . $sql_temp_where .') ';
		
		$sql .= $where .' begin ';
		
		//Simple update query
		
		$sql .= 'UPDATE '. $this->sql_clean_input($table) . ' SET ';
		
		$i = 0;
		while($i <= (count($fields) - 1))
		{
			$sql .= ' `'.$this->sql_clean_input($fields[$i]) .'` = "'.$this->sql_clean_input($values[$i]) .'" ';
			if($i < (count($fields) - 1)) { $sql .=', '; }
			$i++;
		}
		
		
		$i = 0;
		while($i <= (count($fields) - 1))
		{
			$sql_temp_fields = $sql_temp_fields . '`'.$this->sql_clean_input($fields[$i]) .'` ';
			if($i < (count($fields) - 1)) { $sql_temp_fields .=', '; }
			$i++;
		}
		
		$i = 0;
		while($i <= (count($values) - 1))
		{
			$sql_temp_values = $sql_temp_values . '"'.$this->sql_clean_input($values[$i]) .'" ';
			if($i < (count($values) - 1)) { $sql_temp_values .=', '; }
			$i++;
		}
				
		$sql .= ' end else begin ';
		
		//Simple insert query
		
		$sql .= 'INSERT INTO '. $table . ' ('.$sql_temp_fields.') VALUES ('.$sql_temp_values.') ';
		
		$sql .= ' end ';
		
		die($sql);
		//Goal = 
//		IF (EXISTS (SELECT * FROM AA_TestTable AS t1
//  WHERE t1.ord_num = 'FFF'))
//begin
//  UPDATE AA_TestTable
//  SET ord_qty = 999
//  WHERE ord_num = 'FFF'
//end
//else
//begin
//  INSERT INTO AA_TestTable (ord_num, top_assy, ord_qty)
//  VALUES('GGG', 'XYZ', 567)
//end 
	}

	public function sql_insert($table, $fields, $values)
	{
		$table = $this->prefix.$this->sql_clean_input($table);
		
		if(	(count($fields)) != (count($values))	)
		{
			return false;
		}
		
		$i = 0;
		while($i <= (count($fields) - 1))
		{
			$sql_temp_fields = $sql_temp_fields . '`'.$this->sql_clean_input($fields[$i]) .'` ';
			if($i < (count($fields) - 1)) { $sql_temp_fields .=', '; }
			$i++;
		}
		
		$i = 0;
		while($i <= (count($values) - 1))
		{
			$sql_temp_values = $sql_temp_values . '"'.$this->sql_clean_input($values[$i]) .'" ';
			if($i < (count($values) - 1)) { $sql_temp_values .=', '; }
			$i++;
		}
		
		$sql = 'INSERT INTO '. $table . ' ('.$sql_temp_fields.') VALUES ('.$sql_temp_values.') ';
		
		$this->sql_exe($sql, NULL, true);
		return $this->write->lastInsertID($table,'id');
	}
	
	//$Values is for when I get around to implimenting MDB2 properly!
	private function sql_exe($sql, $values, $writeuser = false)
	{
		$sql_start_time = Common::micro_time();
		
		//Query the database Object
		//if(	($writeuser == true) and ($this->write != NULL))
		if($writeuser == true)
		{
			if($values == NULL)
			{
				//Values have not been passed so do simple SQL exec
				$resultset = $this->write->query($sql);
			}
			else
			{
				//Do prepeared statment
				$stmt = $this->write->prepare($sql, TRUE, MDB2_PREPARE_RESULT);
			}
		}
		else
		{
			if($values == NULL)
			{
				//Values have not been passed so do simple SQL exec
				$resultset = $this->read->query($sql);
			}
			else
			{
				//Do prepeared statment
				$stmt = $this->read->prepare($sql, TRUE, MDB2_PREPARE_RESULT);
			}
		}
		
		if(isset($stmt))
		{
			if (PEAR::isError($stmt)) 
			{
				$error = 'Failed to prepare statment : ' . $stmt->getMessage().'<br />From :'. $sql .'<br />With: ' .$values;
				die($error);
			}
			$resultset = $stmt->execute($values);
		}
		
		if(PEAR::isError($resultset)) 
		{
			$error = 'Failed to issue query, error message : ' . $resultset->getMessage().'<br />From :'. $sql;
			die($error);
		}
		
		$this->sql_log($this->site_sql_count, $sql, $sql_start_time, $this->sql_cache_hit);
		
		return $resultset;
	}
	
	private function sql_update_exe($sql, $values = NULL)
	{
		return $this->sql_exe($sql, $values, true);
	}
	
	private function sql_select_exe($sql, $values = NULL, $no_cache = FALSE)
	{
		//Check cache if not explecitly set not to
		if(!$no_cache)
		{
			$results = $this->sql_check_cache($sql, $values);
		}
		
		//If cache fails or not checked to SQL query
		if(!$results)
		{
			$resultset = $this->sql_exe($sql, $values);
//		print_r($resultset);
//		die();
			
			while($row = $resultset->fetchRow(MDB2_FETCHMODE_ASSOC)) 
			{
				$results[] = $row;
			}
			
			//Cache the result
			$this->sql_cache($sql, $values, $results);
		}
		
		return $results;
	}
	
	private function sql_check_cache($sql, $values)
	{
		//Set cache hit to false only set true if actually a hit
		$this->sql_cache_hit = false;
		
		if(isset($_SESSION['sql_cache'][$sql]))
		{
			if($values != NULL)
			{
				//This was a prepared statment return the correct cached result
				if(	(isset($_SESSION['sql_cache'][$sql][$values])) and ($_SESSION['sql_cache'][$sql][$values]['time'] >= (time() - 30)))
				{
					return $_SESSION['sql_cache'][$sql][$values];
				}
				//SQL Query has been done before but with diffrent values
				return false;
			}
			
			if($_SESSION['sql_cache'][$sql]['time'] >= (time() - 30) )
			{
				//Result cached and current
				$this->sql_cache_hit = true;
				$this->sql_cache_hits++;
				$this->sql_log($this->site_sql_count, $sql, 0, true);
				
				return $_SESSION['sql_cache'][$sql]['result'];
			}
			else
			{
				//SQL Query is old so delete it and re-check the DB
				unset($_SESSION['sql_cache'][$sql]);
				return false;
			}
		}
		else
		{
			//Result not caches
			return false;
		}
	}
	
	private function sql_cache($sql, $values, $result)
	{
		if($values == NULL)
		{
			$_SESSION['sql_cache'][$sql]['time'] = time();
			$_SESSION['sql_cache'][$sql]['result'] = $result;
		}
		else
		{
			//This was a prepared statment so its results are cached slightly diffrently
			$_SESSION['sql_cache'][$sql][$values]['time'] = time();
			$_SESSION['sql_cache'][$sql][$values] = $result;
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
		$this->sql_exe('CREATE DATABASE '.$this->sql_clean_input($database), NULL, true);
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
		$this->sql_exe($sql);
	}
	
	public function direct_sql($sql)
	{
		$this->sql_exe($this->sql_clean_input($sql));
	}
}
?>