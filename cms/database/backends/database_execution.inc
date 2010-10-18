<?php
class database_execution
{
	private static $backends = array('mdb2', 'pdo', 'php'); //Preference order of backends
	protected static $instance;

	protected $user_support = true;
	protected $read;
	protected $write = NULL; //FIXME Temp hack from avi to allow the insert methods to get the last id, needs new method I think.
	protected $connected = false;
	protected $show_error_sql = true;
	protected $db_backend;

	public $max_sql_length = 10240; //Max Sql query length
	
	private $dsn_default;
	private $dsn;
	protected $supports_prepared_statments = false;
	protected $sql_prepared_statment_placeholder_prefix = ':';
	

	public static function factory()
	{
   		if(!isset($GLOBALS['database_backend'])) { $GLOBALS['database_backend'] = database_execution::$backends[0]; }
   		if(file_exists(DATABASE_LAYER_PATH.'/backends/'.$GLOBALS['database_backend'].'.inc'))
   		{
   			require_once(DATABASE_LAYER_PATH.'/backends/'.$GLOBALS['database_backend'].'.inc');
   			$c = 'database_execution_'.$GLOBALS['database_backend'];
   		}
   		else
   		{
		    die('Unable to build the requested database layer '.DATABASE_LAYER_PATH.'/backends/'.$GLOBALS['database_backend'].'.inc');
   		}
   		
	   return new $c;
	}
	
	public static function singleton()
	{
		if(!isset(self::$instance) || self::$instance == false || !is_null(self::$instance))
		{
	   		self::$instance = database_execution::factory();
		}
		
		return self::$instance;
	}
	
	public function __destruct()
	{
		self::$instance = false;
	}
	
	public function destroy()
	{
		self::$instance = false;
	}

	/**
	 * The public call to build the DSN and connect to the backend lib
	 * @return true
	 */
	public function sql_connect()
	{
		$this->user_support = database::singleton()->user_support();
		if(!$this->connected)
		{
			$this->dsn_default = array(
			    'phptype'  => $GLOBALS['database_driver'],
			    'username' => $GLOBALS['database_user_read'],
			    'password' => $GLOBALS['database_password'],
			    'hostspec' => $GLOBALS['database_host'],
			    'database' => $GLOBALS['database_name'],
				);
				
				if(isset($GLOBALS['database_mode']))
				{
					$this->dsn_default['mode'] = $GLOBALS['database_mode'];
				}

				$this->read = $this->connect($this->dsn_default);
				//If logged in create db write user this should help prevent against SQL injection
				//As only logged in can use the db writeable user
				if(isset($_SESSION['userid']) && $_SESSION['userid'] != 0)
				{
					if(database::singleton()->user_support())
					{
						//Create write user
						$dsn = array(
						    'phptype'  => $GLOBALS['database_driver'],
						    'username' => $GLOBALS['database_user_readwrite'],
						    'password' => $GLOBALS['database_password'],
						    'hostspec' => $GLOBALS['database_host'],
						    'database' => $GLOBALS['database_name'],
						);
						if(isset($GLOBALS['database_mode']))
						{
							$this->dsn_default['mode'] = $GLOBALS['database_mode'];
						}
						$this->write = $this->connect($dsn);
					}
					else
					{
						$this->write =& $this->read;
					}
				}

			$this->dsn = isset($dsn)?$dsn:NULL;
			$this->connected = true;
		}
		
		if($this->db_backend == null)
		{
			$this->sql_error('Driver failed to load', __LINE__, __FILE__);
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
		if(database::singleton()->user_support())
		{
			$this->dsn = array(
			    'phptype'  => $GLOBALS['database_driver'],
			    'username' => $GLOBALS['database_user_readwrite'],
			    'password' => $GLOBALS['database_password'],
			    'hostspec' => $GLOBALS['database_host'],
			    'database' => $GLOBALS['database_name'],
			);
			
			if(isset($GLOBALS['database_mode']))
			{
				$this->dsn_default['mode'] = $GLOBALS['database_mode'];
			}
			$this->write = $this->connect($this->dsn);
		}
		else
		{
			$this->write =& $this->read;
		}
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
	public function custom_db_object($dsn, $options = NULL, $mods = NULL)
	{
		$this->connect($dsn,$options,$mods);
		return $this;
	}
	
	public function connect($dsn, $options = NULL, $mods = NULL)
	{
		
	}

	public function sql_run($sql, $values, $writeuser)
	{
		
	}

	public function sql_select_exe_run($resultset)
	{

	}
	
	public function lastInsertID($table,$col)
	{
		
	}
	
	public function createDatabase($database)
	{
		
	}
	
	public function dropDatabase($database)
	{
		
	}
	
	/**
	 * Proxy method back to the main db layer
	 * @param $message
	 * @param $line
	 * @param $file
	 * @return unknown_type
	 */
	protected function sql_error($message, $line = NULL, $file = NULL)
	{
		database::singleton()->sql_error($message, $line, $file);
	}
	
	public function supports_prepared_statments()
	{
		return $this->supports_prepared_statments;
	}
	
	public function prepared_statment_placeholder()
	{
		return $this->sql_prepared_statment_placeholder_prefix;
	}
}