<?php
//			Setting the domain seems to sometimes stop the browser from saving the cookie
//		session_set_cookie_params(0, Settings::Singleton()->get_setting('document_root'), $_SERVER['SERVER_NAME'], NULL, TRUE);
//		if(Settings::Singleton()->get_setting('document_root') != '/')
//		{
////			session_set_cookie_params(0, NULL, NULL, NULL, TRUE);
//		}

//Use SHA1 as the Session ID type, reduces the possability of session stealing
//Increase the bits per Char so we can store it in a smaller DB field
ini_set('session.hash_bits_per_character', 5);
ini_set('session.hash_function', 1);
if(version_compare(phpversion(), '5.3', '>='))
{
	ini_set('session.hash_function', 2);
}
	
class session
{
	private static $instance;
	private $session_loaded = false;
	private $nonce_grace_time = 120;

	public static function Singleton()
	{
	   if (!isset(self::$instance))
	   {
		   $c = __CLASS__;
		   self::$instance = new $c;
	   }

	   return self::$instance;
	}
	
	protected function __construct()
	{
		$skip_validation = false;
		//Only load cookie data if a cookie has been sent to help comply with EU reg's
		if(get::variable('session', 'isset') or post::variable('session', 'isset')){
			session::set_cookie_from_hash();
			$skip_validation = true;
		}
		if(isset($_COOKIE['PHPSESSID']) || cookie::Singleton()->is_cookie_value_set('CMS'))
		{
			$this->force();
			$this->timeout_check();
			if(!$skip_validation){
				$this->session_user_agent_check();
				$this->session_ip_check();
				$this->session_nonce_check();
			}
			cookie::Singleton()->load();
			$this->session_loaded = true;
		}
	}
	
	public function force()
	{
		if(!$this->session_loaded)
		{
			//Force a session to start
			$path = DOCUMENT_ROOT;
			if(substr($path, -1) != '/'){
				$path .= '/';
			}
			session_set_cookie_params(0, $path);
			@session_start();
		}
		return true;
	}
	
	private static function set_cookie_from_hash()
	{
		$session_id = false;
		if(get::variable('session', 'isset')){
			$session_id = session::validate_unique_session_id(get::variable('session'));
		}else if (post::variable('session', 'isset')){
			$session_id = session::validate_unique_session_id(post::variable('session'));
		}
		
		if($session_id){
			$_COOKIE['PHPSESSID'] = $session_id;
		}
	}
	
	private function timeout_check()
	{
		if(isset($_SESSION['last_activity']) && ($_SESSION['last_activity'] + (Settings::singleton()->get_setting('session_force_timeout') * 60) < CMS::$time)){
			$this->delete_session();
			warnings::Singleton()->register('Session Timed Out', 'Your session was inactive for to long and you have been logged out', 0);
		}
		$_SESSION['last_activity'] = CMS::$time;
	}
	
	private function session_nonce_check()
	{
		if(Settings::Singleton()->get_setting('session_security_nonce') && isset($_SESSION['userid']) && $_SESSION['userid'] != 0)
		{
			if(!isset($_SESSION['nonce']))
			{
				$_SESSION['nonce'] = Common::generate_random_string(32, true);
				$_SESSION['nonce_time'] = CMS::$time;
				cookie::Singleton()->set_cookie('nonce', $_SESSION['nonce']);
				return true;
			}
			
			if(isset($_SESSION['nonce'])  && $_SESSION['nonce'] != cookie::Singleton()->get_cookie_value('nonce'))
			{
				//There is a grace period for nonce checking due to delayed parallel requests.
				if($_SESSION['nonce_time'] + $this->nonce_grace_time > CMS::$time)
				{
					cookie::Singleton()->set_cookie('nonce', $_SESSION['nonce']);
					cookie::Singleton()->destroy();
					return true;
				}
//				echo('<p>Nonce Error</p>'. $_SESSION['nonce'] .' != '.cookie::Singleton()->get_cookie_value('nonce').'<br/>');

				//TODO:This need's work before been implemented as you can get an overlap with the cookie not been set but the session var been set
				$this->destroy_session();
				$this->force();
				return false;
			}
		}
		return true;
	}
	
