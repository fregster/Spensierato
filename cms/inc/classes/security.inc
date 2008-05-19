<?
class Security
{
	protected static $instance;
	
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
				</security>';
	}
	
	//can_view checks can_view permissions first if false it checks for can_edit and returns the result of that.
	public function can_view($page_id = NULL, $user_id = NULL)
	{
		if($this->can_do('view', $page_id, $user_id) != true)
		{
			//can_do('view') permission was false, check for higher permissions and return the result
			return $this->can_edit($page_id, $user_id);
		}
		else
		{
			//can_do was true
			return true;
		}
	}
	
	public function can_edit($page_id = NULL, $user_id = NULL)
	{
		if($user_id == NULL) { $user_id = $_SESSION['user_id']; }
		//die(get_class($this));
		return $this->can_do('edit', $page_id, $user_id);
	}
	
	public function can_edit_element($element_id)
	{
		return true;
//		$page = Database::Singleton()->sql_select_first(array('main'), 'tbl_element', array('id'), array($element_id));
//
//		return $this->can_do('edit', $page[0]['page_id']);
	}
	
	protected function can_do($what, $page_ids = NULL, $user_id = NULL)
	{
		$options = array('edit', 'view');
		
		if(!in_array($what, $options) )
		{
			error('xx', 'You can not call that');
		}
		return true;
	}
	
	public function get_who_can_do($what, $page_ids = NULL)
	{
		return 'EVERYONE';
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

}

?>