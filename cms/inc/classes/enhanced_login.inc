<?php
require_once('user.inc');

class enhanced_login extends user
{
	private static $instance;
	private $required_hash_type = 'sha256';
	
	public static function Singleton()
	{
	   if (!isset(self::$instance)) 
	   {
		   $c = __CLASS__;
		   self::$instance = new $c;
	   }
	
	   return self::$instance;
	}
	
	public function suported()
	{
		if( (Settings::Singleton()->get_setting('site_option_encryption') == $this->required_hash_type) and (Settings::Singleton()->get_setting('login_capthca')) )
		{
			return true;
		}
		return false;
	}
	
	public function login()
	{
		
		if($this->site_auth())
		{
			$this->do_login();
			die('true');
		}
		
		CMS::Singleton()->delete_session();
		die('false');
	}
	
	private function site_auth()
	{
		if(!isset($_GET['key']) and $this->suported() )
		{
			return parent::site_auth();
		}
		
		//Make username case insensative by forcing lowercase
		$username = strtolower(get::variable('username'));
		
		//Get hash type for posted username
		$userdetails = Database::Singleton()->sql_select_first(array('hash_type', 'id', 'password'), 'tbl_users', array('username'), array($username));
		//Check hash type if count ! 1 then the username is not set
		//Check hash type is default if not auth against it and update the db on sucsessful auth
		if($userdetails['hash_type'] != $this->required_hash_type)
		{
			//Username unknown or the password hash is not sha256
			return('Enhanced Login is not enabled for this user and or site');
		}
		
		if(get::variable('key') == hash($this->required_hash_type, ($userdetails['password'].Capthca::get_code())))
		{
			$this->userid = $userdetails['id'];
			$this->do_login();
			return true;
		}
		return false;
	}
}

?>