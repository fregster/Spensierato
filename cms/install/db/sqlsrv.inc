<?php
$int = 'BIGINT';
$increment = 'IDENTITY';

require_once 'generic.inc';

/** build sql strings and execute the table creation.
 *  this code is MySQL and SQLite specific.
 */
foreach ($tables as $table => $data){

	//prefix the table with the unique string submitted by the user
	$table_orig = $table;
	$table = $prefix.'_'.$table;

	//build the column definitions
	$cols = array();
	foreach ($data['cols_define'] as $col_name => $definition){
		$col = '['.$col_name.']'.' '.$definition;
		if((isset($primary_key_in_define) && $primary_key_in_define == true) && ($col_name == $data['primary_key']))
		{
			if(isset($strip_unsigned_ints) && $strip_unsigned_ints)
			{
				$col = str_replace('UNSIGNED', '', $col);
			}
			
			$col = str_replace('NOT NULL', '', $col);
			
			if($inc_length > 0 && (substr($col, -$inc_length) == $increment)){
//				echo('<p>triming '.$inc_length.'<br/>Start: '.substr($col, 0, -$inc_length).' End: '.substr($col, -$inc_length).'</p>');
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
			echo($key);
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

	//add table charset if exists
	$charset = '';
	if (isset($data['charset']) && !is_null($data['charset']) && $data['charset'] != false) {
		$charset = "DEFAULT CHARSET='{$data['charset']}'";
	}

	//construct sql string
	$sql = str_ireplace('UNSIGNED', '', "CREATE TABLE $table ($cols $primary_key $keys) $charset;");

	//execute sql
//echo('<br/>'.$sql);
//die();
	$result = @$db->query($sql);

	if(!$result)
	{
		die('<br/>Failed to create table '.$table.'. The sql that triggered the error:<br />'. $sql);
	}
}
unset($table, $data);

$user_create_sql = array();
$user_create_sql[] = 'CREATE LOGIN '.$user_rw.' WITH PASSWORD = \''.$user_db_password.'\'; USE '.$database.' CREATE USER '.$user_rw.' FOR LOGIN '.$user_rw.';';
$user_create_sql[] = 'CREATE LOGIN '.$user_r.' WITH PASSWORD = \''.$user_db_password.'\'; USE '.$database.' CREATE USER '.$user_r.' FOR LOGIN '.$user_r.';';
$user_create_sql[] = 'GRANT SELECT, INSERT, UPDATE, DELETE ON '.$grant_db.'	TO '.$grant_user_rw;
$user_create_sql[] = 'GRANT SELECT ON '.$grant_db.'	TO '.$grant_user_r;