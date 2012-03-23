<?php
class database_execution_pdo extends database_execution
{
	protected $supports_prepared_statments = true;
	protected $supports_transactions = true;
	
	public function connect($dsn, $options = NULL, $mods = NULL, $soft_connect = false, $transaction = false)
	{
		try {
			$db_dsn_string = 'dbname=';
			$host = '';
			if(isset($dsn['hostspec'])){
				$host = $dsn['hostspec'];
			}
			
			switch ($dsn['phptype']){
				case 'mysqli':
				case 'mysql':
					$host = 'host='.$host;
					$dsn['phptype'] = 'mysql';
					break;
					
				case 'sqlsrv':
					$host = 'server='.$host.'';
					$db_dsn_string = 'Database=';
					break;
					
				case 'sqlite':
					unset($dsn['database']);
						if(@!defined('DATABASE_INSTALL_MODE') || DATABASE_INSTALL_MODE != true)
						{
							if(!file_exists($host) || !is_writable($host)){ $this->sql_error('Database offline '.$host, NULL, NULL, $soft_connect); }
						}
					break;
			}
			
			if(isset($dsn['database'])){
				$connection_String = $dsn['phptype'].':'.$host.';'.$db_dsn_string.$dsn['database'];
			} else {
				$connection_String = $dsn['phptype'].':'.$host;
			}
			
			$old_track = ini_set('track_errors', '1');
//			$db_backend = new PDO($connection_String, $dsn['username'], $dsn['password']);
			$db_backend = @new PDO($connection_String, $dsn['username'], $dsn['password']);
			
			if(!$db_backend)
			{
				$error = 'Unable to connect to the database '.$php_errormsg;
				$this->sql_error($error, NULL, NULL, $soft_connect);
			}
			
			if($transaction){ //We want to use proper transactions so we start one now and then commit on page completion if no-error happened
				$db_backend->setAttribute(PDO::ATTR_AUTOCOMMIT, FALSE);
				$db_backend->beginTransaction();
			}

			ini_set('track_errors', $old_track);
		}	    
		catch(PDOException $e)
	    {
			$this->sql_error($e->getMessage() . ': Are all the required PHP extentions loaded for your Database Type', __LINE__, __FILE__, $soft_connect);
	    }
		
		return $db_backend;
	}

	public function sql_run($sql, $values, $writeuser, $mode = PDO::FETCH_ASSOC, $soft_connect = false)
	{
		$error = false;
		try
		{
			if($writeuser == true)
			{
				if($this->write != NULL && $this->write != false) //Test the write user is set
				{
					if($values == NULL)
					{
						//Values have not been passed so do simple SQL exec
						$resultset = $this->write->query($sql);
					}
					else
					{
						//Do prepeared statment
						$stmt = null; //PDO Bug fix
						$stmt = $this->write->prepare($sql);
											
						if($stmt === false)
						{
							$error = 'Failed to prepare statment : ' . $stmt->getMessage().'<br />';
							if($this->show_error_sql)
							{
								$error .= 'From : '. $sql .'<br />With: ' .$values;
							}
							$this->sql_error($error, NULL, NULL, $soft_connect);
						}
						$stmt->setFetchMode($mode);
					}
				}
				else
				{
					$error = 'Failed update database, write user disabled<br />';
					$this->sql_error($error, NULL, NULL, $soft_connect);
				}
			}
			else
			{
				if($values == NULL)
				{
					//Values have not been passed so do simple SQL exec
					$resultset = array();
					$result = $this->read->query($sql, $mode);
					if($result){
						foreach($this->read->query($sql) as $row)
						{
							$resultset[] = $row;
						}
					}

					unset($result);
				}
				else
				{
					//Do prepeared statment
					$stmt = $this->read->prepare($sql);
					$stmt->setFetchMode($mode);
					if($stmt === false)
					{
						$error = 'Failed to prepare statment : ' . $stmt->getMessage().'<br />';
						if($this->show_error_sql)
						{
							$error .= 'From : '. $sql .'<br />With: ' .$values;
						}
						$this->sql_error($error, NULL, NULL, $soft_connect);
					}
				}
			}

			if(isset($stmt))
			{
				$result = $stmt->execute($values);

				if($result === false)
				{
					$db_error = $stmt->errorInfo();
					$error = 'Failed to issue query<br />'.$db_error[2].'<br/>'.$sql;
					unset($db_error);
				}
				else
				{
					$resultset = $stmt->fetchAll($mode);
					if($resultset == false){
						$resultset = $stmt->rowCount();
					}
				}
			}
			unset($sql);
		}
		catch(PDOException $e)
	    {
	   		$this->sql_error($e->getMessage(), NULL, NULL, $soft_connect);
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
		return $this->write->query($create_db_sql);
	}

	public function dropDatabase($database)
	{
		$drop_db_sql = 'drop database '.$database;
		return $this->write->query($drop_db_sql);
	}

	//since this method is used only at install time, we don't need to return objects,
	//just success or fail
	public function query($sql_str)
	{
		try{
			$this->write->query($sql_str);
			$result = TRUE;
		}
		catch(PDOException $e){
			$result = FALSE;
		}

		return $result;
	}
	
	public function commit()
	{
		if($this->supports_transactions() && property_exists($this, 'write') && $this->write != null && method_exists($this->write, 'commit'))
		{
			if($this->write->inTransaction()){
				try {
					$this->write->commit();
				} catch(PDOException $e) {
					$this->write->rollBack();
				}
			}
		}
	}
}