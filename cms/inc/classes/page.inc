<?php
/**
 * Class Page
 * Use this to edit all page details
 * Current page functions should be moved here
 *
 */
class page
{
	private static $instance;
	private $id;
	private $page_details;
	
	function __construct()
	{
		$this->load(currentPage::Singleton()->id());
	}
	
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
	
	public function load($id = NULL)
	{
		if($id == NULL or !is_numeric($id))
		{	
			if(Settings::Singleton()->get_setting('homepage'))
			{
				$this->id = Settings::Singleton()->get_setting('homepage');
			}
			else
			{
				$value = database::singleton()->sql_select_first(array('id'), 'tbl_pages', NULL, NULL, array('id'), 'ASC');
				$this->id = $value['id'];
			}
		}
		else
		{
			$this->id = $id;
		}
		
		//Get current page details
		$this->page_details = Database::Singleton()->sql_select_first(array('*'), 'tbl_pages', array('id'), array($this->id));
		
		return $this->id;
	}
	
	
	public function id()
	{
		return $this->id;
	}
	
	public function title()
	{
		return $this->page_details['title'];
	}
	
	public function get_page_parents() 
	{
		if($this->id != 0)
		{
			$parents_exist = true;
			$parents = array();
			$current_page = $this->id;
				
			while($parents_exist == true) 
			{
				$result = Database::Singleton()->sql_select_first(array('id', 'parentid'), 'tbl_pages', array('id'), array($current_page));
		
				$id = $result['parentid'];
				
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
}
?>