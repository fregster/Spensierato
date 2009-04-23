<?php
class database_execution_pdo extends database_execution
{
	public function connect($dsn, $options = NULL, $mods = NULL)
	{
		try {
				$connection_String = 'mysql:host='.$dsn['hostspec'].';dbname='.$dsn['database'];
		    	$pdo = new PDO($connection_String, $dsn['username'], $dsn['password'], array(PDO::FETCH_ASSOC));
		    }
			catch(PDOException $e)
		    {
				$this->sql_error($e->getMessage());
		    }
		return $pdo;
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

	    return array('resultset' => $resultset, 'error' => $error);
	}

	public function sql_select_exe_run($resultset)
	{
		$results = array();

	    foreach ($resultset as $row)
		{
	      		$results[] = $row;
	    }
		
		return $results; 
	}
}
?>