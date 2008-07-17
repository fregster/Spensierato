<?php
/**
 * Security class
 * This security class is the default Spensierato security
 *
 * There are 4 levels in security and permissions are inherited down unless explicetly blocked
 * Block
 * View
 * Edit
 * Approve
 * 
 * If no security is set can_view returns true and only site admins can edit and approve
 * 
 * Once view is set public access is blocked
 * Users with Edit and Approve can always view
 * 
 * If a user is listed in 'Block' all inherited security is dropped and false is returned
 * unless the security for a specific page has been requested
 */
class Security
{
	protected static $instance;
	protected $levels;
	
	public static function Singleton()
	{
	   if (!isset(self::$instance)) {
		   
	   		if(class_exists(Settings::Singleton()->get_setting('security_model')))
	   		{
	   			$c = Settings::Singleton()->get_setting('security_model');
	   		}
	   		else
	   		{
	   			$c = __CLASS__;
	   		}
	   				   
		   self::$instance = new $c;
	   }
	
	   return self::$instance;
	}
	
	function __construct()
	{
		$this->levels = array('block', 'view', 'edit', 'approve');
	}
	
	public function __clone()
	{
	   trigger_error('Clone is not allowed.', E_USER_ERROR);
	}
	
	public function security_as_xml()
	{		
		return '<security>
				<can_view>'.$this->can_view().'</can_view>
				<can_edit>'.$this->can_edit().'</can_edit>
				<can_approve>'.$this->can_approve().'</can_approve>
				</security>';
	}
	
	//can_view checks can_view permissions first if false it checks for can_edit and returns the result of that.
	public function is_admin()
	{
		return User::Singleton()->is_admin();
	}
	
	/**
	 * can_view a proxy to can_do
	 */
	public function can_view($page_id = NULL, $user_id = NULL)
	{
		return $this->can_do('view', $page_id, $user_id);
	}
	
	/**
	 * can_edit a proxy to can_do
	 */
	public function can_edit($page_id = NULL, $user_id = NULL)
	{
		return $this->can_do('edit', $page_id, $user_id);
	}
	
	/**
	 * can_approve a proxy to can_do
	 */
	public function can_approve($page_id = NULL, $user_id = NULL)
	{
		return $this->can_do('approve', $page_id, $user_id);
	}
	
	/**
	 * can_approve_element
	 * Looks up elements main page and calls can_approve
	 */
	public function can_approve_element($element_id = NULL, $user_id = NULL)
	{
		$page = Database::Singleton()->sql_select_first(array('page_id'), 'tbl_page_elements', array('element_id', 'main'), array($element_id, 1));
		return $this->can_approve($page['page_id']);
	}
	
	/**
	 * can_edit_element
	 * Looks up elements main page and calls can_edit on page id
	 */
	public function can_edit_element($element_id, $user_id = NULL)
	{
		$page = Database::Singleton()->sql_select_first(array('page_id'), 'tbl_page_elements', array('element_id', 'main'), array($element_id, 1));
		return $this->can_edit($page['page_id']);
	}
	
	protected function can_do($what, $page_ids = NULL, $user_id = NULL)
	{
		$this->is_level($what);
		
		if($user_id == NULL)
		{
			if($this->is_admin())
			{
				return true;
			}
			$user_id = User::Singleton()->id();
			$lists = User::Singleton()->lists(); 
		}
		else
		{
			//TODO write get lists for user function
			$lists = array();
		}
		
		/**
		 * No page id has been passed in
		 */
		if($page_ids == NULL or $page_ids == '')
		{
			$page_ids = currentPage::Singleton()->page_path();
		}
		/**
		 * A page id has been passed in but it is not an array so get its parents for inherited security
		 */
		if(!is_array($page_ids))
		{
			$pages = currentPage::Singleton()->get_page_parents($page_ids);
			array_push($pages, $page_ids);
			$page_ids = $pages;
		}
		
		$page_security_xml = Database::Singleton()->sql_select(array('data'), 'tbl_security', array('page_id'), array($page_ids));

		$security_array = Common::xml_to_array($this->default_security_xml());
		if(isset($page_security_xml[0]))
		{
			//Security has been set
			foreach($page_security_xml as $xml)
			{
				if(strlen($xml['data']) > 0)
				{
					$security_array = array_merge_recursive($security_array, Common::xml_to_array($xml['data']));
				}			
			}
		}

		/**
		 * Check for blocked first
		 */
		
		if(isset($security_array['blocked']['user'][0]))
		{
			if (in_array($user_id, $security_array['blocked']['user'])) 
			{
	    		return false;
			}
		}
		
		if(isset($lists[0]))
		{
			if(isset($security_array['blocked']['group'][0]))
			{
				if(count(array_intersect($lists, $security_array['group']['group'])) > 0)
				{
					return false; 
				}
			}
		}
		
		/**
		 * The user has not been blocked check the specific security
		 * If view and view array is not set then return true ie public
		 */

		if($what === 'view')
		{
			if(!isset($security_array['view']['user'][0]) and !isset($security_array['view']['group'][0]))
			{
				return true;
			}
		}

		if(isset($security_array[$what]['user'][0]))
		{
			if(in_array($user_id, $security_array[$what]['user']))
			{
				return true; 
			}
		}

		if(isset($security_array[$what]['group'][0]) and isset($lists[0]))
		{
			if(count(array_intersect($lists, $security_array[$what]['group'])) > 0)
			{
				return true; 
			}
		}
		return false;
	}
	
	public function get_who_can_do($what, $page_ids = NULL)
	{
		if($this->is_level($what))
		{
			return 'EVERYONE';
		}
		return NULL;
	}
	
	public function get_security($what, $page_id = NULL, $parent = 0 )
	{
		//The site specific security return. Redefined in site specific class. 
	}

	public function can_view_link($page)
	{
		if(isnumeric($page))
		{
			return Databases::singleton()->database($GLOBALS['cms_db_name'])->table('tbl_pages')->get_record_by_id($page);
		}
		else
		{
			return false;
		}
	}

	public function can_view_element($element_id)
	{
		$page = Databases::singleton()->database($GLOBALS['cms_db_name'])->table('tbl_link_page_elements')->get_records_by_equalto_query(array('element_id', 'main'), array(array($element_id), array(1)), array('page_id'));
		return $this->can_do('view', $page[0]['page_id']);
	}
	
	protected function default_security_xml()
	{
		return '<security><view><user/><group/></view><edit><user/><group/></edit><approve><user/><group/></approve><blocked><user/><group/></blocked></security>';
	}

	private function is_level($what)
	{
		if(!in_array($what, $this->levels) )
		{
			return false;
//			error('xx', $what.' is not a supported security level');
		}
		return $what;
	}
}

?>