<?
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

	require_once('install_form.inc');
}

if($_SERVER['REQUEST_METHOD'] == 'POST')
{
	$install_folder = dirname(__FILE__);
	$settings_file = $install_folder.'/../../spensierato_setup.inc';
	/*
	 * check the file
	 * load the settings
	 */
//	$settings_file_handle = fopen($settings_file, 'w+') or die('can\'t open file you may need to give the webserver temporay permissions over the install folder or create the setup file manual with write permissions for the webserver');
	$settings_file_handle = fopen($settings_file, 'w+');
	if(!$settings_file_handle)
	{
		die('Unable to create settings file ('.$settings_file.'), instalation aborted.<br/>Make sure the root dir is writeable or the file spensierato_setup.inc is created and writeable');
	}
	
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
			$error = 'Failed to issue query '.$i.', error message : ' . $resultset->getMessage().'<br />From :'. $sql_str;
			debug($error);
		}
		++$i;
	}
	
	//Create the DB users and give permissions over the database
	//TODO allow connections from specified places
	$user_create_sql = array('CREATE USER \''.$user_rw.'\'@\'localhost\' IDENTIFIED BY \''.$user_db_password.'\'',
	'GRANT SELECT, INSERT, UPDATE, DELETE ON '.$database.'.* TO \''.$user_rw. '\'@\'localhost\'',
	'CREATE USER \''.$user_r.'\'@\'localhost\' IDENTIFIED BY \''.$user_db_password.'\'',
	'GRANT SELECT ON '.$database.'.* TO \''. $user_r. '\'@\'localhost\'');
	
	debug('Createing users and permissions');
	foreach($user_create_sql as $sql)
	{
		$resultset = $db->query($sql);
	
		if(PEAR::isError($resultset)) 
		{
			$error = 'Failed to issue query, error message : ' . $resultset->getMessage().'<br />From :'. $sql;
			debug($error);
		}
	}
	
	/*
	 * Build the settings file string
	 */
	$settings_file_string = '<?
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
?>';
	
	/*
	 * Close and write the settings file
	 */
	fwrite($settings_file_handle, $settings_file_string);
	fclose($settings_file_handle);
	debug('Settings file created');
	
	debug('Createing Homepage');
	database::Singleton()->force_write_user();
	$id = database::Singleton()->sql_insert('tbl_pages', array('parent_id', 'created', 'creator', 'visible', 'title'), array(0, time(), 0, 1, 'Homepage'));
	
	debug('Adding starting element');
	$element = Element::type('elementText');
	$element->create($id, false);
	
	debug('Adding site administrator');
	$password = user::Singleton()->create('admin');
	
	debug('The site is now setup reload this page to see your new site<br />You can log in with the user admin and the password '.$password);
}

function debug($text)
{
	$debug = true;
	if($debug){ echo('<br />'.$text); }
}
?>