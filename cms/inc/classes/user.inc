<?php
/**
// Proxy class to the user and membership backends
// Well should be!
*/
class user
{

	private static $instance;
	protected $userid = 0;
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

	protected function __construct()
	{
		if(isset($_SESSION['userid']))
		{
			$this->userid =& $_SESSION['userid'];
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
		return true;
	}

	public function create($username, $email, $password = NULL, $admin = false)
	{
		database::Singleton()->force_write_user();
		$cols = array('username');
		$data = array($username);

		if($admin)
		{
			array_push($cols, 'admin');
			array_push($data, 1);
		}
		$this->userid = database::singleton()->sql_insert('tbl_users', $cols, $data);

		database::singleton()->sql_insert('tbl_users_data', array('data', 'user_id'), array(encryption::encrypt('{"email":"'.$email.'"}'), $this->userid));
		$this->set_user_details();
		if($admin)
		{
			return $this->create_password($password);
		}
		else
		{
			$this->reset_password();
		}
		database::Singleton()->destroy_write_user();

		$this->do_login();
		Common::redirect(1);
	}

	public function login()
	{
		$auth = $this->site_auth();
		if($auth === true)
		{
			user::Singleton()->store('login_error', null);
			warnings::Singleton()->drop('login_error', 0);
			$this->do_login();
		}
		else
		{
			warnings::Singleton()->register('Login Failed', 'Either your Username or Password was incorrect or you are not registered', 0);
			user::Singleton()->store('login_error', 'Either your Username or Password was incorrect or you are not registered');
			Common::redirect_inc_vars(implode('/', common::get_path_vars()), array('authfailed' => $auth));
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

		$data = encryption::encrypt(json_encode($values));

		database::Singleton()->sql_update_by_id($this->id(), 'tbl_users_data', array('data'), array($data), true);
		$this->set_user_details(true); //Reload the users details
	}

	public function is_admin()
	{
		if($this->is_logged_in() && isset($this->user_details['administrator']))
		{
			return $this->user_details['administrator'];
		}
		return false;
	}

	private function create_password($password = NULL)
	{
		//Create a password
		if($password == NULL)
		{
			$password = common::generate_random_string(10);
		}
		//The hash is actually a hash of the userid concaternated with the password to add extra security against simple rainbow tables
		//Removed because I need it to be able to be worked out on the client side for the JS sha256 login stuff
		###also on line 246###
		$hash = @hash(Settings::Singleton()->get_setting('site_option_encryption'), $password . Settings::singleton()->get_setting('random_key'));

		//Save the password in the DB
		$this->update_password($hash);

		return $password;
	}

	public function reset_password()
	{
		$password = $this->create_password();

		//Email the password out
		$message = 'Your password for your account on '. Settings::singleton()->get_setting('site_name_long').' has been reset<br />Your password is '.$password;
		$from = Settings::singleton()->get_setting('site_name_long').' <no-reply@'.str_replace('www.', '', $_SERVER['SERVER_NAME']).'>';
		$subject = 'Password change at '.settings::singleton()->get_setting('site_name_long');
		
		if(isset($_SESSION['user_details'])){
			unset($_SESSION['user_details']);
		}

		return common::email($this->email(), $from, $subject, $message, true);
	}

	public function user_details_as_xml()
	{
		return xml::array_to_xml_simple($this->user_details, 'user', 'detail');
	}

	/**
	 * Returns the users email address as string 'User <email@address.tld>'
	 * Returns false if there is a problem
	 */
	public function email()
	{
		return $this->user_details['username'].' <'.$this->user_details['email'].'>';
	}

	private function set_user_details($force = false)
	{		
		if(isset($this->userid) && $this->userid != 0)
		{
			if(!isset($_SESSION['user_details']) || $force)
			{
				$user_data = Database::Singleton()->sql_select_first(array('data'), 'tbl_users_data', array('user_id'), array($this->userid));
				if(!isset($user_data) or $user_data != NULL)
				{
					$_SESSION['user_details'] = json_decode(encryption::decrypt($user_data['data']), TRUE);
				}
				
				$user_details2 = Database::Singleton()->sql_select_first(array('username', 'admin'), 'tbl_users', array('id'), array($this->userid));
				$_SESSION['user_details']['username'] = $user_details2['username'];
				$_SESSION['user_details']['administrator'] = $user_details2['admin'];
			}

			$this->user_details =& $_SESSION['user_details'];
			
			if(!isset($_SESSION['lists']) || $force)
			{
				$_SESSION['lists'] = Lists::Singleton()->get_lists_for_user($this->userid);
			}

			$this->lists =& $_SESSION['lists'];
			
		}
		
		if(!isset($this->user_details['id']))
		{
			$this->user_details['id'] = $this->userid;
		}
	}

	//Returns true on sucsess or error text on fail
	private function site_auth()
	{
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
		if($hash_type_db['hash_type'] != Settings::Singleton()->get_setting('site_option_encryption'))
		{
			$hash_outdated = true;
			$hash_types_supported = array_flip(hash_algos());
		}
		if($hash_outdated == true && isset($hash_types_supported[Settings::Singleton()->get_setting('site_option_encryption')]))
		{
			echo(Settings::Singleton()->get_setting('site_option_encryption'));
			var_dump($hash_types_supported);
			echo($hash_type_db['hash_type']);
			die();
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
				$this->update_password($sql_password);
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

	protected function do_login()
	{
		if(post::variable('login_rememberme') == 'true')
		{
			cookie::Singleton()->set_cookie('username', post::variable('username'));
		}
		else if (get::variable('login_rememberme') == 'true')
		{
			cookie::Singleton()->set_cookie('username', get::variable('username'));
		}
		else
		{
			cookie::Singleton()->remove_cookie('login_rememberme');
		}

		Database::Singleton()->force_write_user();

		//Regenerate the session ID
		Session::Singleton()->regenerate_session();

		$_SESSION['userid'] = $this->userid;
		$this->set_user_details();

		//Update last login time
		Database::Singleton()->sql_update_by_id($this->userid, 'tbl_users', array('last_login'), $values = array(time()));

		//Load the previous option state
		options::Singleton()->load_saved_options();
		options::Singleton()->set_option('force_reload', true);

		Database::Singleton()->destroy_write_user();
	}
	
	public function username($id = NULL)
	{
		if(is_null($id))
		{
			return $this->user_details['username'];
		}
		
		$username = database::Singleton()->sql_select_first('username', 'tbl_users', 'id', $id);
		if(isset($username['username']))
		{
			return $username['username'];
		}
		
		return 'Unknown';
	}

	public function store($name, $value)
	{
		return $_SESSION[$name] = $value;
	}
	
	public function retrieve($name)
	{
		if(!isset($_SESSION[$name])){
			return null;
		}
		return $_SESSION[$name];
	}
}