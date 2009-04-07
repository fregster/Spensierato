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

//$settings_file = Settings::singleton()->get_setting('cms_root').'/../spensierato_setup.inc';

if($_SERVER['REQUEST_METHOD'] == 'GET')
{
	//Check that the settings file does not exsist
//	if(is_file($settings_file))
//	{
//		die('The site has been setup delete the settings file to start again');
//	}
	require_once('dependancycheck.inc');
	require_once('install_form.inc');
}

if($_SERVER['REQUEST_METHOD'] == 'POST')
{
	$install_folder = dirname(__FILE__);

	//SQL connection settings
	$username = post::variable('username');
	$password = post::variable('password');
	$host = post::variable('host');
	$driver =  post::variable('type');

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

	if(!is_file($install_folder.'/'.$driver.'.inc'))
	{
		die('<br/>Unable to load install sql file '.$install_folder.'/'.$driver.'.inc');
	}

	debug('Loading setup file');
	include($install_folder.'/'.$driver.'.inc');

	//Execute sql
	debug('Running...');

	$i = 0;
	foreach($sql as $sql_str)
	{
		//Process sql
		$sql_str = str_replace('cms_', $prefix.'_', $sql_str);
		$resultset = $db->query($sql_str);

		if(PEAR::isError($resultset))
		{
			$error = '<br/>Failed to issue query '.$i.', error message : ' . $resultset->getMessage().'<br />From : '. $sql_str;
			die($error);
		}
		++$i;
	}

	//Create the DB users and give permissions over the database
	//TODO allow connections from specified places
	$user_create_sql = array('GRANT SELECT, INSERT, UPDATE, DELETE ON '.$database.'.*
									TO \''.$user_rw. '\'@\'localhost\' IDENTIFIED BY \''.$user_db_password.'\'',
							 'GRANT SELECT ON '.$database.'.*
							 		TO \''. $user_r. '\'@\'localhost\' IDENTIFIED BY \''.$user_db_password.'\'');

	debug('Createing users and permissions');
	foreach($user_create_sql as $sql)
	{
		$resultset = $db->query($sql);

		if(PEAR::isError($resultset))
		{
			$error = '<br/>Failed to issue query, error message : ' . $resultset->getMessage().'<br />From :'. $sql;
			die($error);
		}
	}

	//define $GLOBALS required by the force_write_user(), sql_insert() methods
	$GLOBALS['database_driver'] = $driver;
	$GLOBALS['database_user_readwrite'] = $user_rw;
	$GLOBALS['database_user_read'] = $user_r;
	$GLOBALS['database_password'] = $user_db_password;
	$GLOBALS['database_host'] = $host;
	$GLOBALS['database_name'] = $database;
	$GLOBALS['table_prefix'] = $prefix.'_';

	debug('Createing Homepage');
	database::Singleton()->force_write_user();
	$id = database::Singleton()->sql_insert('tbl_pages', array('parent_id', 'created', 'creator', 'visible', 'title'), array(0, time(), 0, 1, 'Homepage'));

	debug('Adding starting element');
	$element = Element::type('elementText');
	$element->create($id, false);

	debug('Adding site administrator');
	$password = user::Singleton()->create(post::variable('user_username'), post::variable('user_email'), post::variable('user_password'), true);

	//set the path for the settings file
	$settings_file = $install_folder.'/../../spensierato_setup.inc';

	//detect the server OS and set the doc root accordingly.
	if(stripos(PHP_OS, 'win') !== FALSE){
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

$GLOBALS[\'database_host\'] = \''. $host .'\';
$GLOBALS[\'database_driver\'] = \''. $driver .'\';
$GLOBALS[\'database_name\'] = \''. $database .'\';
$GLOBALS[\'database_user_read\'] = \''. $user_r .'\';
$GLOBALS[\'database_user_readwrite\'] = \''. $user_rw .'\';
$GLOBALS[\'database_password\'] = \''. $user_db_password .'\';
$GLOBALS[\'table_prefix\'] = \''. $prefix .'_\';
$GLOBALS[\'allow_rewrite\'] = '. $rewrite.';
$GLOBALS[\'cms_folder_uploads\'] = \''.$doc_root_prefix.post::variable('cms_folder_uploads').'\';
$GLOBALS[\'cms_folder_downloads\'] = \''.$doc_root_prefix.post::variable('cms_folder_downloads').'\';
$GLOBALS[\'cms_folder_orig_images\'] = \''.$doc_root_prefix.post::variable('cms_folder_orig_images').'\';
$GLOBALS[\'cms_folder_logs\'] = \''.$doc_root_prefix.post::variable('cms_folder_logs').'\';
$GLOBALS[\'cms_folder_cache\'] = \''.$doc_root_prefix.post::variable('cms_folder_cache').'\';
$GLOBALS[\'random_key\'] = \''.post::variable('random_key').'\';
$GLOBALS[\'server_os\'] = \''.$os.'\';
?>';

	/**
	* Write the settings file
	*/
	$write_result = file_put_contents($settings_file, $settings_file_string);

	if(!$write_result)
		die('Unable to create settings file ('.$settings_file.'), instalation aborted.<br/>
				Make sure the root dir is writeable or the file spensierato_setup.inc is created and writeable');
	else
		debug('Settings file created');

	debug('The site is now setup<br/><strong>Note: If you manual set the install directive you will need to remove it now!</strong><br />You can log in with the user admin and the password '.$password.'<br/>Click <a href="'.$GLOBALS['document_root'].'">here</a> to login');
}

function debug($text)
{
	$debug = true;
	if($debug){ echo('<br />'.$text); }
}
?>