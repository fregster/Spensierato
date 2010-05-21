<?php
class database_execution_mdb2 extends database_execution
{
	public function connect($dsn, $options = NULL, $mods = NULL)
	{
		//Load the MDB2 code
		//Doing this here only loads the code if a DB query is actually going to be done
		require_once 'MDB2.php';
		
		if($dsn['phptype'] == 'sqlite' && !file_exists($dsn['database']))
		{
			$this->sql_error('Database offline');
		}
		
		$this->db_backend =& MDB2::singleton($dsn, $options);

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

		return $this->db_backend;
	}

	public function sql_run($sql, $values, $writeuser)
	{
		
		$error = false;
		//Query the database Object

//		$db &= $this->read;
//		if($writeuser == true)
//		{
//			$db &= $this->write;
//		}
//
//		if($values == NULL)
//		{
//			//Values have not been passed so do simple SQL exec
//			$resultset = $this->write->query($sql);
//		}
//		else
//		{
//			//Do prepeared statment
//			$stmt = $this->write->prepare($sql, TRUE, MDB2_PREPARE_RESULT);
//		}
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
						$stmt = $this->write->prepare($sql, TRUE, MDB2_PREPARE_RESULT);
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
					$stmt = $this->read->prepare($sql, TRUE, MDB2_PREPARE_RESULT);
				}
			}

			if(isset($stmt))
			{
				if (PEAR::isError($stmt))
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

			if (PEAR::isError($resultset))
			{
				$error = 'Failed to issue query, error message : ' . $resultset->getMessage().', '. $resultset->getDebugInfo().'<br />';
			}
		 }
		catch(PEAR_Exception $e)
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

		while($row = $resultset->fetchRow(MDB2_FETCHMODE_ASSOC))
		{
			$results[] = $row;
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
		$this->db_backend->createDatabase($database);
	}

	public function dropDatabase($database)
	{
		$this->db_backend->dropDatabase($database);
	}
}