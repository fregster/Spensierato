<?php
/**
// Proxy class to the user and membership backends
// Well should be!
*/
class user
{

	private static $instance;
	private $userid;
	private $lists;
	private $user_details;
	
	public static function Singleton()
	{
	   if (!isset(self::$instance)) 
	   {
		   $c = __CLASS__;
		   self::$instance = new $c;
	   }
	
	   return self::$instance;
	}
	
	public function __clone()
	{
	   trigger_error('Clone is not allowed.', E_USER_ERROR);
	}
	
	function __construct()
	{
		if(isset($_SESSION['userid']))
		{
			$this->userid = $_SESSION['userid'];
		}
		else
		{
			$this->userid = 0;
		}
		
		$this->set_user_details();
		
		//Check that default required values are set		
		if(!isset($this->lists))
		{
			$this->lists = array(0 => 0);
		}
	}

	public function start()
	{
		
	}
	
	public function id()
	{
		return $this->userid;
	}
	
	public function lists()
	{
		return $this->lists;
	}
	
	public function is_logged_in()
	{
		if($this->userid == 0)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	
	public function create($username, $email)
	{
		database::Singleton()->force_write_user();
			$this->userid = database::singleton()->sql_insert('tbl_users', array('username'), array($username));
			database::singleton()->sql_insert('tbl_users_data', array('data', 'user_id'), array('<email>'.$email.'</email>', $this->userid));
			$this->set_user_details();
			$this->reset_password();
		database::Singleton()->destroy_write_user();
		
		$this->do_login();
		Common::redirect(1);
	}
	
	public function login()
	{
		if($_SERVER['REQUEST_METHOD'] == 'GET')
		{
			$auth = $this->site_auth_hash(get::variable('username'), get::variable('hash'));
		}
		else
		{
			$auth = $this->site_auth(post::variable('username'), post::variable('password'), post::variable('security_code'));
		}
		
		if($auth = true)
		{
			if(post::variable('login_rememberme') == 'true')
			{
				cookie::Singleton()->set_cookie('username', post::variable('username'));
			}
			else
			{
				cookie::Singleton()->remove_cookie('login_rememberme');
			}
			$this->do_login();
		}
		else
		{
			warnings::Singleton()->register('Login Failed', 'Either your Username or Password was incorrect or you are not registered', 0);
			Common::reload_with_vars(array('authfailed'), array($auth));
		}
		Common::bounce_to_referer();
	}
	
	public function update_user_session_id()
	{
		Database::Singleton()->sql_update_by_id($this->userid, 'tbl_users', array('session_id'), $values = array(session_id()));
	}
	
	public function update_from_post()
	{
		$values = array('nickname' => post::variable('nickname'),
						'forename' => post::variable('forename'),
						'surname' => post::variable('surname'),
						'email' => post::variable('email')
					);

		$xml = '';
		foreach($values as $key => $value)
		{
			if($value != '')
			{
				$xml .= '<'.$key.'>'.common::html($value).'</'.$key.'>';
			}
		}
		
		database::Singleton()->sql_update_by_id($this->id(), 'tbl_users_data', array('data'), array($xml), true);
	}
	
	public function is_admin()
	{
		return $this->user_details['administrator'];
	}
	
	public function reset_password()
	{
		//Create a password
		$password = common::generate_random_string(6);
		
		$hash = hash(Settings::Singleton()->get_setting('site_option_encryption'), $userid.$password);
		
		//Save the password in the DB
		$this->update_password($hash);
		
		//Email the password out
		$message = 'Your password for your account on '. Settings::singleton()->get_setting('site_name_long').' has been reset<br />Your password is '.$password;
		$from = Settings::singleton()->get_setting('site_name_long').' <no-reply@'.str_replace('www.', '', $_SERVER['SERVER_NAME']).'>';
		$subject = 'Password change at '.settings::singleton()->get_setting('site_name_long');

		return common::email($this->email(), $from, $subject, $message, true);
	}
	
	public function user_details_as_xml()
	{
		$serializer_options = array ( 
			   'addDecl' => FALSE, 
			   'encoding' => Settings::Singleton()->get_setting('char_set'), 
			   'indent' => '  ',
			   'rootName'  => 'user',
			   'defaultTagName'  => 'detail'
			);
		return Common::array_to_xml($this->user_details, $serializer_options);
	}

	/**
	 * Returns the users email address as string 'User <email@address.tld>'
	 * Returns false if there is a problem
	 */
	public function email()
	{
		return $this->user_details['username'].' <'.$this->user_details['email'].'>';
	}
	
	private function set_user_details()
	{
		//$this->user_details = Database::Singleton()->sql_select_first(array('nickname', 'forename', 'surname', 'email'), 'tbl_users', array('id'), array($this->userid));
		
		$user_data = Database::Singleton()->sql_select(array('data'), 'tbl_users_data', array('user_id'), array($this->userid));
		
		if(is_array($user_data))
		{
			foreach($user_data as $data)
			{
				$data = '<xml>'.$data['data'].'</xml>';
				$xml = simplexml_load_string($data);
	
				foreach ($xml->children() as $child)
				{
					$this->user_details[$child->getName()] = strip_tags($child->saveXML());
				}
			}
		}
		
		$user_details2 = Database::Singleton()->sql_select_first(array('username', 'admin'), 'tbl_users', array('id'), array($this->userid));
		
		$this->user_details['id'] = $this->userid;
		$this->user_details['username'] = $user_details2['username'];
		$this->user_details['administrator'] = $user_details2['admin'];
		
		$this->lists = Lists::Singleton()->get_lists_for_user($this->userid);
	}

	private function site_auth_hash($username, $hash)
	{
		
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
			die('Username and password do not match 1');
		}
		//The hash is actually a hash of the userid concaternated with the password to add extra security against simple rainbow tables
		$pwd = $password;

		if($hash_type_db['hash_type'] != Settings::Singleton()->get_setting('site_option_encryption'))
		{
			$this->update_password($hash);
		}

		$id = Database::Singleton()->sql_select_first(array('id'), 'tbl_users', array('username', 'password'), array($username, $hash));
		if(count($id) == 1)
		{
			$this->userid = $id['id'];
			die(1);
		}
		else
		{
			die('Username and password do not match 2');
		}
		
		
		
	}
	
