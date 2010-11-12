<?php
class database_execution_pdo extends database_execution
{
	protected $supports_prepared_statments = true;
	
	public function connect($dsn, $options = NULL, $mods = NULL)
	{
		try {
			$host = $dsn['hostspec'];
			if($dsn['phptype'] == 'mysqli'){ $dsn['phptype'] = 'mysql'; }
			if($dsn['phptype'] != 'sqlite')
			{
				$host = 'host='.$host;
			}
			else
			{
				unset($dsn['database']);
				if(@!defined('DATABASE_INSTALL_MODE') || DATABASE_INSTALL_MODE != true)
				{
					if(!file_exists($host) || !is_writable($host)){ $this->sql_error('Database offline '.$host); }
				}
			}
			if(isset($dsn['database'])){
				$connection_String = $dsn['phptype'].':'.$host.';dbname='.$dsn['database'];
			} else {
				$connection_String = $dsn['phptype'].':'.$host;
			}
			
			$old_track = ini_set('track_errors', '1');
			$this->db_backend = @new PDO($connection_String, $dsn['username'], $dsn['password'], array(PDO::FETCH_ASSOC => PDO::ERRMODE_EXCEPTION));	
			
			if(!$this->db_backend)
			{
				$error = 'Unable to connect to the database '.$php_errormsg;
				$this->sql_error($error);
			}
			ini_set('track_errors', $old_track);
		}	    
		catch(PDOException $e)
	    {
			$this->sql_error($e->getMessage() . ': Are all the required PHP extentions loaded for your Database Type', __LINE__, __FILE__);
	    }
		
		return $this->db_backend;
	}

	public function sql_run($sql, $values, $writeuser)
	{
		$error = false;
		try
		{
			if($writeuser == true)
			{
				if($this->write != NULL) //Test the write user is set
				{
					if($values == NULL)
					{
						//Values have not been passed so do simple SQL exec
						$resultset = $this->write->query($sql);
					}
					else
					{
						//Do prepeared statment
						$stmt = $this->write->prepare($sql);
						if($stmt === false)
						{
							$error = 'Failed to prepare statment : ' . $stmt->getMessage().'<br />';
							if($this->show_error_sql)
							{
								$error .= 'From : '. $sql .'<br />With: ' .$values;
							}
							$this->sql_error($error);
						}
					}
				}
				else
				{
					$error = 'Failed update database, write user disabled<br />';
					$this->sql_error($error);
				}
			}
			else
			{
				if($values == NULL)
				{
					//Values have not been passed so do simple SQL exec
					$resultset = array();
					foreach($this->read->query($sql) as $row)
					{
						$resultset[] = $row;
					}
				}
				else
				{
					//Do prepeared statment
					$stmt = $this->read->prepare($sql);
					if($stmt === false)
					{
						$error = 'Failed to prepare statment : ' . $stmt->getMessage().'<br />';
						if($this->show_error_sql)
						{
							$error .= 'From : '. $sql .'<br />With: ' .$values;
						}
						$this->sql_error($error);
					}
				}
			}
			
			unset($sql);
			if(isset($stmt))
			{
				$result = $stmt->execute($values);

				if($result === false)
				{
					$error = 'Failed to issue query<br />';
				}
				else
				{
					$resultset = $stmt->fetchAll();	
				}
			}
			
		}
		catch(PDOException $e)
	    {
	   		$this->sql_error($e->getMessage());
	   		exit;
	    }
		
	    $final = array();
	    if(isset($resultset)) { $final['resultset'] = $resultset; }
	    if($error) { $final['error'] = $error; }

		return $final;
	}

	public function sql_select_exe_run($resultset)
	{
		$results = array();
		if(is_array($resultset))
		{
		    foreach ($resultset as $row)
			{
		      		$results[] = $row;
		    }
		}

		return $results;
	}

	public function lastInsertID($table,$col)
	{
		return $this->write->lastInsertId();
	}

	public function createDatabase($database)
	{
		$create_db_sql = 'create database '.$database;
		$this->db_backend->query($create_db_sql);
	}

	public function dropDatabase($database)
	{
		$drop_db_sql = 'drop database '.$database;
		$this->db_backend->query($drop_db_sql);
	}

	//since this method is used only at install time, we don't need to return objects,
	//just success or fail
	public function query($sql_str)
	{
		try{
			$this->db_backend->query($sql_str);
			$result = TRUE;
		}
		catch(PDOException $e){
			$result = FALSE;
		}

		return $result;
	}
}