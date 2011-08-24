<?php
require_once('mysql.inc');

//FIXME: Should be redundent now
$GLOBALS['database_mode'] = '0644';
$GLOBALS['database_password'] = null;
$GLOBALS['database_user_readwrite'] = null;
$GLOBALS['database_user_read'] = null;


class Database_sqlite extends Database_mysql
{
	protected $table_quote = '"';
	protected $col_quote = '"';
	protected $terminate_sql = ';';
	protected $allow_extenal_escaping = true; //Force to true
	protected $escape_method = 'sqlite_escape_string';
	protected $sql_use_prepared_statments_if_available = false;
	protected $user_support = false;
	
}