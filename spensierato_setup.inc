<?php
//This file contains the CMS settings required before we include the CMS code.

//Database vars
//The DB layer suports 2 users read and readwrite
//readwrite is used once logged in to provide another layer of prtection against sql injection.

$GLOBALS['database_backend'] = 'pdo'; //can be mdb2 (Default), pdo or php
$GLOBALS['database_host'] = 'localhost'; //DB Host, prob localhost or 127.0.0.1
$GLOBALS['database_driver'] = 'mysqli'; //DB connection driver, for mysql 4+ use mysqli
$GLOBALS['database_name'] = 'spensierato'; //DB name
$GLOBALS['database_user_read'] = 'spensierato_read'; //A standard db user with SELECT only!
$GLOBALS['database_user_readwrite'] = 'spensierato_writ'; //A db user with SELECT, UPDATE, REPLACE and DELETE permissions, this user is only used when required to help prevent 0 day exploits
$GLOBALS['database_password'] = 'fsaaxwgnzyarjkhf'; //A nice random string for the DB password
$GLOBALS['database_password_write'] = 'fsaaxwgnzyarjkhf'; //A nice random string for the DB password
$GLOBALS['table_prefix'] = 'cms_'; //A random table prefix to help prevent 0 day exploits
$GLOBALS['allow_rewrite'] = true; //Apache + mod rewrite gives clean URL's anything else is untested
$GLOBALS['random_key'] = '2fe8b327381ba67d770d22fddff246dca71f0b4e'; //A random key used for generating more secure hash's
$GLOBALS['server_os'] = 'unix'; //Should be unix, windows or osx
$GLOBALS['max_sha_hash'] = 'SHA256';
$GLOBALS['session_handler'] = 'memcached';


//Settings for email
$GLOBALS['smtp_server'] = 'localhost'; //Where should email submissions be sent
$GLOBALS['smtp_port'] = 25;
$GLOBALS['smtp_username'] = null;
$GLOBALS['smtp_password'] = null;

//Paths to folders in the filesystem
$GLOBALS['cms_folder_uploads'] = '/mnt/writable/uploads/tmp/';
$GLOBALS['cms_folder_downloads'] = '/mnt/writable/uploads/';
$GLOBALS['cms_folder_orig_images'] = '/mnt/writable/uploads/original_images/';
$GLOBALS['cms_folder_logs'] = '/mnt/writable/logs/';
$GLOBALS['cms_folder_cache'] = '/mnt/writable/cache/';
$GLOBALS['cms_folder_search'] = '/mnt/writable/search/';
