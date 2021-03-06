<?php
class Menu
{
	private static $instance;

	private $menu_xml;
	private $path = false;
	private $path_cache = array();

	//vars needed to work out XML
	private $current_page; //id
	private $current_page_details = array();
	private $working_page_id; //id
	private $parents = array();
	
	private $options = array('get_full_menu' => false, 'top_menu_depth' => 1, 'include_breadcrumbs' => true);

	public static function Singleton()
	{
	   if (!isset(self::$instance)) {
		   $c = __CLASS__;
		   self::$instance = new $c;
	   }

	   return self::$instance;
	}

	protected function __construct()
	{
		//Setup vars
	}
	/**
	 * set_option is a method to force the menu class to do specific things
	 * Currently supported options
	 * top_menu_depth = int //Forces the menu class to get the top X level of results
	 * get_full_menu = bool //Foces the menu class to populate the XML with the complete menu (Useful for small sites and debuging)
	 */
	public function set_options($options)
	{
		if(isset($options) && is_array($options))
		{
			$this->options = array_merge($this->options, $options);
		}
	}

	//Build the xml for the menu
	public function build()
	{
		//Get current page details
		$this->current_page = cmspage::Singleton()->menu_id();

		$this->current_page_details = cmspage::Singleton()->page_information();

		//Get path to current page
		$this->get_path();

		//Get the parents of the current page
		$this->get_parents();

		//Get top level links of current tree
		if(!isset($menu_array) || !is_array($menu_array)){ $menu_array = array(); }
		
		if(isset($this->path[0]))
		{
			$menu_array['top'] = $this->get_children_of($this->path[0]);
		}
		//Get children of current page
		$this->children = $this->get_children();

		//Get grandchildren of current page

		//Build complete menu array

		//Test if we are on a top page and so don't need parents and parallel links
		$toppage = false;
		
		if($this->current_page_details['parent_id'] == 0){
			$toppage = true;
			if(!isset($menu_array['top'][0]))
			{
				$menu_array['top'][0] =& $this->current_page_details;
			}
		}
		
//NOTE: Not sure if this is needed anymore, seems to cause more problems than it was suposed to fix so I'll leave it commented out until I rember why it was here
//		if(!$toppage && isset($menu_array['top']) && is_array($menu_array['top']))
//		{
//			foreach($menu_array['top'] as $top)
//			{
//				if($top['id'] == $this->current_page)
//				{
//					$toppage = true;
//				}
//			}
//		}

		if(!$toppage)
		{
			//pop the last parent off as it should be in the top array then reverse it
			$menu_array['parents'] = array_reverse($this->parents);

			unset($menu_array['parents'][0]);

			$menu_array['parallel'] = $this->get_parallel_links();
			if(isset($this->current_page_details))
			{
				$menu_array['current'] = $this->current_page_details;
			}
		}
		
		if(isset($menu_array['top'][0]))
		{
			$menu_array['top_menu'] = $this->get_top_menu($menu_array['top'][0]['parent_id']);
		}
		
		$menu_array['full_menu'] = $this->get_full_menu();
		
		$menu_array['children'] = $this->children;
		
		$menu_array['breadcrumbs'] = $this->breadcrumbs();
		
		$serializer_options = array (
			   'addDecl' => FALSE,
			   'encoding' => Settings::Singleton()->get_setting('char_set'),
			   'indent' => '  ',
			   'rootName'  => 'menu',
			   'defaultTagName'  => 'menu_item'
			);
		$this->menu_xml = xml::array_to_xml_simple($menu_array, 'menu', 'menu_item');
		
		return $this->menu_xml;
	}

	public function get_path()
	{
		if(!$this->path)
		{
			$this->path = array_reverse($this->get_path_to($this->current_page));
			$this->path_cache[$this->current_page] &= $this->path;
		}
		return $this->path;
	}
	
