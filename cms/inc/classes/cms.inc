<?php
class CMS
{
	public static $cms_version = '0.0.1';
	public $path_vars = array();
	public $page_id;
	public static $time;

	//Page details
	public $page_details;
	//URL root for site
	private static $document_root;
	//File system root for the CMS folder
	private static $cms_root;

	//Private vars
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
		CMS::$time = time();
		$this->microtime = microtime();
				
		//Start the session object
		session::Singleton();

		//Is a persistant_cache available ie memcached supported
		if(!Settings::Singleton()->get_setting('cache_database_queries')){
			Database::Singleton()->sql_cache_disable();
		} else if(persistantcache::Singleton()->enabled() == true)
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
		CMS::$document_root = Common::document_root();
		Settings::Singleton()->new_setting('document_root', CMS::$document_root, true);

		//Work out the CMS file system root
		CMS::$cms_root = cms_root();
		Settings::Singleton()->new_setting('cms_root', CMS::$cms_root, true, true);

		//Static includes folder
		Settings::Singleton()->new_setting('site_folder_static_includes', (CMS::$cms_root.DS.'inc'.DS.'static'.DS), false, true);
	}

	public function time($microtime = false)
	{
		if($microtime){
			return Common::time_with_microseconds();
		}
		return CMS::$time;
	}

	public function cms_root()
	{
		return CMS::$cms_root;
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
		if(isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'])
		{
			Settings::singleton()->new_setting('https',$_SERVER['HTTPS'] ,false,false);
		}
		Settings::singleton()->new_setting('host',$_SERVER['SERVER_NAME'] ,false,false);

		//Call the methods to set all the required vars
		//These should all be simple functions that do not require any external functions outside of CMS or Common and should not hit the DB

		if(Options::singleton()->get_option('admin_mode'))
		{
			if(options::Singleton()->get_option('disable_sql_cache'))
			{
				Database::Singleton()->sql_cache_disable();
			}
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
		//Clickjacking protection default code option
		//The render class enforces deny where required
		header ('X-Frame-Options: sameorigin');
		
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
		if(Settings::singleton()->get_setting('cache_xslt_transformation'))
		{
			return ('<div id="page_section_footer">XSLT Cache Enabled, Render information not available</div>');
		}
		
		$this->time_finish = Common::time_with_microseconds();
		$totaltime = ($this->time_finish - $this->time_start);
		$phptime = $totaltime - Database::Singleton()->sql_time();
		$sql_info = Database::Singleton()->sql_count();
		$cache = persistantcache::Singleton()->type();
		$percent = 0;
		if($totaltime != 0 and $phptime != 0) { $percent = $phptime/$totaltime*100;}

		$sql_string = $sql_info[0].' SQL Querys (';
		
		if(Options::singleton()->get_option('disable_sql_cache'))
		{
			$sql_string .= 'Database cache disabled) ';
		}
		else
		{
			$sql_string .= round($sql_info[2]).' Cached in '.$cache.') through ('. strtoupper($GLOBALS['database_backend']);
			
			if(Database::Singleton()->using_prepared_statments()){
				$sql_string .= '+';
			}

			$sql_string .=')';
		}

		$runtime=('<div class="footer_text">Page generated in ' . round($totaltime, 4) . ' seconds on '. date("D M j G:i:s T Y") .'<br /> ' . round($percent, 0) . '% php time using '.(memory_get_peak_usage(true) / (1024 * 1024)).' MB and running ' .$sql_string .'</div>');

		return ('<div id="page_section_footer">'.$runtime.'</div>');

		//Run any methods required after the site has rendered
		Debug::Singleton()->render();
	}

	public function page_exists()
	{
		return true;
	}

	public function path_vars($index){
		if (isset($this->path_vars[$index])) {
			return $this->path_vars[$index];
		}
		return NULL;
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

			require_once(dirname(__FILE__).DS.'..'.DS.'..'.DS.'cms_admin_includes.inc');

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
	 * @return unknown_type
	 */
	public function shutdown()
	{
		persistantcache::Singleton()->garbage_collection();
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