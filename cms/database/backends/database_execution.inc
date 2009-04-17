<?php
class database_execution
{
	private static $backends = array('mdb2', 'pdo', 'php'); //Preference order of backends
	protected static $instance;

	protected $read;
	public $write = NULL;
	protected $connected = false;

	private $dsn_default;
	private $dsn;

	public static function Singleton()
	{
	   if (!isset(self::$instance))
	   {
	   		if(!isset($GLOBALS['database_backend'])) { $GLOBALS['database_backend'] = database_execution::$backends[0]; }

	   		if(file_exists(dirname(__FILE__).'/'.$GLOBALS['database_backend'].'.inc'))
	   		{
	   			require_once(dirname(__FILE__).'/'.$GLOBALS['database_backend'].'.inc');
	   			$c = 'database_execution_'.$GLOBALS['database_backend'];
	   		}
	   		else
	   		{
			    die('Unable to build the requested database layer');
	   		}

		   self::$instance = new $c;
	   }
	   return self::$instance;
	}

	/**
	 * The public call to build the DSN and connect to the backend lib
	 * @return true
	 */
	public function sql_connect()
	{
		if(!$this->connected)
		{
			$this->dsn_default = array(
			    'phptype'  => $GLOBALS['database_driver'],
			    'username' => $GLOBALS['database_user_read'],
			    'password' => $GLOBALS['database_password'],
			    'hostspec' => $GLOBALS['database_host'],
			    'database' => $GLOBALS['database_name'],
				);

				$this->read = $this->connect($this->dsn_default);

				//If logged in create db write user this should help prevent against SQL injection
				//As only logged in can use the db writeable user
				if(isset($_SESSION['userid']) && $_SESSION['userid'] != 0)
				{
					//Create write user
					$dsn = array(
					    'phptype'  => $GLOBALS['database_driver'],
					    'username' => $GLOBALS['database_user_readwrite'],
					    'password' => $GLOBALS['database_password'],
					    'hostspec' => $GLOBALS['database_host'],
					    'database' => $GLOBALS['database_name'],
					);
					$this->write = $this->connect($dsn);
				}

			$this->dsn = isset($dsn)?$dsn:NULL;
			$this->connected = true;
		}

		return $this->connected;
	}

	/**
	 * force_write_user
	 * Use this method to overwrite the read user with the write user
	 * This can be used then to allow database updates if there is a requirement to do
	 * inserts etc with a not logged in user
	 *
	 * When finished call destroy_write_user() to revert back to the read user
	 * Examples for use are in registration when you wish to store some data but the user will not be logged in
	 *
	 * Please be careful and ensure to fully protect the input against posible auto form fillers etc.
	 *
	 */
	public function force_write_user()
	{
		//Create write user
		$this->dsn = array(
		    'phptype'  => $GLOBALS['database_driver'],
		    'username' => $GLOBALS['database_user_readwrite'],
		    'password' => $GLOBALS['database_password'],
		    'hostspec' => $GLOBALS['database_host'],
		    'database' => $GLOBALS['database_name'],
		);
		$this->write = $this->connect($this->dsn);
	}

	public function destroy_write_user()
	{
		unset($this->write);
		$this->dsn = $this->dsn_default;
	}

	/**
	 * This method needs to be extended for each lib type
	 * It should return a db object that will get mapped to $this->read or $this->wrtie as required
	 *
	 * @param $dsn
	 * @param $options
	 * @param $mods
	 * @return database object
	 */
	protected function connect($dsn, $options = NULL, $mods = NULL)
	{

	}

	public function sql_run($sql, $values, $writeuser)
	{

	}

	public function sql_select_exe_run($resultset)
	{

	}
}
?>