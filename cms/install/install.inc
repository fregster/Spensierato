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

?>
<form method="POST" action="">
	<div id="dbinfo">
	Please specify the connection information to your SQL server, this user needs to be able to create users and databases, as well as give permissions (Usally root or sa)
		<div>Host: <input type="text" name="host" value="localhost" /></div>
		<div>MDB2 Host type: <input type="text" name="type" value="mysqli" /></div>
		<div>Username: <input type="text" name="username" value="root" /></div>
		<div>Password: <input type="password" name="password" /></div>
	</div>

	<div id="database">
		Database to connect to or create
		<div>Database name: <input type="text" name="database" value="spensierato" /></div>
	</div>

	<div id="prefix">
		If your connecting to an exsisting database this must match the origonal table prefix
		<div>Table Prefix name: <input type="text" name="prefix" value="<? echo(Common::generate_random_string(3)); ?>" /></div>
	</div>
	
	<div id="randomkey">
		Create a random key. This is used when generating hashes to help preventing people work out the final hash it defaults to a sha1 hash of the server microtime when you install.
		<div>Random Key: <input type="text" name="random_key" value="<? echo(sha1(microtime())); ?>" /></div>
	</div>
	
	<div id="users">
		If no usernames are set then one will be created and a random password will be generated (Recomended)<br />
		If the password set is does not meet the recomended length an random password will also be created
		<div>Read only username: <input type="text" name="r_user" value="spensierato_read" /></div>
		<div>Read Write username: <input type="text" name="rw_user" value="spensierato_writ" /></div>
		<div>Users password: <input type="password" name="db_password" value="<? echo(Common::generate_random_string(16)); ?>" /></div>
	</div>
	
	<input type="submit" value="Create Site" />
</form>
<?
}

if($_SERVER['REQUEST_METHOD'] == 'POST')
{
	/*
	 * check the file
	 * load the settings
	 */
//	$settings_file_handle = fopen($settings_file, 'w+') or die('can\'t open file you may need to give the webserver temporay permissions over the install folder or create the setup file manual with write permissions for the webserver');
	$settings_file_handle = fopen($settings_file, 'w+');
	
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

	if(!is_file('/mnt/www/workspace/Spensierato/cms/install/'.$driver.'.txt'))
	{
		die('<br/>Unable to load install sql file '.$driver.'.txt');
	}
	
	debug('Loading setup file');
	$sql = file_get_contents('/mnt/www/workspace/Spensierato/cms/install/'.$driver.'.txt');
	
	//Process sql
	$sql = str_replace('cms_', $prefix.'_', $sql);
	
	
	//Execute sql
	debug('Running...');
	
//	$link = mysql_connect($host.':3307', $username, $password);
//	if (!$link) {
//	    die('Could not connect: ' . mysql_error());
//	}
//	echo 'Connected successfully';
//	$result = mysql_query($sql);
//	mysql_close($link);
	
	
	$resultset = $db->query($sql);
	
	if(PEAR::isError($resultset)) 
	{
		$error = 'Failed to issue query, error message : ' . $resultset->getMessage().'<br />From :'. $sql;
		debug($error);
	}
	
	//Create the DB users and give permissions over the database
	//TODO allow connections from specified places
	$user_create_sql = array('CREATE USER \''.$user_rw.'\'@\'localhost\' [IDENTIFIED BY [PASSWORD] '.$user_db_password.']',
	'GRANT SELECT, INSERT, UPDATE, DELETE ON '.$database.' TO '. $user_rw,
	'CREATE USER \''.$user_r.'\'@\'localhost\' [IDENTIFIED BY [PASSWORD] '.$user_db_password.']',
	'GRANT SELECT ON '.$database.' TO '. $user_r);
	
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
	$settings_file_string = '
<?
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
?>
	';
	
	/*
	 * Close and write the settings file
	 */
	fwrite($settings_file_handle, $settings_file_string);
	fclose($settings_file_handle);
	debug('Settings file created');
	
	debug('Createing Homepage');
	$id = database::Singleton()->sql_insert('tbl_pages', array('parent_id', 'created', 'creator', 'visible'), array(0, time(), 0, 1));
	
	debug('Adding starting element');
	$element = Element::type('elementText');
	$element->create($id);
	
	
	echo('The site is now setup');
}

function debug($text)
{
	$debug = true;
	if($debug){ echo('<br />'.$text); }
}
?>