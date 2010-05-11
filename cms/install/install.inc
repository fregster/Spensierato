<?php
/*	This file needs to setup a default database

	Method

	Check the settings file does not exsist
	Check that the settings file can be created

	Connect to DB
		prompte for username, password and host (default localhost)

	Create the database
		prompte for name

	Create random table prefix
		auto and ask for prompt to change

	Create the tables
		auto

	Create a read only user
		prompte for name creating a random password

	Create a read / write user
		prompte for name creating a random password

	Write settings file out
*/

ob_start();
//$settings_file = Settings::singleton()->get_setting('cms_root').'/../spensierato_setup.inc';
$path_vars = Common::get_path_vars();
if($_SERVER['REQUEST_METHOD'] == 'GET' && !isset($path_vars[0]))
{
	//Check that the settings file does not exsist
//	if(is_file($settings_file))
//	{
//		die('The site has been setup delete the settings file to start again');
//	}

	require_once('dependancycheck.inc');
	require_once('install_form.inc');
	exit;
}

if($_SERVER['REQUEST_METHOD'] == 'POST')
{
	$install_folder = dirname(__FILE__);
	$site_root = file_root();

	//SQL connection settings
	$username = post::variable('username');
	$password = post::variable('password');
	$host = post::variable('host');
	$driver =  post::variable('type');
	$layer = post::variable('db-layer');

	//Database to create
	$database = post::variable('database');

	//Tables
	$prefix = post::variable('prefix');

	//Users
	$user_r = post::variable('r_user');
	$user_rw = post::variable('rw_user');
	$user_db_password = post::variable('db_password');

	//Options
	if(post::variable('rewrite')) { $rewrite = 'true'; }
	else { $rewrite = 'false'; }

	//define $GLOBALS required by the database layer
	if($driver == 'sqlite') { $database = $host; }
	$GLOBALS['database_driver'] = $driver;
	$GLOBALS['database_backend'] = $layer;
	$GLOBALS['database_user_readwrite'] = $user_rw;
	$GLOBALS['database_user_read'] = $user_r;
	$GLOBALS['database_password'] = $user_db_password;
	$GLOBALS['database_host'] = $host;
	$GLOBALS['database_name'] = $database;
	$GLOBALS['table_prefix'] = $prefix.'_';
											
	debug('Vars set');

	/*
	 * Create the db object
	 */	
	$db = database::custom_db_object(array('phptype'  => $driver,
		    								'username' => $username,
											'password' => $password,
											'hostspec' => $host),
											NULL,
											array('Manager'));
	debug('db connected');
	/*
	 * Create the database
	 */
	$db->dropDatabase($database);
	$db->createDatabase($database);

	debug('DB created');
	/**
	 * Add the db users with the appropriate permissions
	 * Read user select on all
	 * Write user select, update, insert, delete on all
	 */

	//TODO add grant sql

	//Replace the db object with a new one in the database
	$db = database::custom_db_object(array('phptype'  => $driver,
		    											'username' => $username,
													    'password' => $password,
													    'hostspec' => $host,
														'database' => $database),
														NULL,
														array('Manager'));

	debug('Connection replaced');
	//Include the file which processes the SQL code
	//Load aproprate sql file

	debug('Loading setup file');
	
	if(!is_file($install_folder.DS.'db'.DS.$driver.'.inc'))
	{
		die('<br/>Unable to load install sql file '.$install_folder.DS.'db'.DS.$driver.'.inc');
	}

	//Execute sql
	debug('Running...');
	include($install_folder.DS.'db'.DS.$driver.'.inc');

	debug('Createing users and permissions');
	if($create_db_users)
	{
		//Create the DB users and give permissions over the database
		//TODO allow connections from specified places
		$user_create_sql = array('GRANT SELECT, INSERT, UPDATE, DELETE ON '.$grant_db.'	TO '.$grant_user_rw,
							 'GRANT SELECT ON '.$grant_db.'	TO '.$grant_user_r);
		
		foreach($user_create_sql as $sql)
		{
			$result = $db->query($sql);
	
			if(!$result)
			{
				die('An error occured while attempting to create database user.<br />
					The SQL statement associated with this error: '.$sql);
			}
		}
	}
	else
	{
		echo (' - Database users not required for this install');
	}

	debug('Createing Homepage');
	database::Singleton()->force_write_user();
	
	$id = database::Singleton()->sql_insert('tbl_pages', array('parent_id', 'created', 'creator', 'visible', 'title'), array(0, time(), 0, 1, 'Homepage'));
	
	debug('Adding starting element');
	$element = Element::type('elementText');
	$element->create($id, false);

	debug('Adding site administrator');
	$password = user::Singleton()->create(post::variable('user_username'), post::variable('user_email'), post::variable('user_password'), true);

	//set the path for the settings file
	$settings_file = $site_root.'/spensierato_setup.inc';

	//detect the server OS and set the doc root accordingly.
	if (stripos(PHP_OS, 'darwin') !== FALSE) {
		$os = 'mac';
		$doc_root_prefix = ''; //OSX has a unix-type folder structure
	}
	else if(stripos(PHP_OS, 'win') !== FALSE){
		$os = 'windows';
		$doc_root_prefix = $GLOBALS['site_root'].'/cms/';
	}
	else{
		$os = 'unix';
		$doc_root_prefix = '';
	}

	/**
	* Build the settings file string
	*/
	$settings_file_string = '<?php
//This file contains the CMS settings required before we include the CMS code.

//Database vars
//The DB layer suports 2 users read and readwrite
//readwrite is used once logged in to provide another layer of prtection against sql injection.

$GLOBALS[\'database_backend\'] = \''.$layer.'\'; //can be mdb2 (Default), pdo or php
$GLOBALS[\'database_host\'] = \''. $host .'\'; //DB Host, prob localhost or 127.0.0.1
$GLOBALS[\'database_driver\'] = \''. $driver .'\'; //DB connection driver, for mysql 4+ use mysqli
$GLOBALS[\'database_name\'] = \''. $database .'\'; //DB name
$GLOBALS[\'database_user_read\'] = \''. $user_r .'\'; //A standard db user with SELECT only!
$GLOBALS[\'database_user_readwrite\'] = \''. $user_rw .'\'; //A db user with SELECT, UPDATE, REPLACE and DELETE permissions, this user is only used when required to help prevent 0 day exploits
$GLOBALS[\'database_password\'] = \''. $user_db_password .'\'; //A nice random string for the DB password
$GLOBALS[\'table_prefix\'] = \''. $prefix .'_\'; //A random table prefix to help prevent 0 day exploits
$GLOBALS[\'allow_rewrite\'] = '. $rewrite.'; //Apache + mod rewrite gives clean URL\'s anything else is untested
$GLOBALS[\'random_key\'] = \''.post::variable('random_key').'\'; //A random key used for generating more secure hash\'s
$GLOBALS[\'server_os\'] = \''.$os.'\'; //Should be unix, windows or osx


//Settings for email
$GLOBALS[\'smtp_server\'] = \''. post::variable('smtp_host').'\'; //Where should email submissions be sent
$GLOBALS[\'smtp_port\'] = '. post::variable('smtp_port').';
$GLOBALS[\'smtp_username\'] = '. post::variable('smtp_username').';
$GLOBALS[\'smtp_password\'] = '. post::variable('smtp_password').';

//Paths to folders in the filesystem
$GLOBALS[\'cms_folder_uploads\'] = \''.$doc_root_prefix.post::variable('cms_folder_uploads').'\';
$GLOBALS[\'cms_folder_downloads\'] = \''.$doc_root_prefix.post::variable('cms_folder_downloads').'\';
$GLOBALS[\'cms_folder_orig_images\'] = \''.$doc_root_prefix.post::variable('cms_folder_orig_images').'\';
$GLOBALS[\'cms_folder_logs\'] = \''.$doc_root_prefix.post::variable('cms_folder_logs').'\';
$GLOBALS[\'cms_folder_cache\'] = \''.$doc_root_prefix.post::variable('cms_folder_cache').'\';
';

	/**
	* Write the settings file
	*/
	$write_result = @file_put_contents($settings_file, $settings_file_string);

	if(!$write_result)
	{
		echo('<p>Unable to create settings file ('.$settings_file.'), instalation aborted.<br/>
				Make sure the root dir is writeable or the file spensierato_setup.inc is created and writeable</p>');
		debug($settings_file_string);
		exit;
	}
	else
		debug('Settings file created');

	debug('The site is now setup<br/><strong>Note: If you manual set the install directive you will need to remove it now!</strong><br />You can log in with the user '.$username.' and the password '.$password.'<br/>Click <a href="'.$GLOBALS['document_root'].'">here</a> to login');
	exit;
}

function debug($text)
{
	$debug = true;
	if($debug){ echo('<br />'.$text); }
}