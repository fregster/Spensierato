<?
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
	}

	public function start()
	{
		//Check that default required values are set		
		if(!isset($this->lists))
		{
			$this->lists = array(0 => 0);
		}
	}
	
	public function id()
	{
		return $this->userid;
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
	
	public function login()
	{
		//Regenerate the session ID
		CMS::Singleton()->regenerate_session();
		$auth = $this->site_auth($_POST['username'], $_POST['password'], $_POST['security_code']);
		if($auth == true)
		{
			$this->set_user_details();
			if(isset($_POST['login_rememberme']))
			{
				cookie::Singleton()->set_cookie('username', $_POST['username']);
			}
			else
			{
				cookie::Singleton()->remove_cookie('login_rememberme');
			}
			//Load the previous option state
			options::Singleton()->load_saved_options();
			options::Singleton()->set_option('force_reload', true);
		}
		else
		{
			warnings::Singleton()->register('Login Failed', 'Either your Username or Password was incorrect or you are not registered', 0);
			Common::reload_with_vars(array('authfailed'), array($auth));
		}
		Common::redirect(1);
	}
	
	public function update_user_session_id()
	{
		//TODO
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

	//Returns true on sucsess or false on fail
	private function site_auth($username, $password, $security_code)
	{
		//Check security code first
		if(!Capthca::Code_check($security_code))
		{
			return 'Security Code was incorect';
		}
		
		//Get hash type for posted username
		$hash_type_db = Database::Singleton()->sql_select_first(array('hash_type'), 'tbl_users', array('username'), array($username));
		//Check hash type if count ! 1 then the username is not set
		//Check hash type is default if not auth against it and update the db on sucsessful auth
		if(!isset($hash_type_db['hash_type']))
		{
			//Username unknown
			//Return username and password error else it gives away the username is valid
			return 'Username and password do not match';
		}

		if($hash_type_db['hash_type'] != Settings::Singleton()->get_setting('site_option_encryption'))
		{
			$hash_outdated = true;
		}
		if($hash_outdated == true)
		{
			$sql_password = hash($hash_type_db['hash_type'], $password);
		}
		else
		{
			$sql_password = Common::return_hash($password, false);
		}
		$id = Database::Singleton()->sql_select_first(array('id'), 'tbl_users', array('username', 'password'), array($username, $sql_password));
		if(count($id) == 1)
		{
			if($hash_outdated == true)
			{
				$this->update_password($hash);
			}
			
			$this->userid = $id['id'];
			$_SESSION['userid'] = $this->userid;
			return true;
		}
		else
		{
			return 'Username and password do not match';
		}
	}
	
	private function update_password($hash)
	{
		return Database::Singleton()->sql_update_by_id(array('password', 'hash_type'), array($hash, Settings::Singleton()->get_setting('site_option_encryption')));
	}
	
}
?>