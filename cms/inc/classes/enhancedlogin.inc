<?php
/**
 * Enhanced Secure Login class
 * This is an extension of the user class login functions
 * The class enhances security by using javascript to create a hash of the users password on the client
 * The has is sent over the internet as an ajax post request
 * This class builds the correct hash to compare with the transmitted hash
 *
 * the hash uses a captcha image to create a salt for the hash.
 * This prevents the salt been sent as clear text
 * hash format sha1 (   sha1(password) + salt )
 * This is not a replacment for SSL but does prevent username and passwords showing up in proxy logs
 *
 */
class enhancedLogin extends user
{
	private $required_hash_type = 'sha256';
	private $key;
	private $user;

	private static $instance;

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
		if( (Settings::Singleton()->get_setting('site_option_hash_type') == $this->required_hash_type) and (Settings::Singleton()->get_setting('login_capthca')) and (Settings::Singleton()->get_setting('login_secure')) and function_exists('imagettfbbox') )
		{
			return true;
		}
		return false;
	}

	public function login()
	{
		$this->key = post::variable('key');
		$this->user = strtolower(post::variable('username'));
		if($this->site_auth())
		{
			$this->do_login();
			die('1');
		}

		session::Singleton()->delete_session();
		die('0');
	}

	private function site_auth()
	{
		if(!($this->key) and !($this->suported()) )
		{
			return parent::site_auth();
		}

		//Get hash type for posted username
		$userdetails = Database::Singleton()->sql_select_first(array('hash_type', 'id', 'password'), 'tbl_users', array('username'), array($this->user));
		//Check hash type if count ! 1 then the username is not set
		//Check hash type is default if not auth against it and update the db on sucsessful auth
		if($userdetails['hash_type'] != $this->required_hash_type)
		{
			//Username unknown or the password hash is not sha256
			return('Enhanced Login is not enabled for this user and/or site');
		}

		if($this->key == hash($this->required_hash_type, ($userdetails['password'].Capthca::get_code())))
		{
			$this->userid = $userdetails['id'];
			return true;
		}
		return false;
	}
}