﻿<?php

/** this file contains arrays defining table creation data
 *  which is compliant to the sql standard. to add any non-standard
 *  data, add array elements in a db-specific file (e.g., mysqli.inc).
 *
 *  the table arrays are structured as follows:
 *  $tables['table_name'] = array('cols_define' =>
 * 									array('col_name1' => 'col definition, including type, length and constraints',
 * 										  ...,
 *										  n),
 * 									'primary_key' => 'name of the promary key. must be a column defined in cols_define
 * 								  )
 */
$create_db_users = true;
$tables = array();

if(!isset($primary_key_in_define))
{
	$primary_key_in_define = false;
}

if(!isset($int))
{
	$int = 'INT';
}

if(!isset($tiny_int))
{
	$tiny_int = 'TINYINT';
}

if(!isset($long_text))
{
	$long_text = 'LONGTEXT';
}

if(!isset($increment))
{
	$increment = 'AUTO_INCREMENT';
}

if(!isset($varchar))
{
	$varchar = 'VARCHAR';
}

if(!isset($col_quote))
{
	$col_quote = '';
}

function varchar($length)
{
	global $varchar;
	if($varchar === 'TEXT'){
		return $varchar;
	}
	return $varchar.'('.$length.')';
}

$tables['tbl_commands'] = array(
								'cols_define' =>
									array(
										'command' => varchar(32).' NOT NULL',
										'type' => varchar(32).' NOT NULL',
										'instruction' => varchar(256).' NOT NULL'
									),
								'primary_key' => 'command',
								);

$tables['tbl_elements_revisions'] = array(
											'cols_define' =>
												array(
													'id' => $int.' UNSIGNED NOT NULL '.$increment,
													'element_id' => $int.' UNSIGNED NOT NULL',
													'data' => 'TEXT',
													'title' => varchar(128).' NOT NULL',
													'revisor' => $int.' UNSIGNED NOT NULL',
													'revised' => $int.' UNSIGNED NOT NULL',
													'approved' => $tiny_int." DEFAULT '0'",
													'search' => 'TEXT',
												),
											'primary_key' => 'id',
											);

$tables['tbl_elements'] = array(
								'cols_define' =>
									array(
										'id' => $int.' UNSIGNED NOT NULL '.$increment,
										'type' => varchar(32).' NOT NULL',
										'created' => $int." UNSIGNED DEFAULT '0'",
										'creator' => $int.' UNSIGNED NOT NULL',
										'edited' => $int.' UNSIGNED',
										'editor' => $int.' UNSIGNED',
										'visible' => $tiny_int." UNSIGNED DEFAULT '1'",
										'deleted' => $tiny_int." UNSIGNED DEFAULT '0'",
										'approved' => $int.' UNSIGNED',
										'approver' => $int.' UNSIGNED',
										'active_revision' => $int.' UNSIGNED'
									),
								'primary_key' => 'id',
								);

$tables['tbl_elements_data'] = array(
									'cols_define' =>
										array(
											'id' => $int.' UNSIGNED NOT NULL',
											'title' => varchar(128)." NOT NULL DEFAULT 'Title'",
											'data' => 'TEXT NOT NULL',
											),
									'primary_key' => 'id',
									);

$tables['tbl_search'] = array(
									'cols_define' =>
										array(
											'search_id' => $int.' UNSIGNED NOT NULL '.$increment,
											'id' => $int.' UNSIGNED NOT NULL',
											'file' => $int." UNSIGNED DEFAULT '0'",
											'title' => varchar(128).' NOT NULL',
											'search' => 'TEXT NOT NULL',
											'keywords' => 'TEXT',
											'indexed' => $int." UNSIGNED DEFAULT '0'",
											),
									'primary_key' => 'search_id',
									);							

