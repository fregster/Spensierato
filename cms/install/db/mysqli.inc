<?php

require_once 'generic.inc';

/**
 * Default storage engine
 *
 * Use MyISAM for a small performance improvment over InnoDB in light situations
 * Use InnoDB anywhere where you need clustering, caching data, higher concurrency, background flushes
 */

if(!isset($default_table_engine))
{
	$default_table_engine = 'MYISAM';
	//$default_table_engine = 'INNODB';
}

if(!isset($archive_table_engine))
{
	//$archive_table_engine = 'ARCHIVE';
	$archive_table_engine = 'MYISAM';
}

if(!isset($full_text_search_table_engine))
{
	$full_text_search_table_engine = 'MYISAM';
}

if(!isset($cache_table_engine))
{
	$cache_table_engine = 'MEMORY';
}

if(!isset($default_charset))
{
	$default_charset = 'latin1';
}

if(!isset($long_text))
{
	$long_text = 'LONGTEXT';
}

if(!isset($tiny_int))
{
	$tiny_int = 'TINYINT';
}

if(!isset($full_text))
{
	$full_text = 'FULLTEXT';
}

if(!isset($full_text_enabled))
{
	$full_text_enabled = true;
}

if(!isset($allow_unique_keys))
{
	$allow_unique_keys = true;
}

/** this is where you add MySQL-specific data to the table definitions.
 *  for example, storage engine and charset are defined here.
 *  this is also where you can edit the table definitions to be mysql-compliant
 *  where mysql doesn't conform to the sql standard. for example, change TEXT to LONGTEXT where required.
 */

/** the 'keys' array should conform to these standards:
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
 */

$tables['tbl_commands']['engine'] = $default_table_engine;
$tables['tbl_commands']['charset'] = $default_charset;
$tables['tbl_commands']['keys'] = array();

$tables['tbl_elements_revisions']['cols_define']['data'] = $long_text;
$tables['tbl_elements_revisions']['cols_define']['search'] = $long_text;
$tables['tbl_elements_revisions']['engine'] = $default_table_engine;
$tables['tbl_elements_revisions']['charset'] = $default_charset;
$tables['tbl_elements_revisions']['keys'] = array();

$tables['tbl_elements']['engine'] = $default_table_engine;
$tables['tbl_elements']['charset'] = $default_charset;
$tables['tbl_elements']['keys'] = array();

$tables['tbl_elements_data']['engine'] = $default_table_engine;
$tables['tbl_elements_data']['charset'] = $default_charset;
$tables['tbl_elements_data']['cols_define']['data'] = $long_text;
$tables['tbl_temp']['keys'] = array();

$tables['tbl_search']['engine'] = $full_text_search_table_engine;
$tables['tbl_search']['charset'] = $default_charset;
$tables['tbl_search']['cols_define']['search'] = $long_text;
$tables['tbl_search']['cols_define']['keywords'] = $long_text;
if($allow_unique_keys)
{
	$tables['tbl_search']['keys'] = array('UNIQUE KEY' => array('id_to_file' => array('id','file')));
}
if($full_text_enabled)
{
	$tables['tbl_search']['keys']['FULLTEXT'] = array(array('search'));
}

$tables['tbl_elements_draft']['cols_define']['data'] = $long_text;
$tables['tbl_elements_draft']['engine'] = $default_table_engine;
$tables['tbl_elements_draft']['charset'] = $default_charset;
if($allow_unique_keys)
{
	$tables['tbl_elements_draft']['keys'] = array('UNIQUE KEY' => array('user_to_element' => array('element_id', 'user_id')));
}

$tables['tbl_list_data']['engine'] = $default_table_engine;
$tables['tbl_list_data']['charset'] = $default_charset;
$tables['tbl_list_data']['keys'] = array();

$tables['tbl_list_membership']['engine'] = $default_table_engine;
$tables['tbl_list_membership']['charset'] = $default_charset;
if($allow_unique_keys)
{
	$tables['tbl_list_membership']['keys'] = array('UNIQUE KEY' => array('user_id' => array('user_id', 'list_id')));
}

$tables['tbl_page_elements']['engine'] = $default_table_engine;
$tables['tbl_page_elements']['charset'] = $default_charset;
if($allow_unique_keys)
{
	$tables['tbl_page_elements']['keys'] = array('UNIQUE KEY' => array('elements_to_page_link' => array('page_id','element_id')), 'UNIQUE KEY' => array('only_one_main_element' => array('element_id','main')));
}

$tables['tbl_pages']['engine'] = $default_table_engine;
$tables['tbl_pages']['charset'] = $default_charset;
$tables['tbl_pages']['cols_define']['append_tags'] = $long_text;
if($allow_unique_keys)
{
	$tables['tbl_pages']['keys'] = array('KEY' => array('valid_child' => array('visible','deleted','parent_id')));
}
$tables['tbl_security']['cols_define']['data'] = $long_text;
$tables['tbl_security']['engine'] = $default_table_engine;
$tables['tbl_security']['charset'] = $default_charset;
$tables['tbl_security']['keys'] = array();

$tables['tbl_settings']['engine'] = $default_table_engine;
$tables['tbl_settings']['charset'] = $default_charset;
$tables['tbl_settings']['keys'] = array();

$tables['tbl_temp']['engine'] = $default_table_engine;
$tables['tbl_temp']['charset'] = $default_charset;
$tables['tbl_temp']['keys'] = array();

$tables['tbl_users']['engine'] = $default_table_engine;
$tables['tbl_users']['charset'] = $default_charset;
if($allow_unique_keys)
{
	$tables['tbl_users']['keys'] = array('UNIQUE KEY' => array('username' => array('username')), 'KEY' => array('login' => array('username','password')));
}

$tables['tbl_users_data']['cols_define']['data'] = $long_text;
$tables['tbl_users_data']['engine'] = $default_table_engine;
$tables['tbl_users_data']['charset'] = $default_charset;
if($allow_unique_keys)
{
	$tables['tbl_users_data']['keys'] = array('KEY' => array('id' => array('user_id')));
}
/** build sql strings and execute the table creation.
 *  this code is MySQL specific.
 */
foreach ($tables as $table => $data){

	//prefix the table with the unique string submitted by the user
	$table = $prefix.'_'.$table;

	//build the column definitions
	$cols = array();
	foreach ($data['cols_define'] as $col_name => $definition){
		$cols[] = $col_name.' '.$definition;
	}
	unset($col_name, $definition);
	$cols = implode(', ', $cols);

	//add a column as the primary key, only if it is a column in cols_define
	$primary_key = '';
	if (!is_null($data['primary_key'])) {
		if (array_key_exists($data['primary_key'], $data['cols_define'])) {
			$primary_key = ", PRIMARY KEY ({$data['primary_key']})";
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
			$keys[] = $key_type.' '.$alias.' ('.implode(', ', $key_data[$alias_key]).')';
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
	$result = $db->query($sql);

	if(!$result)
	{
		$error = '<br/>Failed to create table '.$table.'. The sql that triggered the error:<br />'. $sql;
		die($error);
	}
}
unset($table, $data);

//set the db name and the user info in a mysql-compliant manner fot use in the GRANT queries.
$grant_db = "$database.*";
$grant_user_rw = "'$user_rw'@'localhost' IDENTIFIED BY '$user_db_password'";
$grant_user_r = "'$user_r'@'localhost' IDENTIFIED BY '$user_db_password'";