<?php
//Cache the file system path for performance
define('DATABASE_LAYER_PATH', dirname(__FILE__));

if(!defined('DS'))
{
	define('DS', DIRECTORY_SEPARATOR);
}

//Set the database driver, if it's not set load it from the array of avaiable drivers
if(!isset($GLOBALS['database_driver'])) { $GLOBALS['database_driver'] = $db_drivers[0]; }
define('DATABASE_DRIVER', $GLOBALS['database_driver']);

//Set the database backend, if it's not set load it from the array of avaiable backends
if(!isset($GLOBALS['database_backend'])) { $GLOBALS['database_backend'] = $db_backends[0]; }
define('DATABASE_BACKEND' , $GLOBALS['database_backend']);

//Set the database table prefic, if it's not set load it blank
if(!isset($GLOBALS['table_prefix'])) { $GLOBALS['table_prefix'] = ''; }
define('DATABASE_TABLE_PREFIX' , $GLOBALS['table_prefix']);

if((isset($install) && $install) || isset($force_install))
{ 
	if(!defined('DATABASE_INSTALL_MODE')){ define('DATABASE_INSTALL_MODE', true); }
}