<?php
/**
 * currentPage class
 * This class is for the working out what the current URL page should be doing
 * This is not specific to HTML pages just working out what the page is 
 *
 */
class currentPage
{
	private static $instance;
	private $page_details;
	private $page_type;
	private $id;
	
	public static function Singleton()
	{
	   if (!isset(self::$instance)) {
		   $c = __CLASS__;
		   self::$instance = new $c;
	   }
	
	   return self::$instance;
	}
	
	function __construct()
	{
		//Work out what the current page is
		$this->page_type = $this->check_page();
		$this->id = $this->page_type['value'];
		
		//Get current page details
		$this->get_page_details();
	}
	
	public function __clone()
	{
	   trigger_error('Clone is not allowed.', E_USER_ERROR);
	}
	
	//This name is no longer relevent but for the sake of not having to re-factor ...
	public function page_details()
	{
		return $this->page_type;
	}
	
	public function page_information()
	{
		return $this->page_details;
	}
	
	public function id()
	{
		return $this->page_type['value'];
	}
	
	public function title()
	{
		return $this->page_details['title'];
	}
	
	private function get_page_details()
	{
		$this->page_details = Database::Singleton()->sql_select_first(array('*'), 'tbl_pages', array('id'), array($this->id));
	}
	
	private function command_lookup($command)
	{
		$command = Database::Singleton()->sql_select_first(array('*'), 'tbl_commands', array('command'), array($command));
		if(count($command) == 1)
		{
			if(isset($command['page']))
			{
				return $command['page'];
			}
		}
		else
		{
			return false;
		}
	}
	
