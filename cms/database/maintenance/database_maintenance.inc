<?php
class database_maintenance
{
	protected $create_db_users = true;
	protected $tables = array();
	protected $primary_key_in_define = false;
	protected $default_table_engine = null;
	
	public $int = 'INT';
	public $tiny_int = 'TINYINT';
	public $long_text = 'LONGTEXT';
	public $increment = 'AUTO_INCREMENT';
	public $varchar = 'VARCHAR';
	public $col_quote = '';
	
	private static $instance;
	
	public static function Singleton($force_type = null)
	{
	   if (!isset(self::$instance)) {
		   
	   		if(!is_null($force_type) && file_exists('.'.DS.$force_type.'.inc'))
	   		{
	   			$c = 'database_maintenance_'.$force_type;
	   		}
	   		else if(file_exists('.'.DS.DATABASE_DRIVER.'.inc'))
	   		{
	   			$c = 'database_maintenance_'.DATABASE_DRIVER;
	   		}
	   		else
	   		{
	   			$c = __CLASS__;
	   		}
	   				   
		   self::$instance = new $c;
	   }
	
	   return self::$instance;
	}
	
	public function __construct()
	{

	}
	
	public function optimize()
	{
		return true;
	}
	
	public function create_database(){
		
	}
	
	/** 
	 * For creating larger DB schema's based on in inputed array
	 * 
	 * The 'keys' array should conform to these standards:
	 *
	 * () - choose 1 of the options
	 * [] - optional. leave blank if not required
	 * '' - place holder for a string
	 *
	 * array((KEY|INDEX|UNIQUE|FULLTEXT [(KEY|INDEX)]) =>
	 * 			array(['key_alias' =>]
	 * 					array('col_name1', 'col_name2', ...) can contain any amount of columns
	 * 			)
	 * 		)
	 * 
	 * EXAMPLE:
	 * 
	 * $tables['tbl_example'] = array(
	 * 										'charset' => $default_charset,
											'cols_define' =>
												array(
													'id' => $int.' UNSIGNED NOT NULL '.$increment,
													'external_id' => $int.' UNSIGNED NOT NULL',
													'data' => $long_text,
													'title' => varchar(128).' NOT NULL',
													'revisor' => $int.' UNSIGNED NOT NULL',
													'revised' => $int.' NULL',
													'approved' => $tiny_int." DEFAULT '0'",
													'search' => 'TEXT',
												),
											'primary_key' => 'id',
											'keys' => array('UNIQUE KEY' => array('key_name_unique' => array('external_id')), 'KEY' => array('key_name_indexA' => array('external_id','title')));
											);
	 * 
	 */
	public function create_tables($tables = array(), $prefix = null, $use_prefix_seperator = true){
		
		/** build sql strings and execute the table creation.
		 *  this code is MySQL and SQLite specific but should work on SQL Server.
		 */
		if(!is_null($prefix) && $use_prefix_seperator){
			$prefix = $prefix.'_';
		}
		
		foreach ($tables as $table => $data){
		
			//prefix the table with the unique string submitted by the user
			$table_orig = $table;
			$table = $prefix.$table;
			
			//Set default engine
			if(!isset($data['engine'])) { $data['engine'] = $this->default_table_engine; }
		
			//build the column definitions
			$cols = array();
			foreach ($data['cols_define'] as $col_name => $definition){
				$col = $col_name.' '.$definition;
				if((isset($this->primary_key_in_define) && $this->primary_key_in_define == true) && ($col_name == $data['primary_key']))
				{
					if(isset($this->strip_unsigned_ints) && $this->strip_unsigned_ints)
					{
						$col = str_replace('UNSIGNED', '', $col);
					}
					
					$col = str_replace('NOT NULL', '', $col);
					
					if($inc_length > 0 && (substr($col, -$inc_length) == $increment)){
						$col = substr($col, 0, -$inc_length) . ' PRIMARY KEY '.substr($col, -$inc_length);
					}
					else
					{
						$col .= ' PRIMARY KEY';
					}
				}
				
				$cols[] = $col;
				
			}
			unset($col_name, $definition);
			$cols = implode(', ', $cols);
		
			//add a column as the primary key, only if it is a column in cols_define
			$primary_key = '';
			if (!is_null($data['primary_key'])){
				if (array_key_exists($data['primary_key'], $data['cols_define'])) {
					 if(isset($primary_key_in_define) && $primary_key_in_define != true)
					 {
					 	$primary_key = ", PRIMARY KEY ({$data['primary_key']})";
					 }
				}
				else{
					die('<br />Failed to create table '.$table.' due to an invalid primary key: '.$data['primary_key']);
				}
			}
		
			//build non-primary key definitions, only if all key columns exist in cols_define
			$keys = array();
			if(isset($data['keys']))
			{
				foreach ($data['keys'] as $key_type => $key_data){
					$alias_key = key($key_data);
					if (is_string($alias_key)) {
						$alias = $alias_key;
					}
					else{
						$alias = '';
					}
					foreach ($key_data[$alias_key] as $column){
						if (!array_key_exists($column, $data['cols_define'])) {
							die('<br />Failed to create table '.$table.' due to an invalid '.$key_type.': '.$column);
						}
					}
					$key = $key_type.' '.$alias.' ('.implode(', ', $key_data[$alias_key]).')';
					$keys[] = $key;
				}
			}
			unset($key_type, $key_data);
			if (!empty($keys)) {
				$keys = ', '.implode(', ', $keys);
			}
			else{
				$keys = '';
			}
		
			//add storage engine if exists
			$engine = '';
			if (isset($data['engine']) && !is_null($data['engine']) && $data['engine'] != false) {
				$engine = "ENGINE='{$data['engine']}'";
			}
		
			//add table charset if exists
			$charset = '';
			if (isset($data['charset']) && !is_null($data['charset']) && $data['charset'] != false) {
				$charset = "DEFAULT CHARSET='{$data['charset']}'";
			}
		
			//construct sql string
			$sql = "CREATE TABLE $table ($cols $primary_key $keys) $engine $charset;";
		
			//execute sql
			$result = @database::singleton()->direct_sql($sql);
		
			if(!$result)
			{
				die('<br/>Failed to create table '.$table.'. The sql that triggered the error:<br />'. $sql);
			}
		}
		unset($table, $data);
		
		
	}
	
	public function create_user($user, $database, $permissions = array(), $host = array())
	{
		
	}
		
	
	public function varchar($length)
	{
		$this->varchar;
		if($this->varchar === 'TEXT'){
			return $this->varchar;
		}
		return $this->varchar.'('.$length.')';
	}
}