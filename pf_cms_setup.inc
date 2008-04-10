<?
//This file contains the CMS settings required before we include the CMS code.

//Database vars
//The DB layer suports 2 users read and readwrite
//readwrite is used once logged in to provide another layer of prtection against sql injection.

$GLOBALS['database_host'] = 'localhost';
$GLOBALS['database_driver'] = 'mysqli';
$GLOBALS['database_name'] = 'nuneaton';
$GLOBALS['database_user_read'] = 'nuneaton_read';
$GLOBALS['database_user_readwrite'] = 'nuneaton_write';
$GLOBALS['database_password'] = 'SBu8vPYdAm3F,c2e';

?>