	//Returns true on sucsess or false on fail
	private function site_auth($username, $password, $security_code)
	{
		//Check security code first
		if(!Capthca::Code_check($security_code))
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
		$pwd = $password;

		if($hash_type_db['hash_type'] != Settings::Singleton()->get_setting('site_option_encryption'))
		{
			$hash_outdated = true;
		}
		if($hash_outdated == true)
		{
			$sql_password = hash($hash_type_db['hash_type'], $pwd);
		}
		else
		{
			$sql_password = Common::return_hash($pwd, false);
		}
		$id = Database::Singleton()->sql_select_first(array('id'), 'tbl_users', array('username', 'password'), array($username, $sql_password));
		if(count($id) == 1)
		{
			if($hash_outdated == true)
			{
				$this->update_password($hash);
			}
			$this->userid = $id['id'];
			return true;
		}
		else
		{
			return 'Username and password do not match';
		}
	}
	
	private function update_password($hash)
	{
		return Database::Singleton()->sql_update_by_id($this->userid, 'tbl_users', array('password', 'hash_type'), array($hash, Settings::Singleton()->get_setting('site_option_encryption')));
	}
	
	private function do_login()
	{
		Database::Singleton()->force_write_user();
		
		//Regenerate the session ID
		CMS::Singleton()->regenerate_session();
		
		$_SESSION['userid'] = $this->userid;
		$this->set_user_details();

		//Update last login time
		Database::Singleton()->sql_update_by_id($this->userid, 'tbl_users', array('last_login'), $values = array(time()));
		
		//Load the previous option state
		options::Singleton()->load_saved_options();
		options::Singleton()->set_option('force_reload', true);
		
		Database::Singleton()->destroy_write_user();
	}
	
}
?>