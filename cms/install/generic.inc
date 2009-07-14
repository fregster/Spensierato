<?php

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

$tables = array();

$tables['tbl_commands'] = array(
								'cols_define' =>
									array(
										'command' => 'VARCHAR(32) NOT NULL',
										'type' => 'VARCHAR(32) NOT NULL',
										'instruction' => 'VARCHAR(256) NOT NULL'
									),
								'primary_key' => 'command',
								);

$tables['tbl_elements_revisions'] = array(
											'cols_define' =>
												array(
													'id' => 'INT UNSIGNED NOT NULL AUTO_INCREMENT',
													'element_id' => 'INT UNSIGNED NOT NULL',
													'data' => 'TEXT',
													'title' => 'VARCHAR(128) NOT NULL',
													'revisor' => 'INT UNSIGNED NOT NULL',
													'revised' => 'INT UNSIGNED NOT NULL',
													'approved' => "TINYINT DEFAULT '0'",
													'search' => 'TEXT',
												),
											'primary_key' => 'id',
											);

$tables['tbl_elements'] = array(
								'cols_define' =>
									array(
										'id' => 'INT UNSIGNED NOT NULL AUTO_INCREMENT',
										'type' => 'VARCHAR(32) NOT NULL',
										'created' => "INT UNSIGNED DEFAULT '0'",
										'creator' => 'INT UNSIGNED NOT NULL',
										'edited' => 'INT UNSIGNED',
										'editor' => 'INT UNSIGNED',
										'visible' => "TINYINT UNSIGNED DEFAULT '1'",
										'deleted' => "INT UNSIGNED DEFAULT '0'",
										'approved' => 'INT UNSIGNED',
										'approver' => 'INT UNSIGNED',
										'active_revision' => 'INT UNSIGNED'
									),
								'primary_key' => 'id',
								);

$tables['tbl_elements_data'] = array(
									'cols_define' =>
										array(
											'id' => 'INT UNSIGNED NOT NULL',
											'title' => 'VARCHAR(128) NOT NULL',
											'data' => 'TEXT NOT NULL',
											),
									'primary_key' => 'id',
									);

$tables['tbl_search'] = array(
									'cols_define' =>
										array(
											'serch_id' => 'INT UNSIGNED NOT NULL AUTO_INCREMENT',
											'id' => 'INT UNSIGNED NOT NULL',
											'file' => "INT UNSIGNED DEFAULT '0'",
											'title' => 'VARCHAR(128) NOT NULL',
											'search' => 'TEXT NOT NULL',
											'keywords' => 'TEXT',
											'indexed' => "INT UNSIGNED DEFAULT '0'",
											),
									'primary_key' => 'search_id',
									);							

$tables['tbl_elements_draft'] = array(
									'cols_define' =>
										array(
											'id' => 'INT UNSIGNED NOT NULL AUTO_INCREMENT',
											'element_id' => 'INT UNSIGNED NOT NULL',
											'user_id' => 'INT UNSIGNED NOT NULL',
											'ts' => 'INT UNSIGNED NOT NULL',
											'title' => 'VARCHAR(128) NOT NULL',
											'data' => 'TEXT NOT NULL'
											),
									'primary_key' => 'id',
									);

$tables['tbl_list_data'] = array(
								'cols_define' =>
									array(
										'id' => 'INT UNSIGNED NOT NULL AUTO_INCREMENT',
										'name' => 'VARCHAR(128) NOT NULL',
										'valid_from' => 'INT UNSIGNED',
										'valid_to' => 'INT UNSIGNED NOT NULL',
										'created' => 'INT UNSIGNED NOT NULL',
										'created_by' => 'INT UNSIGNED NOT NULL',
										'edited' => 'INT UNSIGNED',
										'edited_by' => 'INT UNSIGNED'
										),
								'primary_key' => 'id',
								);

$tables['tbl_list_membership'] = array(
									'cols_define' =>
										array(
											'user_id' => 'INT UNSIGNED NOT NULL',
											'list_id' => 'INT UNSIGNED NOT NULL'
											),
									'primary_key' => NULL,
									);

$tables['tbl_page_elements'] = array(
									'cols_define' =>
									array(
										'id' => 'INT UNSIGNED NOT NULL AUTO_INCREMENT',
										'page_id' => 'INT UNSIGNED NOT NULL',
										'element_id' => 'INT UNSIGNED NOT NULL',
										'sort' => 'INT UNSIGNED NOT NULL',
										'main' => "TINYINT UNSIGNED DEFAULT '1'",
										'ts' => 'INT UNSIGNED NOT NULL',
										'visible' => "TINYINT UNSIGNED DEFAULT '1'",
										),
									'primary_key' => 'id',
									);

$tables['tbl_pages'] = array(
							'cols_define' =>
							array(
								'id' => 'INT UNSIGNED NOT NULL AUTO_INCREMENT',
								'parent_id' => 'INT UNSIGNED NOT NULL',
								'title' => 'VARCHAR(64) NOT NULL',
								'visible' => 'TINYINT UNSIGNED NOT NULL',
								'deleted' => 'TINYINT UNSIGNED NOT NULL',
								'sort' => 'INT(11) UNSIGNED NOT NULL',
								'created' => 'INT UNSIGNED NOT NULL',
								'creator' => 'INT UNSIGNED NOT NULL',
								'edited' => 'INT UNSIGNED',
								'editor' => 'INT UNSIGNED',
								),
							'primary_key' => 'id',
							);

$tables['tbl_security'] = array(
								'cols_define' =>
								array(
									'page_id' => 'INT UNSIGNED NOT NULL',
									'data' => 'TEXT NOT NULL'
									),
								'primary_key' => 'page_id',
								);

$tables['tbl_settings'] = array(
								'cols_define' =>
								array(
									'setting' => 'VARCHAR(32) NOT NULL',
									'value' => 'VARCHAR(128) NOT NULL'
									),
								'primary_key' => 'setting',
								);

$tables['tbl_temp'] = array(
							'cols_define' =>
							array(
								'id' => 'INT UNSIGNED NOT NULL AUTO_INCREMENT',
								'data' => 'TEXT NOT NULL',
								'ts' => 'INT UNSIGNED NOT NULL'
								),
							'primary_key' => 'id',
							);

$tables['tbl_users'] = array(
							'cols_define' =>
							array(
								'id' => 'INT UNSIGNED NOT NULL AUTO_INCREMENT',
								'username' => 'VARCHAR(32) NOT NULL',
								'password' => 'VARCHAR(64)',
								'hash_type' => 'VARCHAR(8)',
								'last_login' => 'INT UNSIGNED',
								'session_id' => 'VARCHAR(64)',
								'admin' => "TINYINT UNSIGNED DEFAULT '0' COMMENT 'Globals site admin'",
								'login_attempts' => "TINYINT UNSIGNED DEFAULT '0'"
								),
							'primary_key' => 'id',
							);

$tables['tbl_users_data'] = array(
								'cols_define' =>
								array(
									'id' => 'INT UNSIGNED NOT NULL AUTO_INCREMENT',
									'user_id' => 'INT UNSIGNED NOT NULL',
									'data' => 'TEXT'
									),
								'primary_key' => 'id',
								);
?>