<?php
/**
 * currentPage class
 * This class is for the working out what the current URL page should be doing
 * This is not specific to HTML pages just working out what the page request is
 *
 */
class currentPage
{
	private static $instance;
	private $page_details;
	private $page_type;
	private $id;
	private $path;
	private $xslt = false;

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
		//Work out what the current page is
		$this->check_page();

		$this->id = isset($this->page_type['value'])?$this->page_type['value']:NULL;

		//Get current page details
		$this->get_page_details();

		if(get::variable('xml', 'int') == 2 || Settings::singleton()->get_setting('send_as_xml') == true)
		{
			$this->xslt = true;
		}
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
		return cmspage::Singleton()->page_information();
	}

	public function id()
	{
//		if (isset($this->page_type['value'])) {
//			return $this->page_type['value'];
//		}
//		return NULL;
		return cmspage::Singleton()->id();
	}

	public function title()
	{
		return cmspage::Singleton()->title();
	}

	public function type()
	{
		return $this->page_type['type'];
	}

	private function get_page_details()
	{
		if(isset($this->id) && is_numeric($this->id))
		{
			$this->page_details = cmspage::Singleton()->page_information();	
		}
	}

	private function command_lookup($command)
	{
		$command = Database::Singleton()->sql_select_first(array('type', 'instruction'), 'tbl_commands', array('command'), array($command));
		if(is_array($command))
		{
			if(isset($command['type']) && isset($command['instruction']))
			{
				switch($command['type'])
				{
					case 'redir':
						common::redirect($command['instruction']);
						break;
				
					default:
						return false;
					break;
				}
			}
		}
		return false;
	}

	private function check_page()
	{
		//Page id checking, if not set goto homepage
		if(is_null(CMS::Singleton()->path_vars(0)) || CMS::Singleton()->path_vars(0) == 'home')
		{
			//$path_vars is not set so load an empty page from page
			$this->page_type['type'] = 'html';
			$this->page_type['value'] = NULL; //FIXME This is depreciated the cmspage should work it out
			CMS::Singleton()->page_details = $this->page_type; //FIXME hack to fix looping

			cmspage::Singleton()->initalise();
		}
		else
		{
			if(!is_numeric(CMS::Singleton()->path_vars(0)))
			{
				//FIXME update filenames / merge or change request types etc
				$match = array(
					'tool'=>'tools',
					'loading.html'=>'loading',
					'video'=>'stream',
					'js-combine' => 'javascript',
					'robots.txt' => 'robots',
				);

				$this->page_type['type'] = strtolower(CMS::Singleton()->path_vars(0));
				if(isset($match[$this->page_type['type']]))
				{
					$this->page_type['type'] = $match[$this->page_type['type']];
				}

				CMS::Singleton()->page_details = $this->page_type;
				if(!cmspage::Singleton()->initalise())
				{
					$command = $this->command_lookup(CMS::Singleton()->path_vars(0));
					if(!$command)
					{
						Error::do404();
					}
					$this->page_type['type'] = $command['type'];
					$this->page_type['value'] = $command['page'];
					CMS::Singleton()->page_details = $this->page_type;
				}
			}
			else
			{
				//pat_vars[0] is numeric
				$this->page_type['type'] = 'html';
				$this->page_type['value'] = CMS::Singleton()->path_vars(0);
				CMS::Singleton()->page_details = $this->page_type;

				if(!cmspage::Singleton()->initalise())
				{
					Error::do404();
				}
			}
		}
	}

	public function page_path()
	{
		if(isset($this->path[0]))
		{
			return $this->path;
		}

		$this->path = $this->get_page_parents($this->id);
		array_push($this->path, $this->id);
		return $this->path;
	}

	/**
	 * 
	 * Function replaced in menu
	 * @param unknown_type $id
	 */
	public function get_page_parents($id)
	{
		if($id != 0)
		{
			return menu::Singleton()->get_path_to($id);
		}

		return array();
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
			error::do404();
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
		if(!isset($id) || is_numeric($id))
		{
			return false;
		}
		$page = Database::Singleton()->sql_select_first(NULL, 'tbl_pages', array('id'), array($id));
		return $page;

	}

	public function xslt()
	{
		return $this->xslt;
	}
}