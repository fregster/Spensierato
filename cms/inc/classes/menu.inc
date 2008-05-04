<?
class Menu
{
	protected static $instance;
	
	private $menu_xml;
	private $path;
	
	//vars needed to work out XML
	private $current_page; //id
	private $current_page_details = array();
	private $working_page_id; //id
	private $parents = array();
	
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
		//Setup vars
		
	}
	
	//Build the xml for the menu
	public function build()
	{
		//Get current page details
		$this->current_page = currentPage::Singleton()->id();
		$this->current_page_details = currentPage::Singleton()->page_information();
		
		//Get path to current page
		$this->path = array_reverse($this->get_path());
		
		//Get the parents of the current page
		$this->get_parents();
		
		//Get top level links of current tree
		
		//Get children of current page
		$this->children = $this->get_children();
		
		//Get grandchildren of current page
		
		
		//Build complete menu array
		$menu_array['top'] = $this->get_children_of($this->path[0]);
		
		//Test if we are on a top page and so don't need parents and parallel links
		$toppage = false;
		if(is_array($menu_array['top']))
		{
			foreach($menu_array['top'] as $top)
			{
				if($top['id'] == $this->current_page)
				{
					$toppage = true;
				}
		}
		}
		if(!$toppage)
		{
			$menu_array['parents'] = array_reverse($this->parents);
			$menu_array['parallel'] = $this->get_parallel_links();
			$menu_array['current'] = $this->current_page_details;

		}
		$menu_array['children']= $this->children;
		
		$serializer_options = array ( 
			   'addDecl' => FALSE, 
			   'encoding' => Settings::Singleton()->get_setting('char_set'), 
			   'indent' => '  ',
			   'rootName'  => 'menu',
			   'defaultTagName'  => 'menu_item'
			);

		$this->menu_xml = Common::array_to_xml($menu_array, $serializer_options);
		return $this->menu_xml;
	}
	
	public function get_path()
	{
		return $this->get_path_to($this->current_page); 
	}
	
	public function get_path_to($id)
	{
		//The id of the current page.
		$current_record = Database::Singleton()->sql_select_first(array('parent_id'), 'tbl_pages', array('id'), array($id));

		//The array of ids to the current page. 
		$path = array();
		while(true)
		{	
			$record = Database::Singleton()->sql_select_first(array('parent_id'), 'tbl_pages', array('id'), array($id));
			if(is_null($record['parent_id']))
			{		
				array_push($path,$id);
				return $path;
			}
			else
			{
				array_push($path,$id);
				$id = $record['parent_id'];
			}
		}
	}
	
	private function get_children()
	{
		return $this->get_children_of($this->current_page);
	}
	
	private function get_parents()
	{
		//set working page
		if(!isset($this->working_page_id))
		{
			$this->working_page_id = $this->current_page_details['parent_id'];
		}
		
		$this_page_parent_details = $this->get_page_details($this->working_page_id);

		//Do security check and do not push if failed
		if(Security::Singleton()->can_view($this_page_parent_details['id']))
		{
			
			if(in_array($this_page_parent_details['id'], $this->path))
			{
				$this_page_parent_details['selected'] = true;
			}
			
			if($this_page_parent_details['parent_id'] != 0)
			{	
				array_push($this->parents, $this_page_parent_details);
			}
			
			$this->working_page_id = $this_page_parent_details['parent_id'];
			
			if($this->working_page_id != 0)
			{
				$this->get_parents();
			}
			else
			{
				$this->get_children_of($this->working_page_id['parent_id']);
			}
		//end security check
		}
	}
	
	private function get_parallel_links()
	{
		//if($this->current_page_details['parent_id'] != 0)
		//{
			$parallel = $this->get_children_of($this->current_page_details['parent_id']);
			
			$i=0;
			foreach($parallel as $link)
			{
				if($link['id'] == $this->current_page)
				{
					unset($parallel[$i]);
					break;
				}
				$i++;
			}
			return $parallel;
		//}
	}
	
	private function get_page_details($id)
	{
		return Database::Singleton()->sql_select_first(array('title', 'parent_id', 'visible', 'deleted', 'id'), 'tbl_pages', array('id'), array($id));
		
	}
	
	private function get_children_of($id = 0)
	{
		if(options::Singleton()->get_setting('admin_mode'))
		{
			$where = array('parent_id');
			$values = array($id);
		}
		else
		{
			$where = array('parent_id', 'visible', 'deleted');
			$values = array($id, '1', '0');
		}
		$details = Database::Singleton()->sql_select(array('id', 'title', 'parent_id', 'visible', 'deleted'), 'tbl_pages', $where, $values, array('sort'));

		$i = 0;
		if(is_array($details))
		{
			foreach($details as $page)
			{
				//Do security check and remove from db returned array if failed
				if(Security::Singleton()->can_view($page['id']))
				{
					if($page['id'] == $this->current_page)
					{
						$details[$i]['current'] = true;
					}
					
					if(in_array($page['id'], $this->path))
					{
						$details[$i]['selected'] = true;
					}
				}
				else
				{
					//Remove from children as can_view failed
					unset($details[$i]);
				}
							
				$i++;
			}
		}
		return $details;
	}
}
?>