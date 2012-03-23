<?php
/**
 * 
 * Main interface to the backend database abstraction
 * Currently supports MDB2, PHP's built in PDO and pure PHP in MySQL
 * @author Paul Fryer
 *
 */
class database_execution
{
	private static $backends = array('pdo', 'mdb2', 'php'); //Preference order of backends
	public $type = NULL;
	protected static $instance;

	protected $custom = false;
	protected $user_support = true;
	protected $read;
	protected $write = NULL; //FIXME Temp hack from avi to allow the insert methods to get the last id, needs new method I think.
	protected $connected = false;
	protected $show_error_sql = true;

	public $max_sql_length = 10240; //Max Sql query length
	
	protected $dsn_read = array();
	protected $dsn_write = array();
	protected $supports_database_backup = false;
	protected $supports_prepared_statments = false;
	protected $supports_transactions = false;
	protected $sql_prepared_statment_placeholder_prefix = ':';
	

	public static function factory($read_dsn = array(), $write_dsn = array())
	{
		if(isset($read_dsn['backend'])){ $backend = $read_dsn['backend']; }
   		if(!isset($backend)) { $backend = database_execution::$backends[0];}
   		
   		if(file_exists(DATABASE_LAYER_PATH.DIRECTORY_SEPARATOR.'backends'.DIRECTORY_SEPARATOR.$backend.'.inc'))
   		{
   			require_once(DATABASE_LAYER_PATH.DIRECTORY_SEPARATOR.'backends'.DIRECTORY_SEPARATOR.$backend.'.inc');
   			$c = 'database_execution_'.$backend;
   		}
   		else
   		{
		    die('Unable to build the requested database layer '.DATABASE_LAYER_PATH.DIRECTORY_SEPARATOR.'backends'.DIRECTORY_SEPARATOR.$backend.'.inc');
   		}
   		
   		return new $c;
	}
	
	public static function singleton($read_dsn = array(), $write_dsn = array())
	{		
		if(!isset(self::$instance) || self::$instance == false || !is_null(self::$instance))
		{
			self::$instance = database_execution::factory($read_dsn = array());	
		}
		
		self::$instance->set_dsn($read_dsn, $write_dsn);
		
		return self::$instance;
	}
	
	public function set_dsn($dsn_read = array(), $dsn_write = array()){
		$this->dsn_read = $dsn_read;
		$this->type = $dsn_read['backend'];
		if(isset($dsn_write['phptype'])){
			$this->dsn_write = $dsn_write;
		}else{
			$this->dsn_write = $this->dsn_read;
		}
	}
	
	public function __destruct()
	{
		$this->commit();
		self::$instance = false;
	}
	
	public function destroy()
	{
		$this->commit();
		self::$instance = false;
	}

	/**
	 * The public call to build the DSN and connect to the backend lib
	 * @return true
	 */
	public function sql_connect($soft_connect = false)
	{
		if(!$this->custom){
			$this->user_support = database::singleton()->user_support();
		}

		if(!$this->connected)
		{
				$this->read = $this->connect($this->dsn_read, NULL, NULL, $soft_connect);
				//If logged in create db write user this should help prevent against SQL injection
				//As only logged in can use the db writable user
				if(class_exists('Session')){
					Session::Singleton();
				}
				
				if((isset($_SESSION['userid']) && $_SESSION['userid'] != 0) || defined('DATABASE_INSTALL_MODE'))
				{
					if($this->user_support)
					{
						if(!isset($this->dsn_write['password'])){
							$this->dsn_write['password'] = $this->dsn_read['password'];
						}
						
						//Create write user
						$this->write = $this->connect($this->dsn_write, NULL, NULL, $soft_connect, true);
					}
					else
					{
						$this->write =& $this->read;
					}
				}
				
			if(!$this->read){
				return false;
			}

			$this->dsn = isset($dsn)?$dsn:NULL;
			$this->connected = true;
		}
		
		if($this->read == null)
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
		$this->write = $this->connect($this->dsn_write);
		return true;
	}

	public function destroy_write_user()
	{
		$this->commit();
		unset($this->write);
		if(isset($this->dsn_default)){
			$this->dsn = $this->dsn_default;
		}
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
   		 $this->custom = true;
   		 return $this;
	}
	
	public function connect($dsn, $options = NULL, $mods = NULL, $soft_connect = false)
	{
		var_dump($dsn);
			var_dump($db_backend);
			var_dump(debug_backtrace());
			die();
	}

	public function sql_run($sql, $values, $writeuser, $soft_connect = false)
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
	
	public function backupDatabase()
	{
		
	}
	
	/**
	 * Proxy method back to the main db layer
	 * @param $message
	 * @param $line
	 * @param $file
	 * @return unknown_type
	 */
	protected function sql_error($message, $line = NULL, $file = NULL, $soft_connect = false)
	{
		if($soft_connect){
			return false;
		}
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
	
	public function supports_transactions()
	{
		return $this->supports_transactions;
	}
	
	public function commit()
	{
		return false;
	}
	
	public function start_transaction()
	{
		return false;
	}
}