<?php
class database_execution_php extends database_execution
{
	
	public function connect($dsn, $options = NULL, $mods = NULL, $soft_connect = false, $transaction = false)
	{
		$this->db_backend = @mysql_connect($dsn['hostspec'], $dsn['username'], $dsn['password']);
		if(!$this->db_backend)
		{
			$error = 'Unable to connect to the database';
			$this->sql_error($error);
		}

		if(isset($dsn['database']))
		{
			if (! @mysql_select_db($dsn['database']) )
			{
				$error = 'Unable to locate the database';
				$this->sql_error($error, null, null, $soft_connect);
			}
		}
	
		if($dsn['phptype'] != 'sqlite')
		{
			if(!file_exists($dsn['phptype'])){ $this->sql_error('Database offline'); }
		}
		return $this->db_backend;
	}

	public function sql_run($sql, $values, $writeuser, $soft_connect = false)
	{
		$error = false;
		if($writeuser==true)
			$result = mysql_query($sql, $this->write);
		else
			$result = mysql_query($sql, $this->read);

		if (!$result)
		{
	  		$error = mysql_error();
		}

		$final = array('resultset' => $result);
	    if($error) { $final['error'] = $error; }

		return $final;
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

		//temp solution to get installer working with mysql
		$table = $GLOBALS['table_prefix'].$table;
		$sql = "SELECT $col FROM $table ORDER BY $col DESC LIMIT 1";
		$resultset = mysql_query($sql, $this->read);
		$id = mysql_fetch_assoc($resultset);
		if (!$id) {
			die(mysql_error());
		}
		else return $id[$col];
	}

	public function createDatabase($database)
	{
		$create_db_sql = 'create database '.$database;
		mysql_query($create_db_sql, $this->db_backend);
	}

	public function dropDatabase($database)
	{
		$drop_db_sql = 'drop database '.$database;
		mysql_query($drop_db_sql, $this->db_backend);
	}

	public function query($sql_str)
	{
		return mysql_query($sql_str, $this->db_backend);
	}
}