	private function session_user_agent_check()
	{
		//creates a hash of the user agent
		if(Settings::Singleton()->get_setting('session_security_enhanced'))
		{
			if (isset($_SESSION['HTTP_USER_AGENT']))
			{
				if ($_SESSION['HTTP_USER_AGENT'] != md5($_SERVER['HTTP_USER_AGENT'].$_SESSION['CMS_SALT']))
				{
					/* The user agent is not the same as when the session was created the cookie might have been hijacked */
					$this->destroy_session();
					$this->force();
				}
			}
			else
			{
				$_SESSION['CMS_SALT'] = md5(CMS::$time);;
				$_SESSION['HTTP_USER_AGENT'] = md5($_SERVER['HTTP_USER_AGENT']. $_SESSION['CMS_SALT']);
			}

			//Regerates the session id every 5 minuites but still keeps session data
			/* Why?
			This only helps if the cookie has been sniffed it must be used within the timeframe set
			If it gets sniffed and used before the time is up and previous checks pass then both users
			will be able to use the site, once the session is regenerated each use will have a seperate
			session.
			The CMS checks the logged in session against the tbl_user if found to be diffrent then 2 sessions are
			using the same user and so delete the session, this means the user must re-log in and so the sniffed
			account is also no longer valid
			*/
			if (isset($_SESSION['CREATED_TIME']))
			{
				if ($_SESSION['CREATED_TIME'] > (CMS::$time + 300))
				{
					/* The session is old regerate the id */
					$this->regenerate_session();
				}
			}
			else
			{
				$_SESSION['CREATED_TIME'] = CMS::$time;
			}
		}
	}
	
	/**
	 * If we have advanced session security we can if we wish lock the session to an IP,
	 * this will not work behind a dynamic upstream poxy set ie AOL users
	 * @return unknown_type
	 */
	private function session_ip_check()
	{
		if(Settings::Singleton()->get_setting('session_security_ip'))
		{
			if (isset($_SESSION['REMOTE_IP']))
			{
				if ($_SESSION['REMOTE_IP'] != md5($_SERVER['REMOTE_ADDR']))
				{
					/* The remote ip has changed from when the session was created the cookie might have been hijacked */
					$this->destroy_session();
				}
			}
			else
			{
				$_SESSION['REMOTE_IP'] = md5($_SERVER['REMOTE_ADDR']);
			}
		}
	}
	
	public function regenerate_session()
	{
		/* Regenerate the session file and delete the old one */
		session_regenerate_id(true);
		User::Singleton()->update_user_session_id();
	}

	public function delete_session()
	{
		@$_SESSION = @array(); //Clear the session data

		if (isset($_COOKIE[session_name()])) {
		    cookie::Singleton()->destroy();
		}

		session_destroy();
	}

	public function destroy_session($url = NULL)
	{
		$this->delete_session();

		if($url == NULL) { $url = 'home'; }
		Common::redirect($url);
	}
	
	public function generate_unique_session_id()
	{
		$salt = common::generate_random_string(32,true);
		$encrypt = encryption::encrypt(serialize(array('time' => CMS::$time, 'session_id' => session_id(), 'random_salt' => $salt)));
		return sha1(Settings::singleton()->get_setting('random_key').$encrypt).$encrypt.md5(CMS::$time.Settings::singleton()->get_setting('random_key').$salt);
	}
		
	public static function validate_unique_session_id($hash)
	{
		$encrypt = substr($hash, 40, -32);
	
		if(substr($hash, 0, 40) == sha1(Settings::singleton()->get_setting('random_key').$encrypt)){
			$decrypt = unserialize(encryption::decrypt($encrypt));
			if(($decrypt['time'] + settings::singleton()->get_setting('session_public_key_lifetime')) > CMS::$time && substr($hash, -32) == md5($decrypt['time'].Settings::singleton()->get_setting('random_key').$decrypt['random_salt'])){
				return $decrypt['session_id'];
			}
		}
		
		return 'error';
	}
}