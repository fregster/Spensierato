<?
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
	private $time_start;
	private $time_finish;
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

	private function __construct()
	{
		/*
		$time = microtime();
		print_r($time);
		echo('mtime: '.($time[0] + $time[1]).'<br />');
		die(microtime());
		*/
		//Start page timer
		if(Settings::Singleton()->Setting['site_option_showpagetime'])
		{
			$time = Common::time_with_microseconds();
		}
		
		//Get path vars
		$this->path_vars = Common::get_path_vars();
		
		//Work out document root (URL)
		$this->document_root = Common::document_root();
		Settings::Singleton()->new_setting('document_root', $this->document_root, true);
		
		//Work out the CMS file system root
		$this->cms_root = cms_root();
		Settings::Singleton()->new_setting('cms_root', $this->cms_root, true);
		
		//Work out the Site file system root
		$this->site_root = Common::site_root();

		//Static includes folder
		Settings::Singleton()->new_setting('site_folder_static_includes', ($this->cms_root . '/inc/static/'));
	}
	
	/**
	* start()
	* Use this function to initalise the CMS code
	* It sets up all the vars required to use the run_cms method
	*/
	public function start()
	{
		//Start the session
		$this->start_session();
		
		//Call the methods to set all the required vars
		//These should all be simple functions that do not require any external functions outside of CMS or Common and should not hit the DB
		
		//Update the settings with those from the DB
		Settings::Singleton()->get_database_settings();
		
		//Setup the user
		User::Singleton()->start();
	}
	
	public function run_cms()
	{
		//Work out what sort of page we are running
		$this->page_details = currentPage::Singleton()->page_details();

		//Decide what to do with the current page
		$this->get_page($this->page_details['type'], $this->page_details['value']);
		
	}
	
	public function end_cms()
	{	
		$this->time_finish = Common::time_with_microseconds();
		$totaltime = ($this->time_finish - $this->time_start) ;
		$phptime = $totaltime - Database::Singleton()->sql_time();
		$sql_info = Database::Singleton()->sql_count();
		
		$sql_string = $sql_info[0].' SQL Querys ('.round($sql_info[1]).'% Cached)';

		$runtime=('<div class="footer_text">Page generated in ' . Common::microtime_to_seconds($totaltime) . ' seconds<br /> ' . round((100/$totaltime*$phptime), 0) . '% php time and with ' .$sql_string.'</div>');
		
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
	
	private function destroy_session($url = NULL)
	{
		if($url == NULL) { $url = 'home'; }
		session_destroy();
		Common::redirect($url);
	}
	
	private function start_session()
	{
		session_set_cookie_params(0, Settings::Singleton()->get_setting('document_root'), $_SERVER['SERVER_NAME'], NULL, TRUE);
		session_start();
		
		$this->session_user_agent_check();
		$this->session_ip_check();
		cookie::Singleton()->load();
		
		ob_start();
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
				$_SESSION['CMS_SALT'] = md5(time());;
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
	
	private function site_login($id)
	{
		$_SESSION['site_login'] = true;
		$_SESSION['user_id'] = $id;
		User::site_get_users_details($_SESSION['user_id']);
		Database::Singleton()->sql_update_time('tbl_users', $_SESSION['user_id']);
		
		//Redirect after login
		if(isset($_GET['option']))
		{
		
			Common::redirect(Settings::Singleton()->get_setting('site_root') .'/' . $_GET['option']);
		}
		else
		{
			Common::redirect(Settings::Singleton()->get_setting('site_root') . '/home');
		}
	}
	
	private function get_page($type, $value)
	{		
		switch ($type) 
		{
			case 'HTML':
				//Database file, call content singleton
				$this->get_html();
			    break;
			    
			case 'STYLESHEET':
				//Just send the XSLT
				$this->get_stylesheet();
				break;
				
			case 'JS':
				//Get JS useing minify
				$this->get_js($value);
				break;
			
			case 'TINYMCE':
				//Get TinyMCE file
				$value = Settings::Singleton()->get_setting('site_folder_js').'/tiny_mce/'.$value;
				FileTransfer::singleton()->get_file($value);
				die();
				break;
				
			case 'HELP':
				$element = Element::type('elementHelp');
				$element->build('help');
				$this->get_html();
				break;
				
			case 'ACCOUNT':
				$element = Element::type('elementAccount');
				$element->build('account');
				$this->get_html();
				break;
				
			case 'ACCESS':
				$element = Element::type('elementAccess');
				$element->build('access');
				$this->get_html();
				break;
				
			case 'LOGIN':
				//CMS Login
				if($_SERVER['REQUEST_METHOD'] == 'GET')
				{
					//If not loged in load box else redirect to $value
					if(User::Singleton()->is_logged_in())
					{
						Common::redirect($value);
					}
					else
					{
						//Load CMS Login box
						$element = Element::type('elementLogin');
						$element->build('login');
						$this->get_html($value);
					}
				}
				else
				{
					//Do login function
					User::Singleton()->login();
					Common::redirect($value);
				}
				break;
			
			case 'LOGOUT':
				//CMS Logout
				$this->destroy_session($value);
				break;
						    
			case 'ADMIN':
				//Admin page check admin security
				if(Security::singleton()->is_admin())
				{
					$this->get_admin($value);
				}
				else
				{
					Error::do403();
				}
				break;
			
			case 'TOOL':
				//CMS Included Functions no security checks
				$file = $this->cms_root . '/tools/' . Common::clean_path($value) .'.inc';
				if(file_exists($file))
				{
					include($file);
					die();
				}
				else
				{
					CMS::do404();
				}
			    break;
				
			case 'CMS':
				//CMS Included Files page specific security checks
				$this->get_document($value);
				break;
				
			case 'DOWNLOAD':
				// Site downloads
				// Need to get document details
				// Check the related page details
				// call file transfer retrieve

				$element_detail = Content::singleton()->get_element_main_page_where_valid($value);
				
				if(Security::singleton()->can_view($element_detail['page_id']))
				{
					FileTransfer::singleton()->retrieve($value);
				}
				else
				{
					CMS::do403();
				}
				break;
			
				case 'IMAGE':
				//Site images NO security check
				/**
				 * Site images
				 * Need to get document details
				 * Check the related page details
				 * call file transfer retrieve
				 */

				$element_detail = Content::singleton()->get_element_main_page_where_valid($value);
				
				if(Security::singleton()->can_view($element_detail['page_id']))
				{
					FileTransfer::singleton()->retrieve($value, 'scaled');
				}
				else
				{
					CMS::do403();
				}
				break;
				
				case 'SKIN':
				//Skin images NO security check
				/**
				 * Site images
				 * Need to get document details
				 * Check the related page details
				 * call file transfer retrieve
				 */
				$value = Settings::Singleton()->get_setting('cms_root').'/skins/'.Settings::Singleton()->get_setting('skin') .'/graphics/'.$value;
				FileTransfer::singleton()->get_file($value);
				die();
				
				break;
				
			case 'CSS':
				//CSS Files NO security check
				/**
				 * Calls the template class which compresses and sends the CSS file
				 */
				Template::Singleton()->send_css();
				die();
				
				break;
			
			case 'EDIT':
				//Edit render element
				if(Security::singleton()->can_edit_element($value))
				{
					Content::singleton()->render_mode('edit');
					$this->get_edit($value);
				}
				else
				{
					CMS::do403();
				}
				break;
				
			case 'EDITSAVE':
				if(Security::singleton()->can_edit_element($value))
				{
					Content::singleton()->render_mode('edit');
					$this->save_edit($value);
				}
				else
				{
					CMS::do403();
				}
				break;
	
			case 'OPTIONTOGGLE':
				// Toggle option value
				options::Singleton()->toggle_option($value);
				Common::bounce_to_referer();
				break;
				
			case 'FILE':
				FileTransfer::Singleton()->get_public_file($value);
				break;
				
			case 'CAPTHCA':
				Capthca::create_Images();
				break;
			    
			case 'REDIR':
				$this->get_html($value);
			    break;
			    
			case 'FOLDER':
				if(Security::singleton()->can_view_element($value))
				{
					$element_xml = Content::singleton()->render_element_xml($value);
					$output_xml = DocumentFolder::convert_xml_to_displayable($element_xml);
					header('Content-Type: text/xml');
					die($output_xml);
				}
				else
				{
					CMS::do403();
				}
				
			default:
			    die('500 Unable in include any content because type (' . $type . ') unknown  (' . $value.')');
		}
	}
	
	private function get_html()
	{
		// Load the site XML pass directly into Render
		if($_GET['xml'] or Settings::singleton()->get_setting('send_as_xml'))
		{
			//Just send the XML
			header('content-type: text/xml');
			$search = '<?xml version="1.0"?>';
			$replace = '<?xml version="1.0" encoding="'. Settings::singleton()->get_setting('char_set').'"?><?xml-stylesheet href="'. Settings::singleton()->get_setting('document_root').'/xslt" type="text/xsl" ?>';
			
			echo(str_replace($search, $replace, Content::Singleton()->build()));
			exit;
		}
		
		Render::Singleton()->render_html(Content::Singleton()->build());
		//cookie::Singleton()->debug();
	}
	
	private function get_stylesheet()
	{
		header('content-type: text/xml');
		echo(Template::Singleton()->load());
		exit;
	}
	
	private function get_js($path)
	{
		$try = Settings::Singleton()->get_setting('cms_root').'/js/'.Common::clean_path($path);
		if(is_file($try))
		{
			$this->send_js_file($try);
			exit;
		}
		
//		die($try);
		
		$try = Settings::Singleton()->get_setting('cms_root') . '/publicfiles/'.Common::clean_path($path);
		if(is_file($try))
		{
			$this->send_js_file($try);
			exit;
		}
		die('404 '.$path);
	}
	
	private function send_js_file($file)
		{
			Cache::headers_allow_cache($file);

			// javascript header
			header('Content-type: text/javascript');
			if($file == Settings::Singleton()->get_setting('cms_root').'/js/tiny_mce/tiny_mce_gzip.php')
			{
				//TinyMCE Compressor so just let it run
				include($file);
			}
//			die($file);
			
			switch (Settings::Singleton()->get_setting('js_minify')) 
			{
				case '1':
					//Use internal code minification
					echo(Common::remove_extra_lines(file_get_contents($file)));
				    break;
				
				case '2':
					//Use JSMini
					echo JSMin::minify(file_get_contents($file));
				    break;
				
				default:
				    echo file_get_contents($file);
			}
		
			exit;
		}
}
?>