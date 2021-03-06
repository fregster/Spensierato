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
	private $key_lifetime = 3600;

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

	public function __construct()
	{
		$this->set_user_details();

		//Check that default required values are set
		if(!isset($this->lists[0]))
		{
			$this->lists = array(0 => 0);
		}
	}

	public function start()
	{

	}
	
	public function force_id($id)
	{
		$this->userid = $id;
		$this->set_user_details(true);
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

	public function create($username, $email, $password = NULL, $admin = false, $redirect = true)
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
		if($redirect){
			Common::redirect(1);
		}
	}

	public function login()
	{
		$auth = authentication::singleton()->auth();
		
		if($auth === true)
		{
			user::Singleton()->store('login_error', null);
			warnings::Singleton()->drop('login_error', 0);
			log::record('login', 'sucsess');
			$this->do_login();
		}
		else
		{
			log::record('login', 'failed', true);
			warnings::Singleton()->register('Login Failed', 'Either your Username or Password was incorrect or you are not registered', 0);
			user::Singleton()->store('login_error', 'Either your Username or Password was incorrect or you are not registered');
			Common::redirect_inc_vars(implode('/', common::get_path_vars()), array('authfailed' => $auth));
		}

		Common::bounce_to_referer();
	}

	public function update_user_session_id($old_id = null)
	{
		$fields = array('session_id', 'session_refresh_ts');
		$values = array(session_id(), CMS::$time);
		if(!is_null($old_id)){
			$fields[] = 'session_id_old';
			$values[] = $old_id;
		}
		
		database::Singleton()->force_write_user();
		Database::Singleton()->sql_update_by_id($this->userid, 'tbl_users', $fields, $values);
	}

	public function update_from_post()
	{
		$values = array('nickname' => post::variable('nickname'),
						'forename' => post::variable('forename'),
						'surname' => post::variable('surname'),
						'email' => post::variable('email')
					);

		$data = encryption::encrypt(json_encode($values));

		database::singleton()->force_write_user();
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

	public function create_password($password = NULL)
	{
		//Create a password
		if($password == NULL)
		{
			$password = common::generate_random_string(Settings::Singleton()->get_setting('user_min_password_length'), true);
		}
		//The hash is actually a hash of the userid concaternated with the password to add extra security against simple rainbow tables
		//Removed because I need it to be able to be worked out on the client side for the JS sha256 login stuff
		###also on line 246###
		$hash = @hash(Settings::Singleton()->get_setting('site_option_hash_type'), $password . Settings::singleton()->get_setting('random_key'));
		//Save the password in the DB
		
		//echo $password . Settings::singleton()->get_setting('random_key'). ' - '.$hash;
		$this->update_password($hash);

		return $password;
	}
	
	public function remote_reset_password()
	{
		if($this->user_details['session_reset_nonce_ts'] > 0 && ($this->user_details['session_reset_nonce_ts'] + Settings::Singleton()->get_setting('forgotten_password_timeout') > cms::$time)) {
			
			database::singleton()->force_write_user();
			database::singleton()->sql_update_by_col('tbl_users', array('id'), array($this->userid), array('session_reset_nonce', 'session_reset_nonce_ts'), array('0', '0'));
			$this->reset_password();
			die('Your password reset and has been emailed to you. Please change your password as soon as you login');
			//blank nonce and ts
		}
	}

	public function reset_password()
	{
		log::record('Forgotten Password Reset', 'Sent ('.$this->userid.')');
		$password = $this->create_password();

		//Email the password out
		$message = 'The password for your account on <a href="'.Settings::singleton()->get_setting('document_root').'">'. Settings::singleton()->get_setting('site_name_long').'</a> has been reset<br /><br />Your password is '.$password.'<br/><br/>';
		$message .= 'Please login at <a href="'.Settings::singleton()->get_setting('document_root').'/login">'.Settings::singleton()->get_setting('document_root').'/login</a>';
		$from = Settings::singleton()->get_setting('site_name_long').' <no-reply@'.str_replace('www.', '', $_SERVER['SERVER_NAME']).'>';
		$subject = 'Password change at '.settings::singleton()->get_setting('site_name_long');
		
		if(isset($_SESSION['user_details'])){
			unset($_SESSION['user_details']);
		}

		return common::email($this->email(), $from, $subject, $message, true);
	}
	
	public function forgotten_password()
	{
		$fpuuid = common::generate_random_string(128, true);	
	
		database::singleton()->force_write_user();
		database::singleton()->sql_update_by_col('tbl_users', array('id'), array($this->userid), array('session_reset_nonce', 'session_reset_nonce_ts'), array($fpuuid, cms::$time));
		
		//Email the password out
		$message = 'A forgotten password request was made for your account on '. Settings::singleton()->get_setting('site_name_long').'<br />If you did not do this then please delete this email, if it happens again then someone may be trying to access your account.<br /><br />';
		$message .= '<p>To reset your password then please follow thie link <br/><a href="'.Settings::singleton()->get_setting('document_root').'/forgottenpassword?id='.$this->userid.'&fpuuid='.$fpuuid.'">'.Settings::singleton()->get_setting('document_root').'/forgottenpassword?id='.$this->userid.'&fpuuid='.$fpuuid.'</a></p><p>This link will only work once, if you have requested this email more than once then only the last one will work</p>';
		$from = Settings::singleton()->get_setting('site_name_long').' <no-reply@'.str_replace('www.', '', $_SERVER['SERVER_NAME']).'>';
		$subject = 'Password change at '.settings::singleton()->get_setting('site_name_long');
	
		log::record('Forgotten Password Request', 'Sent ('.$this->userid.')');
		
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

	public function set_user_details($force = false)
	{
		if(isset($_SESSION['userid']))
		{
			$this->userid = $_SESSION['userid'];
		}
		
		if(isset($this->userid) && $this->userid != 0)
		{			
			if(!isset($_SESSION['user_details']) || $force || true)
			{
				$user_data = Database::Singleton()->sql_select_first(array('data'), 'tbl_users_data', array('user_id'), array($this->userid));
				if(!isset($user_data) or $user_data != NULL)
				{
					$_SESSION['user_details'] = json_decode(encryption::decrypt($user_data['data']), TRUE);
					if(!is_array($_SESSION['user_details']))
					{
						$_SESSION['user_details'] = array();
					}
				}
				
				$user_details2 = Database::Singleton()->sql_select_first(array('username', 'admin', 'session_reset_nonce_ts'), 'tbl_users', array('id'), array($this->userid));
				$_SESSION['user_details']['username'] = $user_details2['username'];
				$_SESSION['user_details']['administrator'] = $user_details2['admin'];
				$_SESSION['user_details']['session_reset_nonce_ts'] = $user_details2['session_reset_nonce_ts'];
			}
			
//			if(options::singleton()->get_option('admin_mode')){
				$_SESSION['user_details']['unique_session_id'] = session::Singleton()->generate_unique_session_id();
//			} else if (!options::singleton()->get_option('admin_mode') && isset($_SESSION['user_details']['unique_session_id'])){
//				unset($_SESSION['user_details']['unique_session_id']);
//			}

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
		authentication::singleton()->auth();
	}

	private function update_password($hash)
	{
		database::Singleton()->force_write_user();
		return Database::Singleton()->sql_update_by_id($this->userid, 'tbl_users', array('password', 'hash_type'), array($hash, Settings::Singleton()->get_setting('site_option_hash_type')));
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