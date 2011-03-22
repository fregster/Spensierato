<?php
class authentication_default extends authentication
{
	public function auth()
	{
		if(!post::variable('username', 'isset')){
			return 'Username and password was not set';
			warnings::singleton()->register('Username and password was not set');
		}
		$username = post::variable('username');
		$password = post::variable('password') . Settings::singleton()->get_setting('random_key');

		//Check security code first
		if(Capthca::enabled() && !Capthca::Code_check(post::variable('security_code')))
		{
			return 'Security Code was incorect';
			warnings::singleton()->register('Security Code was incorect');
		}

		//Make username case insensative by forcing lowercase
		$username = strtolower($username);

		//Get hash type for posted username
		$hash_type_db = Database::Singleton()->sql_select_first(array('hash_type', 'id'), 'tbl_users', array('username'), array($username));
		//Check hash type if count ! 1 then the username is not set
		//Check hash type is default if not auth against it and update the db on sucsessful auth
		if(!isset($hash_type_db['hash_type']))
		{
			//Username unknown
			//Return username and password error else it gives away the username is valid
			warnings::singleton()->register('Username and password do not match');
			return 'Username and password do not match';
		}
		//The hash is actually a hash of the userid concaternated with the password to add extra security against simple rainbow tables
		//Removed because I need it to be able to be worked out on the client side for the JS sha256 login stuff
		###also on line 156###

		$hash_outdated = FALSE;
		if($hash_type_db['hash_type'] != Settings::Singleton()->get_setting('site_option_hash_type'))
		{
			$hash_outdated = true;
			$hash_types_supported = array_flip(hash_algos());
		}
		if($hash_outdated == true && isset($hash_types_supported[Settings::Singleton()->get_setting('site_option_hash_type')]))
		{
			$sql_password = hash($hash_type_db['hash_type'], $password);
		}
		else
		{
			$sql_password = Common::return_hash($password, false);
		}
		
		$id = Database::Singleton()->sql_select_first('id', 'tbl_users', array('username', 'password'), array($username, $sql_password));
		
		if(isset($id['id']))
		{
			if($hash_outdated == true)
			{
				User::singleton()->create_password($password);
			}
			User::singleton()->force_id($id['id']);
			return true;
		}
		else
		{
			return 'Username and password do not match';
		}
	}
}