<?php
//This file contains the CMS settings required before we include the CMS code.

//Database vars
//The DB layer suports 2 users read and readwrite
//readwrite is used once logged in to provide another layer of prtection against sql injection.

$GLOBALS['database_host'] = 'localhost';
$GLOBALS['database_driver'] = 'mysqli';
$GLOBALS['database_name'] = 'spensierato';
$GLOBALS['database_user_read'] = 'spensierato_read';
$GLOBALS['database_user_readwrite'] = 'spensierato_writ';
$GLOBALS['database_password'] = 'yyspfvgmrcmcmxzm';
$GLOBALS['table_prefix'] = 'tfk_';
$GLOBALS['allow_rewrite'] = true;
$GLOBALS['random_key'] = 'oknbvfde34567890o';
$GLOBALS['cms_folder_uploads'] = '/mnt/share/www/workspace/Spensierato/cms/writeable/uploads/tmp/';
$GLOBALS['cms_folder_downloads'] = '/mnt/share/www/workspace/Spensierato/cms/writeable/uploads/';
$GLOBALS['cms_folder_orig_images'] = '/mnt/share/www/workspace/Spensierato/cms/writeable/uploads/original_images/';
$GLOBALS['cms_folder_logs'] = '/mnt/share/www/workspace/Spensierato/cms/writeable/logs/';
$GLOBALS['cms_folder_cache'] = '/mnt/share/www/workspace/Spensierato/cms/writeable/cache/';
?>