<?php
$backends = array('mdb2', 'pdo', 'php'); //PHP is mysql only
$drivers = array('mysqli', 'mysql', 'mssql');

//Cache the file system path for performance
define('DATABASE_LAYER_PATH', dirname(__FILE__));

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
require_once(DATABASE_LAYER_PATH.'/backends/database_execution.inc');

//Load the main database API
require_once(DATABASE_LAYER_PATH.'/layer.inc');
?>