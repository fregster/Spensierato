<?php
/**
 * 
 * With PHP 5.3 the SQL driver from Microsoft has been renamed to SqlSrv and PDO_SqlSrv so this is just a proxy class really for us simple people
 * @author paulfryer
 *
 */

require_once('mssql.inc');
class Database_sqlsrv extends Database_mssql
{
	protected $user_support = false;
	protected $table_quote = '';
	protected $col_quote = '';
	protected $terminate_sql = ';';
	protected $allow_extenal_escaping = true; //Force to true
	protected $sql_use_prepared_statments_if_available = false;
}