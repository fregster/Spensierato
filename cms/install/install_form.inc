<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
	<head>
		<meta http-equiv="content-type" content="text/html;charset=UTF-8" />
		
		<title>Spensierato Install Page</title>	
	</head>
<body>

<form method="post" action="">
	<div id="dbinfo">
	Please specify the connection information to your SQL server, this user needs to be able to create users and databases, as well as give permissions (Usally root or sa)
		<div>Host: <input type="text" name="host" value="localhost" /></div>
		<div>MDB2 Host type: <select name="type"><option value="mysqli">MySQL</select></div>
		<div>Username: <input type="text" name="username" value="root" /></div>
		<div>Password: <input type="password" name="password" /></div>
	</div>

	<div id="database">
		Database to connect to or create
		<div>Database name: <input type="text" name="database" value="spensierato" /></div>
	</div>

	<div id="prefix">
		If your connecting to an exsisting database this must match the origonal table prefix
		<div>Table Prefix name: <input type="text" name="prefix" value="<? echo(Common::generate_random_string(3, false)); ?>" /></div>
	</div>
	
	<div id="randomkey">
		Create a random key. This is used when generating hashes to help preventing people work out the final hash it defaults to a sha1 hash of the server microtime when you install.
		<div>Random Key: <input type="text" name="random_key" value="<? echo(sha1(microtime())); ?>" /></div>
	</div>
	
	<div id="db_users">
		If no usernames are set then one will be created and a random password will be generated (Recomended)<br />
		If the password set is does not meet the recomended length an random password will also be created
		<div>Read only username: <input type="text" name="r_user" value="spensierato_read" /></div>
		<div>Read Write username: <input type="text" name="rw_user" value="spensierato_writ" /></div>
		<div>Users password: <input type="password" name="db_password" value="<? echo(Common::generate_random_string(16)); ?>" /></div>
	</div>
	
	<div id="users">
		Create a user for the site
		<div>Username: <input type="text" name="user_username" value="admin" /></div>
		<div>Password: <input type="text" name="user_password" value="<? echo(Common::generate_random_string(8)); ?>" /></div>
		<div>Email: <input type="text" name="user_email" value="root@localhost" /></div>
	</div>
	
	<div><input type="submit" value="Create Site" /></div>
</form>

</body>
</html>