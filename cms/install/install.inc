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

//The installer can generate severl error's depending on the system setup, most are caught but for readability incase any are missed tuen displaying error's off.
ini_set('display_errors', 1);
ob_start();
$install = true;
if(!defined('CACHE_TYPE')){
	DEFINE('CACHE_TYPE', 'off');
}
//$settings_file = Settings::singleton()->get_setting('cms_root').'/../spensierato_setup.inc';
$path_vars = Common::get_path_vars();
if($_SERVER['REQUEST_METHOD'] == 'GET')
{
	if(isset($path_vars[0]) && $path_vars[0] == 'skin' && isset($path_vars[1]))
	{
		//ini_set('display_errors', 'off');
		$file = DIR_SKINS.'default'.DS.'graphics'.DS.$path_vars[1];
		FileTransfer::singleton()->get_file($file, false, false, true, true);
		exit;
	}
	
	//Check that the settings file does not exsist
//	if(is_file($settings_file))
//	{
//		die('The site has been setup delete the settings file to start again');
//	}

	if(!isset($path_vars[0]))
	{
		require_once('dependancycheck.inc');
		require_once('install_form.inc');
		exit;	
	}
}

if($_SERVER['REQUEST_METHOD'] == 'POST')
{
	$doc_root_prefix = DIR_CONFIG;
	//detect the server OS and set the doc root accordingly.
	if (stripos(PHP_OS, 'darwin') !== FALSE) {
		$os = 'mac';
	}
	else if(stripos(PHP_OS, 'win') !== FALSE){
		$os = 'windows';
	}
	else{
		$os = 'unix';
	}
	
	if(!post::variable('cms_folder_use_custom'))
	{
		debug('Creating folders');
		if(!is_writable($doc_root_prefix.'writable'))
		{
			if(!isset($_GET['skip_folders']))
			{
				Debug('Unable to create folders, installation cancled. Ensure that '.$doc_root_prefix.'writable has been created and is writable');
				die();
			}
			debug('Unable to create folders, please create them manually after the install');
			debug($doc_root_prefix.'writable was not a writable folder' );
		}

		$dir_logs = $doc_root_prefix.'writable'.DS.'logs';
		$dir_uploads = $doc_root_prefix.'writable'.DS.'uploads';
		$dir_uploads_tmp = $doc_root_prefix.'writable'.DS.'uploads'.DS.'tmp';
		$dir_uploads_orig_images = $doc_root_prefix.'writable'.DS.'uploads'.DS.'original_images';
		$dir_cache = $doc_root_prefix.'writable'.DS.'cache';
		$dir_search = $doc_root_prefix.'writable'.DS.'search';
		
		@mkdir($dir_logs);
		@mkdir($dir_uploads);
		@mkdir($dir_uploads_tmp);
		@mkdir($dir_uploads_orig_images);
		@mkdir($dir_cache);
		@mkdir($dir_search);

	}
	else
	{
		debug('Setting custom folders');
		
		$dir_logs = realpath($doc_root_prefix.post::variable('cms_folder_logs'));
		$dir_uploads = realpath($doc_root_prefix.post::variable('cms_folder_downloads'));
		$dir_uploads_tmp = realpath($doc_root_prefix.post::variable('cms_folder_uploads'));
		$dir_uploads_orig_images = realpath($doc_root_prefix.post::variable('cms_folder_orig_images'));
		$dir_cache = realpath($doc_root_prefix.post::variable('cms_folder_cache'));
		$dir_search = realpath($doc_root_prefix.post::variable('cms_folder_search'));
	}
	
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
	$user_db_password_write = post::variable('db_password_write');

	//Options
	if(post::variable('rewrite')) { $rewrite = 'true'; }
	else { $rewrite = 'false'; }

	//define $GLOBALS required by the database layer
	if($driver == 'sqlite') { @mkdir( $doc_root_prefix.'writable'.DS.'database'); $host = $doc_root_prefix.'writable'.DS.'database'.DS.'cms_db.sqlite'; $database = $host; }
	$GLOBALS['database_driver'] = $driver;
	$GLOBALS['database_backend'] = $layer;
	$GLOBALS['database_user_readwrite'] = $user_rw;
	$GLOBALS['database_user_read'] = $user_r;
	$GLOBALS['database_password'] = $user_db_password;
	$GLOBALS['database_password_write'] = $user_db_password_write;
	$GLOBALS['database_host_read'] = $host;
	$GLOBALS['database_host_write'] = $host;
	$GLOBALS['database_name'] = $database;
	$GLOBALS['table_prefix'] = $prefix.'_';

	debug('Vars set ('.$GLOBALS['table_prefix'].')');
	
	require_once(INCLUDE_PATH.DIRECTORY_SEPARATOR.'cms'.DIRECTORY_SEPARATOR.'database'.DIRECTORY_SEPARATOR.'database.inc');
	$install_folder = dirname(__FILE__);

	//set the path for the settings file
	$settings_file = DIR_CONFIG.'spensierato_setup.inc';
	
	/*
	 * Create the db object
	 */

	$db = database::factory(array(	'phptype'  => $driver,
									'backend'	=> $layer,
		    						'username' => $username,
									'password' => $password,
									'hostspec' => $host,
									'driver' => $driver,
										),
							NULL,
							array('Manager'));
	
	debug('DB Connected');

	/*
	 * Create the database
	 */	
	debug('Forcing write user to be created');
	$db->force_write_user();
	debug('Attepmting to drop DB');
	$db->drop_database($database);
	
	debug('DB Dropped');
	
	debug('Attepmting to create DB');
	$db->create_database($database);
	debug('DB Created');
	
	debug('Loading required settings');
	//A random key is required in settings for creating more secure password hashes
	Settings::singleton()->new_setting('random_key',post::variable('random_key'));
	Settings::singleton()->new_setting('site_option_hash_type',max_sha_hash());
	Settings::singleton()->new_setting('user_min_password_length', 10);
	
	//Replace the db object with a new one in the database
	if(method_exists($db, 'destroy'))
	{
		$db->destroy();
	}
	unset($db);

	debug('Building DB Singleton');
	var_dump(database::Singleton());
	$dsn = array('phptype'  => $driver,
							'backend'	=> $layer,
							'username' => $username,
							'password' => $password,
							'hostspec' => $host,
							'database' => $database);
	var_dump($dsn);
	database::Singleton($dsn, true, $dsn);
	unset($dsn);
	
	var_dump(database::Singleton());
							 
	debug('DB Singleton Created');
	
	//Include the file which processes the SQL code
	//Load aproprate sql file
	debug('Loading setup file');
	
	if(!is_file($install_folder.DS.'db'.DS.$driver.'.inc'))
	{
		die('<br/>Unable to load install sql file '.$install_folder.DS.'db'.DS.$driver.'.inc');
	}

	//Execute sql
	debug('Running SQL...');

	include($install_folder.DS.'db'.DS.$driver.'.inc');

	/**
	 * Add the db users with the appropriate permissions
	 * Read user select on all
	 * Write user select, update, insert, delete on all
	 */
	debug('Createing users and permissions');
	if($create_db_users && is_array($user_create_sql))
	{
		//Create the DB users and give permissions over the database
		//TODO allow connections from specified places
		
		foreach($user_create_sql as $sql)
		{
//			echo('<br/>'.$sql.'<br/>');
			$result = database::Singleton()->direct_query($sql);
	
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
	
	debug('Rebuilding Database Singleton');
	database::Singleton(null, true); //This forces the singleton to be replaced using the default values it ensures that the autoloader will work at any point in time

	/**
	* Build the settings file string
	*/
	$settings_file_string = '<?php
//This file contains the CMS settings required before we include the CMS code.

//Database vars
//The DB layer suports 2 users read and readwrite
//readwrite is used once logged in to provide another layer of prtection against sql injection.

$GLOBALS[\'database_backend\'] = \''.$layer.'\'; //can be mdb2 (Default), pdo or php
$GLOBALS[\'database_host_read\'] = \''. $host .'\'; //DB Host, prob localhost or 127.0.0.1
$GLOBALS[\'database_host_write\'] = \''. $host .'\'; //DB Host, prob localhost or 127.0.0.1
$GLOBALS[\'database_driver\'] = \''. $driver .'\'; //DB connection driver, for mysql 4+ use mysqli
$GLOBALS[\'database_name\'] = \''. $database .'\'; //DB name
$GLOBALS[\'database_user_read\'] = \''. $user_r .'\'; //A standard db user with SELECT only!
$GLOBALS[\'database_user_readwrite\'] = \''. $user_rw .'\'; //A db user with SELECT, UPDATE, REPLACE and DELETE permissions, this user is only used when required to help prevent 0 day exploits
$GLOBALS[\'database_password\'] = \''. $user_db_password .'\'; //A nice random string for the DB password
$GLOBALS[\'database_password_write\'] = \''. $user_db_password_write .'\'; //A nice random string for the DB password
$GLOBALS[\'table_prefix\'] = \''. $prefix .'_\'; //A random table prefix to help prevent 0 day exploits
$GLOBALS[\'allow_rewrite\'] = '. $rewrite.'; //Apache + mod rewrite gives clean URL\'s anything else is untested
$GLOBALS[\'random_key\'] = \''.post::variable('random_key').'\'; //A random key used for generating more secure hash\'s
$GLOBALS[\'server_os\'] = \''.$os.'\'; //Should be unix, windows or osx
$GLOBALS[\'session_handler\'] = \'files\';

//Default options
$GLOBALS[\'max_sha_hash\'] = \''.max_sha_hash().'\'; //Max SHA hash type supported at install time

//Settings for email
$GLOBALS[\'smtp_server\'] = \''. post::variable('smtp_host').'\'; //Where should email submissions be sent
$GLOBALS[\'smtp_port\'] = '. post::variable('smtp_port').';
$GLOBALS[\'smtp_username\'] = '. post::variable('smtp_username').';
$GLOBALS[\'smtp_password\'] = '. post::variable('smtp_password').';

//Paths to folders in the filesystem
$GLOBALS[\'cms_folder_uploads\'] = \''.realpath($dir_uploads_tmp).'\';
$GLOBALS[\'cms_folder_downloads\'] = \''.realpath($dir_uploads).'\';
$GLOBALS[\'cms_folder_orig_images\'] = \''.realpath($dir_uploads_orig_images).'\';
$GLOBALS[\'cms_folder_logs\'] = \''.realpath($dir_logs).'\';
$GLOBALS[\'cms_folder_cache\'] = \''.realpath($dir_cache).'\';
$GLOBALS[\'cms_folder_search\'] = \''.realpath($dir_search).'\';
';

	/**
	* Write the settings file
	*/
	$write_result = @file_put_contents($settings_file, $settings_file_string);

	if(!$write_result)
	{
		echo('<p>Unable to create settings file ('.$settings_file.'), instalation aborted.<br/>
				Make sure the root dir is writable or the file spensierato_setup.inc is created and writable</p>');
		debug($settings_file_string);
		exit;
	}
	else
	{
		debug('Settings file created');
		
		if(!masterpassword::Singleton()->set_password(true) || !masterpassword::Singleton()->set_password_secondary(true))
		{
			debug('<strong>Random Encryption key file output failed. Fix folder permissions and re run the installer</strong>');
			exit;
		}

		debug('Random Encryption key files created');
		
		debug('Adding site administrator');
		database::Singleton()->force_write_user();
		$password = user::Singleton()->create(post::variable('user_username'), post::variable('user_email'), post::variable('user_password'), true, false);
		
		debug('Createing Homepage');		
		$id = database::Singleton()->sql_insert('tbl_pages', array('parent_id', 'created', 'creator', 'visible', 'title'), array(0, time(), 0, 1, 'Homepage'));
		
		debug('Adding starting element');
		$element = Element::type('elementText');
		$element->create($id, false);
		
		debug('Copying HTAcess file if required');
		if(!file_exists(DIR_INDEX.'.htaccess')){
			if(!@copy(INCLUDE_PATH.'example.htaccess', DIR_INDEX.'.htaccess')){
				debug('Failed to write HTAcess out');
			}
		}
		
		debug('<br/>Write permission should <strong>be removed</strong> from the following files and only readable by the required users</br/>');

		debug('Testing database now has content');
		
		if(!database::Singleton()->sql_select_first(null, 'tbl_pages')){
			die('Did not get any data from the database, its creation must have failed');
		}

		debug('The site is now setup<br/><strong>Note: If you manual set the install directive you will need to remove it now!</strong><br />You can log in with the user '.$username.' and the password '.$password.'<br/>Click <a href="'.DOCUMENT_ROOT.'">here</a> to login');
		exit;
	}
}

function debug($text)
{
	$debug = true;
	if($debug){ echo('<br />'.$text.' '); }
}

exit;