	private function check_page()
	{
		//Page id checking, if not set goto homepage
		if(!isset(CMS::Singleton()->path_vars[0]))
		{
			//$path_vars is not set so load an empty page from page
			return array('type' => 'HTML',
						'value' => Page::Singleton()->load(NULL));
		}
		else
		{
			//Check to see if the option is a command return 404 page if not found
			if(!is_numeric(CMS::Singleton()->path_vars[0]))
			{
				switch (CMS::Singleton()->path_vars[0]) 
				{
					//CMS Included Functions no security checks
					case 'tools':
							return array('type' => 'TOOL',
										'value' => CMS::Singleton()->path_vars[1]);
						break;
					
					//TinyMCE file
					case 'editor':
							return array('type' => 'CONTENTEDITOR',
										'value' => Common::clean_and_combine_url(CMS::Singleton()->path_vars, 1));
						break;
					
					//Advert page
					case 'ad':
						return array('type' => 'AD', 'value'=> null);
						break;
						
					case 'loading':
							return array('type' => 'LOADING',
										'value' => Common::clean_and_combine_url(CMS::Singleton()->path_vars, 1));
							break;
					
					case 'ajax':
							return array('type' => 'AJAX',
										'value' => CMS::Singleton()->path_vars);
							break;
							
					case 'register':
						if($_SERVER['REQUEST_METHOD'] == 'POST')
						{
							//Submision of registration details
							return array('type' => 'REGISTER_DETAILS',
										'value' => CMS::Singleton()->path_vars[1]);
						}
						else
						{
							//Filling in registration forms
							return array('type' => 'REGISTER',
										'value' => CMS::Singleton()->path_vars[1]);
						}
					
					//CMS Included Files page specific security checks
					case 'cms':
						return array('type' => 'CMS',
									'value' => CMS::Singleton()->path_vars[1]);
						break;
						
					//CSS download
					case 'css':
						return array('type' => 'CSS',
									'value' => CMS::Singleton()->path_vars[1]);
						break;
					
					//Site downloads does a security check
					case 'download':
						return array('type' => 'DOWNLOAD',
									'value' => CMS::Singleton()->path_vars[1]);
						break;
					
					//CMS files no security cached
					case 'file':
						return array('type' => 'FILE',
									'value' => Common::clean_and_combine_url(CMS::Singleton()->path_vars, 1));
						break;
						
					//CMS JS files no security cached and minifyed
					case 'js':
						return array('type' => 'JS',
									'value' => Common::clean_and_combine_url(CMS::Singleton()->path_vars, 1));
						break;
					
					//TODO Merge
					//CMS JS files running through combine no security and cached
					case 'js-combine':
						return array('type' => 'JAVASCRIPT',
									'value' => Common::clean_and_combine_url(CMS::Singleton()->path_vars, 1));
						break;
						
					case 'xslt':
						return array('type' => 'STYLESHEET',
									'value' => Common::clean_and_combine_url(CMS::Singleton()->path_vars, 1));
						break;
					
					//Site images NO security check
					case 'image':
						return array('type' => 'IMAGE',
									'value' => CMS::Singleton()->path_vars[1]);
						break;
						
					//Skin images NO security check
					case 'skin':
						return array('type' => 'SKIN',
									'value' => CMS::Singleton()->path_vars[1]);
						break;
					
					//CMS Admin pages Checks for Site Admin permissions
					case 'admin':
						return array('type' => 'ADMIN',
									'value' => Common::clean_and_combine_url(CMS::Singleton()->path_vars, 1));
						break;
						
					//CMS Users Account page
					case 'account':
						return array('type' => 'ACCOUNT',
									'value' => Page::Singleton()->load(CMS::Singleton()->path_vars[1]));
						break;
						
					//CMS Users Account page
					case 'warnings':
						return array('type' => 'WARNINGS',
									'value' => Page::Singleton()->load(CMS::Singleton()->path_vars[1]));
						break;
					
					//CMS in edit mode
					case 'edit':
						if($_SERVER['REQUEST_METHOD'] == 'POST')
						{
							//This is to update an element
							return array('type' => 'EDITSAVE',
										'value' => CMS::Singleton()->path_vars[1]);
						}
						else
						{
							if(CMS::Singleton()->path_vars[1] == 'toggle')
							{
								return array('type' => 'EDIT_TOGGLE',
											'value' => array(CMS::Singleton()->path_vars[2],CMS::Singleton()->path_vars[3]));
							}

							//View an element in edit mode
							return array('type' => 'EDIT',
										'value' => CMS::Singleton()->path_vars[1]);
						}
						
						break;
						
					case 'tp':
							return array('type' => 'EDIT_TOGGLE_PAGE',
										'value' => CMS::Singleton()->path_vars);
						break;
						
					case 'te':
							return array('type' => 'EDIT_TOGGLE_ELEMENT',
										'value' => CMS::Singleton()->path_vars);
						break;
						
					case 'up':
							return array('type' => 'EDIT_UPDATE_PAGE',
										'value' => CMS::Singleton()->path_vars);
						break;
							
					//Set a CMS var and bounce back	
					case 'toggle':
						return array(	'type' => 'OPTIONTOGGLE',
										'value' => CMS::Singleton()->path_vars[1],
										'bounce' => CMS::Singleton()->path_vars[2]);
										
					//Set a CMS var and bounce back	
					case 'capthca':
							return array(	'type' => 'CAPTHCA');
						
					//Logging in / out options
					case 'login':
						return array(	'type' => 'LOGIN',
										'value' => Page::Singleton()->load(CMS::Singleton()->path_vars[1]));
										
					case 'logout':
						return array(	'type' => 'LOGOUT',
										'value' => Page::Singleton()->load(CMS::Singleton()->path_vars[1]));
								
					//Static CMS content type pages		
					case 'help':
						return array(	'type' => 'HELP',
										'value' => Page::Singleton()->load(CMS::Singleton()->path_vars[1]));
										
					case 'access':
						return array(	'type' => 'ACCESS',
										'value' => Page::Singleton()->load(CMS::Singleton()->path_vars[1]));
						
					case 'install':
						return array( 'type' => 'INSTALL',
										'value' => null);
					
					default:
						$command = $this->command_lookup($page_var);
		
						if($command != false)
						{
							return array('type' => $command['type'],
										'value' => $command['page']);
						}
						else
						{
							die('Page not setup');
							Error::error(404, 'No page found');
						}
				}
			}
			else
			{
				//pat_vars[0] is numeric
				if(CMS::Singleton()->page_exists(CMS::Singleton()->path_vars[0]) == true)
				{
					return array('type' => 'HTML',
								'value' => CMS::Singleton()->path_vars[0]);
				}
				else
				{
					Error::error(404, 'The page requested does not exsist');
				}
			}
		}
	}
	
	
	public function get_page_parents($id) 
	{
		if($id != 0)
		{
			$parents_exist = true;
			$parents = array();
			$current_page = $id;
				
			while($parents_exist == true) 
			{
				$result = Database::Singleton()->sql_select(array('id', 'parentid'), 'tbl_pages', array('id'), array($current_page));
		
				$id = $result[0]['parentid'];
				
				array_push($parents, $id);
						
				if(($id == 0) or ($id == null) )
				{
					$parents_exist = false;
					break;
				} 
				else 
				{
					$current_page = $id;
				}
			}
			array_pop($parents);
		}
		else
		{
			$parents = array();
		}
		return $parents;
	}

	public function include_page($page_id)
	{
		echo('<div id="page_section_body">');
		
		$page_type = Database::Singleton()->sql_select_first(array('*'), 'tbl_pages', array('id'), array($page_id));
		
		if(Settings::Singleton()->Setting['debug'] > 3)
		{
			print_r($page_type);
		}
		
		if(count($page_type) == 0)
		{
			echo(count($page_type));
			error_page('404', 'Page load error');
		}
		else
		{
			$elements = Database::Singleton()->sql_select(array('*'), 'tbl_page_elements', array('page'), array($page_id), array('sort'));
	
			if(count($elements) != 0)
			{
				foreach($elements as $element)
				{
					render_element($element['element'], $element['type']);
				}
			}
		}
		echo('</div>
	');
	}
	
	public function get_page($id) 
	{
		$page = Database::Singleton()->sql_select_first(NULL, 'tbl_pages', array('id'), array($id));
		return $page;
		
	}
}
?>