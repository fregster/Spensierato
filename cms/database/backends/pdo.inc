<?php
class database_pdo extends database
{
	protected function connect($dsn, $options = NULL, $mods = NULL)
	{
		try {
		    	$pdo = new PDO("mysql:host=$dsn['hostspec'];dbname=$dsn['database']", "$dsn['username']", "$dsn['password']");
		    }
			catch(PDOException $e)
		    {
				$this->sql_error($e->getMessage());
		    }
		return $pdo;
	}
	
	protected function sql_run($sql, $values, $writeuser)
	{
		$error = false;
		
		$db &= $this->read;
		if($writeuser == true)
		{
			$db &= $this->write;
		}
		
		try 
		{
			$resultset = $pdo->query($sql);
		}
		catch(PDOException $e)
	    {
	   		$this->sql_error($e->getMessage());
	    }
	    
	    return array('resultset' => $resultset, 'error' => $error);
	}
	
}
?>