	private function breadcrumbs()
	{
		if(isset($this->options['include_breadcrumbs']) && $this->options['include_breadcrumbs'])
		{
			$menu = $this->get_full_menu(true); //Load the full menu as it is probably cached
			$menu = common::retrieve_values_from_array($menu, array('id', 'title'));
			
			$menu_link_map = array();
			if(isset($menu[0]))
			{
				foreach($menu as $item)
				{
					if(strlen($item[1]) > 0)
					{
						$menu_link_map[$item[0]] = $item[1];
					}
				}
			}
			
			$xml = '';
			if(is_array($this->path) && is_array($menu_link_map))
			{
				foreach($this->path as $key => $id)
				{
					if(isset($id) && isset($menu_link_map[$id]))
					{
						$xml .= '<menu_item sort="'.$key.'" id="'.$id.'">'.$menu_link_map[$id].'</menu_item>';
					}
				}
			}
			
			return $xml;
		}
	}
	
	private function get_path_to_loop_results($id, $path = array())
	{
		$recursion = 10;
		$results = database::singleton()->sql_select_recursive('id', 'tbl_pages', 'parent_id', null, 'id', $id, $recursion);
		if(isset($results[0]) && is_array($results[0])){
			foreach($results[0] as $value)
			{
				if($value == null){ break; }
				$path[] = $value;
			}
			if(count($path) == $recursion)
			{
				return $this->get_path_to_loop_results($path[$recursion-1], $path);
			}
		}
		else {
			$path[] = $id;
		}
		$this->path_cache[$path[0]] = $path;
		return $path;
	}

	public function get_path_to($id)
	{
		if(isset($this->path_cache[$id]))
		{
			return $this->path_cache[$id];
		}
		/**
		 * Function replaced with a self joining SQL statment to significantly reduce the number of SQL statments at the cost of increased PHP SQL build statment time
		 * Previous method left in here incase there is a need to revert quickly
		 */
		return $this->get_path_to_loop_results($id);
		
		$id = intval($id);
		if(is_int($id))
		{
			//The id of the current page.
			$current_record = Database::Singleton()->sql_select_first('parent_id', 'tbl_pages', array('id'), array($id));
			//The array of ids to the current page.
			$path = array();
			$i = 0;
			while(true)
			{
				if($i != 0)
				{
					$record = Database::Singleton()->sql_select_first('parent_id', 'tbl_pages', 'id', $id);
				}
				else{
					$record =& $current_record;
				}				
				
				if(is_null($record['parent_id']) || $record['parent_id'] == $id || $record['parent_id'] == 0)
				{
					array_push($path,$id);
					return $path;
				}
				else
				{
					array_push($path,$id);
					$id = $record['parent_id'];
				}
				++$i;
			}
		}

		return array();
	}

	private function get_children()
	{
		$children = $this->get_children_of($this->current_page);
		$additional_children = $this->get_additional_children();

		if(isset($additional_children[0]))
		{
			if(!isset($children[0]))
			{
				$children = array();
			}
			$children = array_merge($children, $additional_children);
		}
		
		return $children;
	}
	
	private function get_additional_children()
	{
		$additional_children = cmspage::Singleton()->get_menu_children();
		if(isset($additional_children[0]))
		{
			$additional_children = $this->security_check(Database::Singleton()->sql_select(array('id', 'title', 'parent_id', 'visible', 'deleted'), 'tbl_pages', array('id'), $additional_children, array('sort')));
			foreach($additional_children as $key=>$child)
			{
				$additional_children[$key]['tagged_child'] = 1;
				$additional_children[$key]['parent_id'] = $this->current_page;
			}
			return $additional_children;
		}
		return array();
	}

