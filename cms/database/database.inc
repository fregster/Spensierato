<?php
$backends = array('mdb2', 'pdo', 'php'); //PHP is mysql, sqlite only
$drivers = array('mysqli', 'mysql', 'mssql', 'sqlite');

//Cache the file system path for performance
define('DATABASE_LAYER_PATH', dirname(__FILE__));

if(!defined('DS'))
{
	define('DS', DIRECTORY_SEPARATOR);
}

//Set the database driver, if it's not set load it from the array of avaiable drivers
if(!isset($GLOBALS['database_driver'])) { $GLOBALS['database_driver'] = $drivers[0]; }
define('DATABASE_DRIVER', $GLOBALS['database_driver']);

//Set the database backend, if it's not set load it from the array of avaiable backends
if(!isset($GLOBALS['database_backend'])) { $GLOBALS['database_backend'] = $backends[0]; }
define('DATABASE_BACKEND' , $GLOBALS['database_backend']);

//Set the database table prefic, if it's not set load it blank
if(!isset($GLOBALS['table_prefix'])) { $GLOBALS['table_prefix'] = ''; }
define('DATABASE_TABLE_PREFIX' , $GLOBALS['table_prefix']);

//Load the required files for the database layer
require_once(DATABASE_LAYER_PATH.DS.'backends'.DS.'database_execution.inc');

//Load the main database API
require_once(DATABASE_LAYER_PATH.DS.'layer.inc');