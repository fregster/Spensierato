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
		
class session
{
	private static $instance;
	private $session_loaded = false;

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
		//Only load cookie data if a cookie has been sent to help comply with EU reg's
		if(isset($_COOKIE['PHPSESSID']) || isset($_COOKIE['CMS']))
		{
			session_start();
			$this->session_user_agent_check();
			$this->session_ip_check();
			cookie::Singleton()->load();
		}
	}
	
	public function force()
	{
		if(!$this->session_loaded)
		{
			//Force a session to start
			@session_start();
		}
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
				}
			}
			else
			{
				$_SESSION['CMS_SALT'] = md5(CMS::time());;
				$_SESSION['HTTP_USER_AGENT'] = md5($_SERVER['HTTP_USER_AGENT']. $_SESSION['CMS_SALT']);
			}

			//Regerates the session id every 10 minuites but still keeps session data
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
				if ($_SESSION['CREATED_TIME'] > (CMS::time()+ 600))
				{
					/* The session is old regerate the id */
					$this->regenerate_session();
				}
			}
			else
			{
				$_SESSION['CREATED_TIME'] = CMS::time();
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
		    setcookie(session_name(), '', time()-42000, '/');
		}

		session_destroy();
	}

	public function destroy_session($url = NULL)
	{
		$this->delete_session();

		if($url == NULL) { $url = 'home'; }
		Common::redirect($url);
	}
}