	private function get_parents()
	{
		//set working page
		if(!isset($this->working_page_id))
		{
			$this->working_page_id = $this->current_page_details['parent_id'];
		}

		$this->page_parent_details = $this->get_page_details($this->working_page_id);

		//Do security check and do not push if failed
		if(Security::Singleton()->can_view($this->page_parent_details['id']))
		{

			if(isset($this->page_parent_details['id']) && in_array($this->page_parent_details['id'], $this->path))
			{
				$this->page_parent_details['selected'] = true;
			}


			if(isset($this->page_parent_details['parent_id']) && $this->page_parent_details['parent_id'] != 0)
			{
				$this->parents[] = $this->page_parent_details;
			}

			$this->working_page_id = $this->page_parent_details['parent_id'];

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
		$parallel = $this->get_children_of($this->current_page_details['parent_id']);

		$i=0;
		if(isset($parallel[0]))
		{
			foreach($parallel as $link)
			{
				if($link['id'] == $this->current_page)
				{
					unset($parallel[$i]);
					break;
				}
				$i++;
			}
		}
		return $parallel;
	}

	public function get_page_details($id)
	{
		if(is_int($id)){
			return Database::Singleton()->sql_select_first(array('title', 'parent_id', 'visible', 'deleted', 'id'), 'tbl_pages', array('id', 'visible', 'deleted'), array($id));
		}
		
		if(is_array($id)){
			return Database::Singleton()->sql_select(array('title', 'parent_id', 'visible', 'deleted', 'id'), 'tbl_pages', array('id', 'visible', 'deleted'), array($id, 1, 0));
		}
		
		return false;
	}

	private function get_children_of($id = 0)
	{
		$where = array('parent_id', 'product');
		$values = array($id, 0);
		if(!options::Singleton()->get_option('admin_mode') || !user::singleton()->is_logged_in())
		{
			$where[] = 'deleted';
			$values[] = '0';

			$where[] =  'visible';
			$values[] =  1;

		}
		$details = Database::Singleton()->sql_select(array('id', 'title', 'parent_id', 'visible', 'deleted', 'sort'), 'tbl_pages', $where, $values, array('sort'));

		return $this->security_check($details);
	}
	
	private function security_check($details)
	{
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

				if((options::Singleton()->get_option('admin_mode')) and ($page['visible'] != 1 or $page['deleted'] != 0) and (!Security::Singleton()->can_edit($page['id'])))
				{
					//Remove from children as can_view failed
					unset($details[$i]);
				}
				$i++;
			}
		}
		return $details;
	}
	
	public function get_full_menu($force = false)
	{
		if($this->options['get_full_menu'] || $force)
		{
			return Database::Singleton()->sql_select(array('title', 'parent_id', 'visible', 'deleted', 'id'), 'tbl_pages', array('product'), array(0));
		}
	}
	
	private function get_top_menu($ids = array(0))
	{
		if($this->options['top_menu_depth'] >= 1)
		{
			$menu = array();
//			$ids = array(0);
			$i=1;
			$where = array('parent_id', 'product', 'deleted', 'visible');
			$values = array($ids,0, 0, 1);
				
			if(options::Singleton()->get_option('admin_mode'))
			{
				array_pop($where); array_pop($where);
				array_pop($values); array_pop($values);
			}
			
			$menu[] = database::Singleton()->sql_select_first(array('title', 'parent_id', 'visible', 'deleted', 'id'), 'tbl_pages', 'id', $ids);
			
			if($ids[0] == $this->current_page_details['id'] )
			{
				$menu[0]['selected'] = true;
			}
			
			//TODO: This pulls the entire menu structure to get the top menu which is just a bit rubish, recode perhalps with different logic for different senarios
			while($i <= $this->options['top_menu_depth'])
			{
				$values[0] = $ids;
				$result = Database::Singleton()->sql_select(array('title', 'parent_id', 'visible', 'deleted', 'id'), 'tbl_pages', $where, $values);
				$ids = array();
				if(isset($result[0]))
				{
					foreach($result as $key => $res)
					{
						$ids[] = $res['id'];
						if(isset($res['id']) && in_array($res['id'], $this->path))
						{
							$result[$key]['selected'] = true;
						}
					}
					
					$result['level'] = $i;
					$menu[0] = array_merge($menu, $result);
				}
				else
				{
					//There were no children do not waste cycles fetching more menu levels
					return $menu;
				}
				++$i;
			}
			return $menu;
		}
	}
}