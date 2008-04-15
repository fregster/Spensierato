<?

class Database
{

	private static $instance;
	
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
	
	public function __construct()
	{
		$this->connect();
	}
	
	public function connect($dsn = NULL)
	{
		if($dsn == NULL)
		{
			if($_SESSION['site_login'])
			{
				$db_user = $GLOBALS['database_user_readwrite'];
			}
			else
			{
				$db_user = $GLOBALS['database_user_read'];
			}
			
			
			$dsn = array(
			    'phptype'  => $GLOBALS['database_driver'],
			    'username' => $db_user,
			    'password' => $GLOBALS['database_password'],
			    'hostspec' => $GLOBALS['database_host'],
			    'database' => $GLOBALS['database_name'],
			);
		}
		
		$mdb2 =& MDB2::singleton($dsn, $options);
		if (PEAR::isError($mdb2)) 
		{
		    die($mdb2->getMessage());
		}
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
		
		
		$sql = "SELECT $sql_temp_select FROM $table";
		
		
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
				
				$sql_temp_order = $sql_temp_order . ' ' . $this->sql_clean_input($order_fields[$i]) .' '. $order .' ';
				
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
			$sql = $sql	. ' LIMIT ' . $limit;
		}
		
		//Logging info
		$this->site_sql_count ++;
		
		//Check SQL
		//$resultset = $mdb2->query($sql);
		$results = $this->sql_select_exe($sql, NULL, $FORCE_NO_CACHE);
		
		return $results;
	
	
	}
	
	//SQL Update time field query
	public function sql_update_time($table, $id)
	{
		$sql_start_time = Common::micro_time();
		
		$sql = 'UPDATE '. $this->sql_clean_input($table) . ' SET update_ts = \'' . time() . '\' WHERE id = \'' . $this->sql_clean_input($id) . '\'';
		
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
		
		$sql = 'UPDATE '. $this->sql_clean_input($table) . ' SET ';
		
		$i = 0;
		while($i <= (count($fields) - 1))
		{
			$sql_temp_fields = $sql_temp_fields . $this->sql_clean_input($fields[$i]) .', ';
		}
		
		$i = 0;
		while($i <= (count($fields) - 1))
		{
			$sql_temp_values = $sql_temp_values . $this->sql_clean_input($values[$i]) .', ';
		}
		
		$sql = $sql	. $sql_temp_fields . ' VALUES ' . sql_temp_values;
		
		//Logging info
		$this->site_sql_count ++;
		
		//Run the SQL
		return $this->sql_update_exe($sql);
	}

	//$Values is for when I get around to implimenting MDB2 properly!
	private function sql_exe($sql, $values)
	{
		global $mdb2;
		$sql_start_time = Common::micro_time();
		
		$resultset = $mdb2->query($sql);
		
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
		return $this->sql_exe($sql, $values);
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
			
			while($row = $resultset->fetchRow(MDB2_FETCHMODE_ASSOC)) 
			{
				$results[] = $row;
			}
			
			//Cache the result
			$this->sql_cache($sql, $results);
		}
		
		return $results;
	}
	
	private function sql_check_cache($sql, $values)
	{
		//Set cache hit to false only set true if actually a hit
		$this->sql_cache_hit = false;
		
		if(isset($_SESSION['sql_cache'][$sql]))
		{
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
	
	private function sql_cache($sql, $result)
	{
		$_SESSION['sql_cache'][$sql]['time'] = time();
		$_SESSION['sql_cache'][$sql]['result'] = $result;
	}
}
?>