$tables['tbl_elements_draft'] = array(
									'cols_define' =>
										array(
											'id' => $int.' UNSIGNED NOT NULL '.$increment,
											'element_id' => $int.' UNSIGNED NOT NULL',
											'user_id' => $int.' UNSIGNED NOT NULL',
											'ts' => $int.' UNSIGNED NOT NULL',
											'title' => varchar(128).' NOT NULL',
											'data' => 'TEXT NOT NULL'
											),
									'primary_key' => 'id',
									);

$tables['tbl_list_data'] = array(
								'cols_define' =>
									array(
										'id' => $int.' UNSIGNED NOT NULL '.$increment,
										'name' => varchar(128).' NOT NULL',
										'valid_from' => $int.' UNSIGNED',
										'valid_to' => $int.' UNSIGNED NOT NULL',
										'created' => $int.' UNSIGNED NOT NULL',
										'created_by' => $int.' UNSIGNED NOT NULL',
										'edited' => $int.' UNSIGNED',
										'edited_by' => $int.' UNSIGNED'
										),
								'primary_key' => 'id',
								);

$tables['tbl_list_membership'] = array(
									'cols_define' =>
										array(
											'user_id' => $int.' UNSIGNED NOT NULL',
											'list_id' => $int.' UNSIGNED NOT NULL'
											),
									'primary_key' => NULL,
									);
									
$tables['tbl_log'] = array(
									'cols_define' =>
										array(
											'id' => $int.' UNSIGNED NOT NULL '.$increment,
											'ts' => $int.' UNSIGNED NOT NULL',
											'security' => $int.' UNSIGNED NOT NULL',
											'user' => $int.' UNSIGNED NOT NULL',
											'action' => varchar(128).' NOT NULL',
											'detail' => varchar(255).' NOT NULL',
											'ip' => varchar(64). ' NOT NULL',
											'request_type' => varchar(12).' NOT NULL',
											'request_data' => $long_text,
											'request_uri' => varchar(255).' NOT NULL',
											'user_agent' => varchar(128).' NOT NULL',
											'ids' => $long_text,
											'trigger_file' => varchar(64). ' NOT NULL',
											'trigger_line' => $int.' UNSIGNED NOT NULL',
											),
									'primary_key' => 'id',
									);

$tables['tbl_page_elements'] = array(
									'cols_define' =>
									array(
										'id' => $int.' UNSIGNED NOT NULL '.$increment,
										'page_id' => $int.' UNSIGNED NOT NULL',
										'element_id' => $int.' UNSIGNED NOT NULL',
										'sort' => $int.' UNSIGNED NOT NULL',
										'main' => $tiny_int." UNSIGNED DEFAULT '1'",
										'ts' => $int.' UNSIGNED NOT NULL',
										'visible' => $tiny_int." UNSIGNED DEFAULT '1'",
										'valid_from' => $int.' UNSIGNED',
										'valid_to' => $int.' UNSIGNED',
										),
									'primary_key' => 'id',
									);

$tables['tbl_pages'] = array(
							'cols_define' =>
							array(
								'id' => $int.' UNSIGNED NOT NULL '.$increment,
								'product' => $tiny_int." UNSIGNED DEFAULT '0'",
								'parent_id' => $int.' UNSIGNED NOT NULL',
								'append_tags' => 'TEXT',
								'menu_type' => $tiny_int." DEFAULT '0'",
								'title' => varchar(64)." NOT NULL DEFAULT 'New Page'",
								'icon' => $tiny_int.' UNSIGNED',
								'visible' => $tiny_int.' UNSIGNED NOT NULL DEFAULT 0',
								'deleted' => $tiny_int.' UNSIGNED NOT NULL DEFAULT 0',
								'sort' => $int.' UNSIGNED NOT NULL DEFAULT 0',
								'created' => $int.' UNSIGNED NOT NULL',
								'creator' => $int.' UNSIGNED NOT NULL',
								'edited' => $int.' UNSIGNED',
								'editor' => $int.' UNSIGNED',
								'valid_from' => $int.' UNSIGNED',
								'valid_to' => $int.' UNSIGNED',
								),
							'primary_key' => 'id',
							);

