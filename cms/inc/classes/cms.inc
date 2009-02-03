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

	private function __construct()
	{
		//Is a persistant_cache available ie memcached supported
		$this->persistant_cache = persistantcache::Singleton()->load();
		
		if($this->persistant_cache == true)
		{
			Database::Singleton()->use_memcached();
		}
		
		//Start the session
		$this->start_session();
		
		warnings::Singleton()->register('Options Loaded', '', 4);
		
		//Start page timer
		if(Settings::Singleton()->Setting['site_option_showpagetime'])
		{
			$this->time_start = Common::time_with_microseconds();
		}
		$this->time = time();
		$this->microtime = microtime();
		
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
		return $this->time;
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
		//If required include PHPIDS
		if(Settings::singleton()->get_setting('phpids'))
		{
			require_once('./phpids/phpids.inc');
		}	
		ob_start();
		
		if($_SERVER['HTTPS'])
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
		
		//Update settings which require moding
		$this->settings_update();
		
		//Setup the user
		User::Singleton()->start();
	}
	
	public function run_cms()
	{
		//Do any admin checks
		$this->admin();
		
		//Work out what sort of page we are running
		$this->page_details = currentPage::Singleton()->page_details();
	
		//Decide what to do with the current page
		$this->get_page();
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
	
	public function delete_session()
	{
		$_SESSION = array();
		
		if (isset($_COOKIE[session_name()])) {
		    setcookie(session_name(), '', time()-42000, '/');
		}

		session_destroy();
	}
	
	private function destroy_session($url = NULL)
	{
		$this->delete_session();
		
		if($url == NULL) { $url = 'home'; }
		Common::redirect($url);
	}
	
	private function start_session()
	{
//		if($this->memcached == true)
//		{
//			$session_save_path = "tcp://localhost:11211?persistent=1&weight=2&timeout=2&retry_interval=10,  ,tcp://$host:$port  ";
//			ini_set('session.save_handler', 'memcache');
//			ini_set('session.save_path', $session_save_path);
//		}
//			Setting the domain seems to sometimes stop the browser from saving the cookie
//		session_set_cookie_params(0, Settings::Singleton()->get_setting('document_root'), $_SERVER['SERVER_NAME'], NULL, TRUE);
//		if(Settings::Singleton()->get_setting('document_root') != '/')
//		{
////			session_set_cookie_params(0, NULL, NULL, NULL, TRUE);
//		}
		//Use SHA1 as the Session ID type, reduces the possability of session stealing
		//Increase the bits per Char so we can store it in a smaller DB field
		ini_set('session.hash_bits_per_character',    5);
		ini_set('session.hash_function',    1);
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
	
	private function admin()
	{
		if(Options::singleton()->get_option('admin_mode'))
		{
			//Force loading of all elements

			require_once(dirname(__FILE__).'/../../cms_admin_includes.inc');
			
			//When in admin mode do extra code such as version checking
			$this->version_check();
			
			//Register any shutdown cleanup functions
			register_shutdown_function(array(&$this, 'shutdown'));
		}
	}
	
	public function shutdown()
	{
		
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
		die();
		$_SESSION['site_login'] = true;
		User::site_get_users_details($id);
		
		
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
	
	private function get_page()
	{
		$value = $this->page_details['value'];
		switch ($this->page_details['type']) 
		{
			case 'HTML':
				//Database file, call content singleton
				Options::singleton()->admin_check();
				Render::Singleton()->render_html(Content::Singleton()->build(), true);
			    break;
			    
			case 'STYLESHEET':
				//Just send the XSLT
				$this->get_stylesheet();
				break;
				
			case 'LOADING':
				//Send a loading page and image;
				$this->send_loading($this->page_details['value']);
				break;
				
			case 'JS':
				//Get JS useing minify
				$this->get_js($this->page_details['value']);
				break;
			
			case 'JAVASCRIPT':
				//Get JS useing minify
				javascript::run($this->page_details['value']);
				die();
				break;
			
			case 'CONTENTEDITOR':
				ContentEditor::run($this->page_details['value']);
				break;
				
			case 'HELP':
				$element = Element::type('elementHelp');
				$element->build('help');
				$this->get_html();
				break;
				
			case 'REGISTER':
				//If not loged in load box else redirect to $this->page_details['value']
				if(User::Singleton()->is_logged_in())
				{
					Common::redirect($this->page_details['value']);
				}
				else
				{
					if(Settings::singleton()->get_setting('allow_registration'))
					{
						if(isset($_GET['key']))
						{
							register::singleton()->verify_register();
							die();
						}
						else
						{
							//Check cookies are enabled first
							Common::cookie_check();
	
							//Load CMS Registration box
							$element = Element::type('elementRegister');
							$element->build('Register');
							$this->get_html($this->page_details['value']);
						}
					}
					else
					{
						Error::do403();
					}
				}
				break;
				
			case 'REGISTER_DETAILS':
				if(User::Singleton()->is_logged_in())
				{
					Common::redirect($this->page_details['value']);
				}
				else
				{
					if(Settings::singleton()->get_setting('allow_registration'))
					{
						//Do registration function
						register::singleton()->new_registration();
						$element = Element::type('elementRegister');
						$element->build('Register');
						$this->get_html($this->page_details['value']);
					}
					else
					{
						Error::do403();
					}
				}
				break;
				
			case 'WARNINGS':
				$element = Element::type('elementWarnings');
				$element->build('Warnings');
				Render::Singleton()->render_edit(Content::Singleton()->build());
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
				
				//Check cookies are enabled first
				Common::cookie_check();
				if($_SERVER['REQUEST_METHOD'] == 'GET' and !isset($_GET['username']))
				{
					//If not loged in load box else redirect to $this->page_details['value']
					if(User::Singleton()->is_logged_in())
					{
						Common::redirect($this->page_details['value']);
					}
					else
					{
						//Load CMS Login box
						$element = Element::type('elementLogin');
						$element->build('login');
						$this->get_html($this->page_details['value']);
					}
				}
				else
				{
					//Do login function
					User::Singleton()->login();
					Common::redirect($this->page_details['value']);
				}
				break;
			
			case 'LOGOUT':
				//CMS Logout
				$this->destroy_session($this->page_details['value']);
				break;
						    
			case 'ADMIN':
				//Admin page check admin security
				if(Security::singleton()->is_admin())
				{
					$this->get_admin($this->page_details['value']);
				}
				else
				{
					Error::do403();
				}
				break;
			
			case 'TOOL':
				//CMS Included Functions no security checks
				$file = $this->cms_root . '/tools/' . Common::clean_path($this->page_details['value']) .'.inc';
				if(file_exists($file))
				{
					include($file);
					die();
				}
				else
				{
					error::do404();
				}
			    break;
				
			case 'CMS':
				//CMS Included Files page specific security checks
				$this->get_document($this->page_details['value']);
				break;
				
			case 'DOWNLOAD':
				// Site downloads
				// Need to get document details
				// Check the related page details
				// call file transfer retrieve

				
				if(Security::singleton()->can_view_element($this->page_details['value']))
				{	
					FileTransfer::singleton()->get_download($this->page_details['value']);
				}
				else
				{
					error::do403();
				}
				break;
			
				case 'IMAGE':
				/**
				 * Site images
				 * Need to get document details
				 * Check the related page details
				 * call file transfer retrieve
				 */

				if(Security::singleton()->can_view_element($this->page_details['value']))
				{	
					FileTransfer::singleton()->get_image($this->page_details['value']);
				}
				else
				{
					error::do403();
				}
				break;
				
				case 'STREAM':
					Stream::stream($this->page_details['value']);
					die();
				break;
				
				case 'PREVIEW':
				/**
				 * Site images
				 * Need to get document details
				 * Check the related page details
				 * call file transfer retrieve
				 */

				if(Security::singleton()->can_view_element($this->page_details['value']))
				{	
					FileTransfer::singleton()->get_preview($this->page_details['value']);
				}
				else
				{
					error::do403();
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
					
				$file_search = array(
				Settings::Singleton()->get_setting('cms_root').'/skins/'.Settings::Singleton()->get_setting('skin').'/'.$this->page_details['value'],
				Settings::Singleton()->get_setting('cms_root').'/skins/'.Settings::Singleton()->get_setting('skin') .'/graphics/'.$this->page_details['value'],
				Settings::Singleton()->get_setting('cms_root').'/skins/'.Settings::Singleton()->get_setting('skin') .'/pre-load-images/'.$this->path_vars[2],
				Settings::Singleton()->get_setting('cms_root').'/skins/default/pre-load-images/'.$this->path_vars[2],
				Settings::Singleton()->get_setting('cms_root').'/skins/default/pre-load-images/'.$this->path_vars[1],
				Settings::Singleton()->get_setting('cms_root').'/skins/default/graphics/'.$this->path_vars[1],
				);
				
				foreach($file_search as $file)
				{
					if(is_file($file))
					{
						FileTransfer::singleton()->get_file($file, true, false);
						die();
					}
				}
				error::do404();
				
				break;
				
			case 'CSS':
				//CSS Files NO security check
				/**
				 * Calls the template class which compresses and sends the CSS file
				 */
				Render::Singleton()->send_css();
				die();
				
				break;
				
			case 'AD':
				ad::google();
				die();
				break;
			
			case 'EDIT':
				//Edit render element
				if(Security::singleton()->can_edit_element($this->page_details['value']))
				{	
					$element = Element::type($this->page_details['value']);
					$element->build($this->page_details['value']);
					Render::Singleton()->render_edit(Content::Singleton()->build());
				}
				else
				{
					if($this->page_details['value'] === 'style.css')
					{
						FileTransfer::singleton()->get_file(Settings::Singleton()->get_setting('cms_root').'/skins/default/style.css');
					}
					error::do403();
				}
				break;
				
			case 'EDIT_TOGGLE_PAGE':
				if(Security::singleton()->can_approve($this->page_details['value'][1]))
				{
					if($this->page_details['value'][2] == 'visibility')
					{
						database::singleton()->sql_toggle_field_by_id($this->page_details['value'][1], 'tbl_pages', 'visible');
					}
					
					if($this->page_details['value'][2] == 'delete')
					{
						database::singleton()->sql_toggle_field_by_id($this->page_details['value'][1], 'tbl_pages', 'deleted');
					}
					common::redirect($this->page_details['value'][1]);
				}
				else
				{
					error::do403();
				}
				break;
				
			case 'EDIT_UPDATE_PAGE':			
				if(Security::singleton()->can_approve($this->page_details['value'][1]))
				{	
					//TODO update from laptop as required classes have not been comited
					if(isset($_POST['title']))
					{
						database::singleton()->sql_update_by_id($this->page_details['value'][1], 'tbl_pages', array('title'), array(post::variable('title', 'sql')));
					}
					common::redirect($this->page_details['value'][1]);
				}
				else
				{
					error::do403();
				}
				break;
				
			case 'EDIT_TOGGLE_ELEMENT':
				if(Security::singleton()->can_approve_element($this->page_details['value'][1]))
				{	
					$element = Element::type($this->page_details['value'][1]);
					$element->build($this->page_details['value'][1]);
					
					$element->toggle($this->page_details['value'][2]);
					common::redirect($element->main());
				}
				else
				{
					error::do403();
				}
				break;
				
			case 'AJAX':
				//Pass directly to the AJAX class for processing
				ajax::process($this->page_details['value']);
				break;
				
			case 'EDITSAVE':
				//Is this an update or createing a new element
				//TODO remove refrences to POST
				if($this->page_details['value'] == 'account')
				{
					User::Singleton()->update_from_post();
					Common::redirect('account');
				}
				
				if(isset($_POST['new_type']))
				{
					if(Security::singleton()->can_approve($this->page_details['value']))
					{
						if(post::variable('new_type') === 'newPage')
						{
							$this->create_new_page($this->page_details['value']);
						}
						else
						{
							$element = Element::type($_POST['new_type']);
							$element->create($this->page_details['value']);
						}
					}
					else
					{
						error::do403();
					}
				}
				else
				{
					if(Security::singleton()->can_edit_element($this->page_details['value']))
					{
						$element = Element::type($this->page_details['value']);
						$element->build($this->page_details['value']);
						if($element->save(false))
						{
							//Save worked bounce to main page for element
							$this->saved($element->main());
//							Common::redirect($element->main());
						}
						else
						{
							//Save failed show error page
						}
					}
					else
					{
						error::do403();
					}
				}
				common::redirect($this->page_details['value']);
				
				break;
	
			case 'OPTIONTOGGLE':
				// Toggle option value
				options::Singleton()->toggle_option($this->page_details['value']);
				Common::bounce_to_referer();
				break;
				
			case 'FILE':
				FileTransfer::Singleton()->get_public_file($this->page_details['value']);
				break;
				
			case 'CAPTHCA':
				Capthca::create_Images();
				break;
			    
			case 'REDIR':
				$this->get_html($this->page_details['value']);
			    break;
			    
			case 'FOLDER':
				if(Security::singleton()->can_view_element($this->page_details['value']))
				{
					$element_xml = Content::singleton()->render_element_xml($this->page_details['value']);
					$output_xml = DocumentFolder::convert_xml_to_displayable($element_xml);
					header('Content-Type: text/xml');
					die($output_xml);
				}
				else
				{
					error::do403();
				}
				
			default:
			    die('500 Unable in include any content because type (' . $type . ') unknown  (' . $this->page_details['value'].')');
		}
	}
	
	private function create_new_page($parent_id)
	{
		if($parent_id === '-1')
		{
			if(Security::Singleton()->is_admin())
			{
				$id = database::Singleton()->sql_insert('tbl_pages', array('created', 'creator', 'title'), array(time(), user::Singleton()->id(), 'Homepage'));
			}
		}
		else
		{
			$sort = database::Singleton()->sql_max_field('tbl_pages', 'sort', array('parent_id'), array($parent_id));
			$id = database::Singleton()->sql_insert('tbl_pages', array('parent_id', 'created', 'creator'), array($parent_id, time(), user::Singleton()->id()));
		}
		common::redirect($id);
	}
	
	private function get_html()
	{		
		Render::Singleton()->render_html(Content::Singleton()->build());
		//cookie::Singleton()->debug();
	}
	
	private function get_stylesheet()
	{
		header('Content-Type: text/xml; charset='.Settings::Singleton()->get_setting('char_set'));
		die(Template::Singleton()->load());
	}
	
	private function get_js($path)
	{
		$try = Settings::Singleton()->get_setting('cms_root').'/js/'.Common::clean_path($path);
		if(is_file($try))
		{
			$this->send_js_file($try);
			exit;
		}
		
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
			
			switch (Settings::Singleton()->get_setting('js_minify')) 
			{
				case '1':
					//Use internal code minification
					echo(Common::remove_js_comments(file_get_contents($file)));
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

	private function send_loading($value)
	{
		include(Settings::Singleton()->get_setting('cms_root').'/js/submodal/loading.inc');
		die();
	}

	private function settings_update()
	{
		//Update folders to make them relative if required
		$settings = array('cms_folder_uploads',
						'cms_folder_downloads',
						'cms_folder_orig_images');
		
		foreach($settings as $setting)
		{
			$value = Settings::singleton()->get_setting($setting);
			//die(substr($setting, 0, 1));
			if(substr($value, 0, 1) != '/')
			{
				$new_folder = $this->cms_root.'/'.$value;
				Settings::Singleton()->overwrite_setting($setting, $new_folder);
			}
			
		}
	}

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

	private function version_check()
	{
		$hostname = Settings::singleton()->get_setting('update_version_check');
		if($hostname != NULL)
		{
			return false;		
		}
		
		
		$update_log = Settings::singleton()->get_setting('cms_root') . '/writeable/logs/update.log';
		if(file_exists($update_log))
		{
			$pervious_update_check = file_get_contents($update_log);
		}
		$update_interval = Settings::Singleton()->get_setting('update_version_check_schedule')*24*60*60;
		
		if($pervious_update_check+$update_interval > time())
		{

			$update_log = fopen($update_log, 'w');
			
			fwrite($update_log, time());
			fclose($update_log);
			
			
			//This function does not work in windows fo fake it
			if(!function_exists('checkdnsrr'))
			{
			    function checkdnsrr($host, $type='')
			    {
			        if(!empty($host))
			        {
			            $type = (empty($type)) ? 'MX' :  $type;
			            exec('nslookup -type='.$type.' '.escapeshellcmd($host), $result);
			            $it = new ArrayIterator($result);
			            foreach(new RegexIterator($it, '~^'.$host.'~', RegexIterator::GET_MATCH) as $result)
			            {
			                if($result)
			                {
			                    return true;
			                }               
			            }
			        }
			        return false;
			   	}
			}
			
			//If a DNS record is found
			if(checkdnsrr($hostname, 'TXT'))
			{
				$dns = dns_get_record($hostname, DNS_TXT);
				if($dns['TXT'] > CMS::$cms_version)
				{
					warnings::Singleton()->register('New version of Spensierato released', 'There is a new version available ('.$dns['txt'].') to download which might contain new features, bug fixes and security updates, please visit the spensierato website for details');
					return true;
				}
				else
				{
					warnings::Singleton()->register('Auto update check', 'Update check shows you are running the latest version of Spensierato '.$dns['txt'], 3);
					return false;
				}
			}
		}
		return false;
	}

	private function persistant_cache()
	{
		return persistantcache::Singleton()->load();
	}
}
?>