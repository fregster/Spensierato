<?php
class database_execution_php extends database_execution
{
	public function connect($dsn, $options = NULL, $mods = NULL)
	{
		$db = @mysql_connect($dsn['hostspec'], $dsn['username'], $dsn['password']);
		if(!$db) 
		{
			$error = 'Unable to connect to the database';
			$this->sql_error($error);
		}
		
		if (! @mysql_select_db($dsn['database']) )
		{
			$error = 'Unable to locate the database';
			$this->sql_error($error);
		}
		return $db;
	}
	
	public function sql_run($sql, $values, $writeuser)
	{
		$error = false;
		$result = mysql_query($sql);
		
		if (!$result) 
		{
	  		$error = mysql_error();
		}
		
		return array('resultset' => $result, 'error' => $error);
	}
	
	public function sql_select_exe_run($resultset)
	{
		$results = array();
		while ($row = mysql_fetch_assoc($resultset))
		{
			$results[] = $row;	
		}
		mysql_free_result($resultset);
		return $results;
	}
	
	public function lastInsertID($table,$col)
	{
		//TODO have a think about how to implement this as it depends on the driver etc
	}
}
?>