<?php
class CMS
{
	public static $cms_version = '0.0.1';
	public $path_vars = array();
	public $page_id;

	//Page details
	public $page_details;
	//URL root for site
	private static $document_root;
	//File system root for the CMS folder
	private static $cms_root;
	//File system root for the outer site
	private static $site_root;

	//Private vars
	private $time;
	private $microtime;
	private $time_start;
	private $time_finish;
	private $persistant_cache;
	//Switch to turn on testing / debuging / logging
	private $testing = true;

	private $html;
	private static $instance;

	public static function Singleton()

	{
	   if (!isset(self::$instance)) {
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
		$this->time = time();
		$this->microtime = microtime();

		//Start the session
		$this->start_session();

		//Is a persistant_cache available ie memcached supported
		if(persistantcache::Singleton()->enabled() == true)
		{
			Database::Singleton()->use_memcached();
		}

		warnings::Singleton()->register('Options Loaded', '', 4);

		//Start page timer
		if(Settings::Singleton()->get_setting('site_option_showpagetime'))
		{
			$this->time_start = Common::time_with_microseconds();
		}

		//Get path vars
		$this->path_vars = Common::get_path_vars();

		//Work out document root (URL)
		$this->document_root = Common::document_root();
		Settings::Singleton()->new_setting('document_root', $this->document_root, true);

		//Work out the CMS file system root
		$this->cms_root = cms_root();
		Settings::Singleton()->new_setting('cms_root', $this->cms_root, true, true);

		//Work out the Site file system root
		$this->site_root = Common::site_root();

		//Static includes folder
		Settings::Singleton()->new_setting('site_folder_static_includes', ($this->cms_root . '/inc/static/'), false, true);
	}

	public function time()
	{
		return Common::time_with_microseconds();
	}

	public function cms_root()
	{
		return $this->cms_root;
	}

	public function microtime()
	{
		return $this->microtime;
	}

	/**
	* start()
	* Use this function to initalise the CMS code
	* It sets up all the vars required to use the run_cms method
	*/
	public function start()
	{
		ob_start();
		
		//If required include PHPIDS
//		if(Settings::singleton()->get_setting('phpids'))
//		{
//			require_once(cms_root().'/external/phpids/phpids.inc');
//		}

		if(isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'])
		{
			Settings::singleton()->new_setting('https',$_SERVER['HTTPS'] ,false,false);
		}
		Settings::singleton()->new_setting('host',$_SERVER['SERVER_NAME'] ,false,false);

		//Call the methods to set all the required vars
		//These should all be simple functions that do not require any external functions outside of CMS or Common and should not hit the DB

		if(Options::singleton()->get_option('admin_mode'))
		{
			Database::Singleton()->sql_cache_disable();
		}

		//Setup the user
		User::Singleton()->start();
	}

	/**
	 * Use this function after the inclusion of any custom files
	 * This is the start of the main code flow
	 * 
	 * It stats by checking if there is any admin work to do
	 * It then class the currentPage, this works out what the current page request is for and creates a cmspage object and initalises it
	 * Finally run_cms() runs the page (Each page decides it's own security requirments and output)
	 * @return unknown_type
	 */
	public function run_cms()
	{
		//Do any admin checks
		$this->admin();

		//Work out what sort of page we are running
		$this->page_details = currentPage::Singleton()->page_details();

		//Decide what to do with the current page
		cmspage::singleton()->run();
	}

	/**
	 * end_cms()
	 * This is called by the render object to return a footer string if required.
	 * @return string
	 */
	public function end_cms()
	{
		$this->time_finish = Common::time_with_microseconds();
		$totaltime = ($this->time_finish - $this->time_start);
		$phptime = $totaltime - Database::Singleton()->sql_time();
		$sql_info = Database::Singleton()->sql_count();
		$cache = persistantcache::Singleton()->type();
		$percent = 0;
		if($totaltime != 0 and $phptime != 0) { $percent = $phptime/$totaltime*100;}

		$sql_string = $sql_info[0].' SQL Querys ('.round($sql_info[2]).' Cached in '.$cache.') through ('. strtoupper($GLOBALS['database_backend']) .')';

		$runtime=('<div class="footer_text">Page generated in ' . round($totaltime, 4) . ' seconds on '. date("D M j G:i:s T Y") .'<br /> ' . round($percent, 0) . '% php time and with ' .$sql_string.' using '.(memory_get_peak_usage(true) / (1024 * 1024)).' MB</div>');

		return ('<div id="page_section_footer">'.$runtime.'</div>');

		/*
		//Run any methods required after the site has rendered
		Debug::Singleton()->render();
		*/
	}

	public function page_exists()
	{
		return true;
	}

	public function regenerate_session()
	{
		/* Regenerate the session file and delete the old one */
		session_regenerate_id(true);
		User::Singleton()->update_user_session_id();
	}

	public function delete_session()
	{
		$_SESSION = array(); //Clear the session data

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

	public function path_vars($index){
		if (isset($this->path_vars[$index])) {
			return $this->path_vars[$index];
		}
		return NULL;
	}

	private function start_session()
	{
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
		session_start();
		$this->session_user_agent_check();
		$this->session_ip_check();
		cookie::Singleton()->load();
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
				$_SESSION['CMS_SALT'] = md5($this->time);;
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
				if ($_SESSION['CREATED_TIME'] > (time() + 600))
				{
					/* The session is old regerate the id */
					$this->regenerate_session();
				}
			}
			else
			{
				$_SESSION['CREATED_TIME'] = time();
			}
		}
	}

	/**
	 * Just some checks to do if were an admin user, like version_checking
	 * @return true
	 */
	private function admin()
	{
		if(Options::singleton()->get_option('admin_mode'))
		{
			//Force loading of all elements

			require_once(dirname(__FILE__).'/../../cms_admin_includes.inc');

			//When in admin mode do extra code such as version checking
			update::check();
			
			if(!isset($_SESSION['check_inc_restrictions']))
			{
				if(check_inc_restrictions())
				{
					warnings::singleton()->register('System Access Permissions', 'Your webserver is serving files directly, this is very bad. Please enable mod rewrite, use the .htacess or disable serving files from below the root folder at least block *.inc');
				}
				else
				{
					$_SESSION['check_inc_restrictions'] = true;
				}
			}

			//Register any shutdown cleanup functions
			register_shutdown_function(array(&$this, 'shutdown'));
		}
	}

	/**
	 * shutdown()
	 * This is a shutdown method that can be called after the page has finished rendering.
	 * Currently does nothing
	 * @return unknown_type
	 */
	public function shutdown()
	{

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

	private function testing()
	{
		return $this->testing;
	}

	//this has been moved to cmspage_edit, and should not be required here anymore.
	//delete on cleanup
	private function saved($main)
	{
		die('
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
	<title>Saved</title>
	<link rel="stylesheet" type="text/css" href="style.css" />
	<script type="text/javascript">
		window.top.hidePopWin(true);
	</script>
</head>
<body>
	<p>
	Your updates have been saved.<br />
	You should be redirected back the page the element you changed is on in a few seconds<br/>
	If not click <a href="'.$this->document_root.'/'.$main.'">here</a>
	</p>

</body>
</html>');
	}

	/**
	 * Check to see if persistant cacheing is enabled
	 * @return true / false
	 */
	private function persistant_cache()
	{
		return persistantcache::Singleton()->load();
	}
}