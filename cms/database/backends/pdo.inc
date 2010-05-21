<?php
class database_execution_pdo extends database_execution
{
	public function connect($dsn, $options = NULL, $mods = NULL)
	{
		try {
			$host = $dsn['hostspec'];
			if($dsn['phptype'] != 'sqlite')
			{
				$host = 'host='.$host;
			}
			else
			{
				if(!file_exists($host)){ $this->sql_error('Database offline'); }
			}
				if(isset($dsn['database']))
					$connection_String = $dsn['phptype'].':'.$host.';dbname='.$dsn['database'];
				else
					$connection_String = $dsn['phptype'].':'.$host;
		    	$this->db_backend = new PDO($connection_String, $dsn['username'], $dsn['password'], array(PDO::FETCH_ASSOC => PDO::ERRMODE_EXCEPTION));
		    }
			catch(PDOException $e)
		    {
				$this->sql_error($e->getMessage());
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
					$resultset = $this->read->query($sql);
				}
				else
				{
					//Do prepeared statment
					$stmt = $this->read->prepare($sql);
				}
			}
		}
		catch(PDOException $e)
	    {
	   		$this->sql_error($e->getMessage());
	    }

		$final = array('resultset' => $resultset);
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