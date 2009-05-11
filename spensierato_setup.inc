<?php
//This file contains the CMS settings required before we include the CMS code.

//Database vars
//The DB layer suports 2 users read and readwrite
//readwrite is used once logged in to provide another layer of prtection against sql injection.

$GLOBALS['database_backend'] = 'mdb2'; //can be mdb2 (Default), pdo or php
$GLOBALS['database_host'] = 'localhost'; //DB Host, prob localhost or 127.0.0.1
$GLOBALS['database_driver'] = 'mysqli'; //DB connection driver, for mysql 4+ use mysqli
$GLOBALS['database_name'] = 'spensierato'; //DB name
$GLOBALS['database_user_read'] = 'spensierato_read'; //A standard db user with SELECT only!
$GLOBALS['database_user_readwrite'] = 'spensierato_writ'; //A db user with SELECT, UPDATE, REPLACE and DELETE permissions, this user is only used when required to help prevent 0 day exploits
$GLOBALS['database_password'] = 'jqyqfxwcfrvwrvqq'; //A nice random string for the DB password
$GLOBALS['table_prefix'] = 'tfk_'; //A random table prefix to help prevent 0 day exploits
$GLOBALS['allow_rewrite'] = true; //Apache + mod rewrite gives clean URL's anything else is untested
$GLOBALS['random_key'] = 'oknbvfde34567890o'; //A random key used for generating more secure hash's
$GLOBALS['server_os'] = 'unix'; //Should be unix, windows or osx

//Paths to folders in the filesystem
$GLOBALS['cms_folder_uploads'] = 'writeable/uploads/tmp/';
$GLOBALS['cms_folder_downloads'] = 'writeable/uploads/';
$GLOBALS['cms_folder_orig_images'] = 'writeable/uploads/original_images/';
$GLOBALS['cms_folder_logs'] = 'writeable/logs/';
$GLOBALS['cms_folder_cache'] = 'writeable/cache/';
?>