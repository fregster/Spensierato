<?php
class database_execution_mdb2 extends database_execution
{
	protected $supports_prepared_statments = true;
	
	public function connect($dsn, $options = NULL, $mods = NULL)
	{
		//Load the MDB2 code
		//Doing this here only loads the code if a DB query is actually going to be done
		if(!@include 'MDB2.php')
		{
			$this->sql_error('Unable to load MDB2, ensure the class is available. Hint try installing the pear class');
			exit;
		}
		
		$host = $dsn['hostspec'];
		if($dsn['phptype'] != 'sqlite')
		{
			$host = 'host='.$host.'?mode=0660';
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
		$this->db_backend = @MDB2::connect($dsn, $options);
		if(!$this->db_backend)
		{
			$error = 'Unable to connect to the database ('.$php_errormsg.')';
			$this->sql_error($error);
		}
		ini_set('track_errors', $old_track);

		if (PEAR::isError($this->db_backend) or (MDB2::isError($this->db_backend)))
		{
			$this->sql_error($this->db_backend->getMessage());
		}

		if(isset($mods[0]))
		{
			foreach($mods as $mod)
			{
				$this->db_backend->loadModule($mod);
			}
		}

		$this->db_backend->setFetchMode(MDB2_FETCHMODE_ASSOC);
		return $this->db_backend;
	}

	public function sql_run($sql, $values, $writeuser)
	{
		
		$error = false;
		//Query the database Object
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
					}
				}
				else
				{
					$error = 'Failed update database, write user disabled<br />';
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
					$stmt = $this->read->prepare($sql);
				}
			}

			if(isset($stmt))
			{
				if (PEAR::isError($stmt) or (MDB2::isError($stmt)))
				{
					$error = 'Failed to prepare statment : ' . $stmt->getMessage().'<br />';
					if($this->show_error_sql)
					{
						$error .= 'From : '. $sql .'<br />With: ' .$values;
					}
					$this->sql_error($error);
				}
				
				$resultset = $stmt->execute($values);

			}
			
			if(PEAR::isError($resultset) || MDB2::isError($resultset))
			{
				$error = 'Failed to issue query, error message : ' . $resultset->getMessage().', '. $resultset->getDebugInfo().'<br />';
			}
				
			unset($sql);
			
			if($error != false){
				$this->sql_error($error);
			}
			
		 }
		catch(PEAR_Exception $e)
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
		if(is_object($resultset))
		{
			 return $resultset->fetchAll();
		}
		
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

	//since this method is used only at install time, we don't need to return objects,
	//just success or fail
	public function query($sql_str)
	{
		$result = $this->db_backend->query($sql_str);
		if (PEAR::isError($result)) {
			return FALSE;
		}
		else return TRUE;
	}

	public function lastInsertID($table,$col)
	{
		return $this->write->lastInsertId($table,$col);
	}

	public function createDatabase($database)
	{
		$this->db_backend->loadModule('Manager');
		return $this->db_backend->createDatabase($database);
	}

	public function dropDatabase($database)
	{
		$this->db_backend->loadModule('Manager');
		return $this->db_backend->dropDatabase($database);
	}
}