$tables['tbl_security'] = array(
								'cols_define' =>
								array(
									'page_id' => $int.' UNSIGNED NOT NULL',
									'data' => 'TEXT NOT NULL'
									),
								'primary_key' => 'page_id',
								);

$tables['tbl_sessions'] = array(
								'cols_define' =>
								array(
									'id' => varchar(32).' NOT NULL',
									'expiration_ts' => $int.' UNSIGNED NOT NULL',
									'data' => 'TEXT NOT NULL'
									),
								'primary_key' => 'id',
								);						
								
$tables['tbl_settings'] = array(
								'cols_define' =>
								array(
									'setting' => varchar(64).' NOT NULL',
									'value' => varchar(128).' NOT NULL'
									),
								'primary_key' => 'setting',
								);

$tables['tbl_system'] = array(
								'cols_define' =>
								array(
									'name' => varchar(64).' NOT NULL',
									'value' => varchar(128).' NOT NULL'
									),
								'primary_key' => 'name',
								);								
								
$tables['tbl_temp'] = array(
							'cols_define' =>
							array(
								'id' => $int.' UNSIGNED NOT NULL '.$increment,
								'data' => 'TEXT NOT NULL',
								'ts' => $int.' UNSIGNED NOT NULL'
								),
							'primary_key' => 'id',
							);

$tables['tbl_users'] = array(
							'cols_define' =>
							array(
								'id' => $int.' UNSIGNED NOT NULL '.$increment,
								'username' => varchar(32).' NOT NULL',
								'password' => varchar(250),
								'hash_type' => varchar(8),
								'last_login' => $int.' UNSIGNED',
								'session_id' => varchar(128),
								'session_id_old' => varchar(128),
								'session_refresh_ts' => $int.' UNSIGNED',
								'admin' => $tiny_int." UNSIGNED DEFAULT '0'",
								'login_attempts' => $tiny_int." UNSIGNED DEFAULT '0'",
								'session_reset_nonce' => varchar(128),
								'session_reset_nonce_ts' => $int.' UNSIGNED',
								),
							'primary_key' => 'id',
							);

$tables['tbl_users_data'] = array(
								'cols_define' =>
								array(
									'id' => $int.' UNSIGNED NOT NULL '.$increment,
									'user_id' => $int.' UNSIGNED NOT NULL',
									'data' => 'TEXT'
									),
								'primary_key' => 'id',
								);
								
$tables['tbl_tags_to_pages'] = array(
								'cols_define' =>
								array(
									'page_id' => $int.' UNSIGNED NOT NULL',
									'tag' => varchar(64),
									),
								);
$tables['tbl_tags_to_elements'] = array(
								'cols_define' =>
								array(
									'element_id' => $int.' UNSIGNED NOT NULL',
									'tag' => varchar(64),
									),
								);
$tables['tbl_orders'] = array(
								'cols_define' =>
								array(
									'id' => $int.' UNSIGNED NOT NULL '.$increment,
									'email' => varchar(64),
									'data' => 'TEXT NOT NULL',
									'paid' => $tiny_int.' UNSIGNED DEFAULT 0',
									'payment_method' => varchar(64),
									),
								'primary_key' => 'id',
								);
								

								
//set the db name and the user info in a SQL compliant manner fot use in the GRANT queries.
$grant_db = "$database.*";
//TODO: write installation methods to allow this to be defined at installation time
$grant_user_rw = "'$user_rw'@'%' IDENTIFIED BY '$user_db_password_write'";
$grant_user_r = "'$user_r'@'%' IDENTIFIED BY '$user_db_password'";
$grant_user_rw_local = "'$user_rw'@'localhost' IDENTIFIED BY '$user_db_password_write'";
$grant_user_r_local = "'$user_r'@'localhost' IDENTIFIED BY '$user_db_password'";