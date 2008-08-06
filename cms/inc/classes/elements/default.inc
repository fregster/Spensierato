<?
class elementDefault
{
	protected $id;
	protected $data;
	protected $details;
	protected $type;
	protected $name;
	protected $image;
	protected $image_edit;
	protected $edit_height;
	protected $edit_width;
	protected $visible;
	protected $deleted;
	protected $created;
	protected $creator;
	protected $edited;
	protected $editor;
	protected $approved;
	protected $approver;
	protected $revision;
	protected $draft;
	protected $approve;
	private $tags = array('title' => 'text');
	private $element;
	
	function __construct()
	{
		$this->type = __CLASS__;
		$this->name = 'Default Element';
		$this->image = 'accept';
		$this->image_edit = 'page_edit';
		$this->edit_height = 400;
		$this->edit_width = 700;
		
		$this->add_header();
	}
	
	protected function allowed_tags($tags)
	{
		foreach($tags as $key => $tag)
		{
			if(is_numeric($key)) { $key = $tag; $tag = 'auto'; }
			$this->tags[$key] = $tag;
		}
	}

	public function create($main_page)
	{
		$time = time();
		$this->id = database::Singleton()->sql_insert('tbl_elements', array('type', 'created', 'creator'), array($this->type, $time, user::Singleton()->id()));
		$sort = database::Singleton()->sql_max_field('tbl_page_elements', 'sort', array('page_id'), array($main_page));
		
		database::Singleton()->sql_insert('tbl_page_elements', array('page_id', 'element_id', 'main','ts', 'sort'), array($main_page, $this->id, 1, $time, $sort+1));
		database::Singleton()->sql_insert('tbl_elements_data', array('id', 'data'), array($this->id, '<null />'));
		common::redirect($main_page);
	}
	
	public function build($id, $data = NULL, $link = NULL)
	{
		$this->id = $id;
		if($link == NULL)
		{
			$this->link = Database::Singleton()->sql_select_first(array('*'), 'tbl_page_elements', array('element_id', 'main'), array($id, 1));
		}
		else
		{
			$this->link = $link;
		}
		$this->get_data($data);
		Content::Singleton()->attach_content($this->render());
	}
	
	public function main()
	{
		return $this->link['page_id'];
	}
	
	public function render()
	{
		//Create DOM
		$dom = new DOMDocument();
		$dom->formatOutput = false;
		
		//Create element and set required attributes
		$this->element = $dom->createElement('element');
		$this->element->setAttribute('id', $this->id);
		$this->element->setAttribute('type', $this->type);
		$this->element->setAttribute('etype', $this->name);
		$this->element->setAttribute('eh', $this->edit_height);
		$this->element->setAttribute('ew', $this->edit_width);
		$this->element->setAttribute('image', $this->image);
		
		$this->admin();
		
		//Transform the data field
		$this->transform_data();
		
		//Create XML tag
		$element_data = $dom->createDocumentFragment();
		
		//Attach the data
		$element_data->appendXML($this->data);
		$this->element->appendChild($element_data);
		
		//Attach the element to the root dom
		$dom->appendChild($this->element);
		
		//Return the completed XML of the document
		return $dom->saveXML();
	}
	
	public function save($draft = true)
	{
		if($this->attach_upload() == false)
		{
			die('error comiting upload');
			return false;
		}
		$extra = $this->attach_extra_data();
		
		$details = array_merge($_POST, $extra);
		
		//Build posted data as XML structure
		$xml = '<xml>';
		foreach($details as $key => $value)
		{
			if($key == 'html') { $value = htmlentities($value, ENT_QUOTES); }
			$value = common::sql_clean(strip_tags($value));
			$xml .= '<'.$key.'>'.$value.'</'.$key.'>';
		}
		$xml .= '</xml>';
		
		//Check data
		foreach($this->tags as $key => $value)
		{
			if($key == 'title')
			{
				$title = new SimpleXMLElement($xml);
				$title = ''.$title->title;
				if(strlen($title) < 2)
				{
					$title = NULL;
				}
			}
			else
			{
				$data = common::node_from_xml($xml, '/xml/'.$key);
				$built_xml .= '<'.$key.'>'.utf8_encode(common::sql_clean(strip_tags(htmlentities($data, ENT_QUOTES)))).'</'.$key.'>';
			}
		}
		
		if($draft)
		{
			//database::Singleton()->sql_update_or_insert('cms_tbl_elements_draft', array('element_id', 'user_id'), array($this->id,user::Singleton()->id()), array('data', 'ts'), array(common::sql_clean(strip_tags(htmlentities($data, ENT_QUOTES))), CMS::Singleton()->time()));
			//database::Singleton()->sql_update_by_id($this->id, $table, $fields = array(), $values = array())
			database::Singleton()->sql_update_or_insert('tbl_elements_approve', array('element_id', 'user_id'), array($this->id, User::Singleton()->id()), array('user_id', 'element_id', 'data', 'ts', 'title'), array(User::Singleton()->id(), $this->id, $built_xml, CMS::Singleton()->time(), $title));
			die('SAVED');
		}
		else
		{
			if(Security::Singleton()->can_approve_element())
			{
				if($this->process_save($built_xml, $title));
				{
					return true;
				}
			}
			else
			{
				if($this->submit_save_for_approval($built_xml, $title));
				{
					return true;
				}
			}
			return false;
		}
				
	}
	
	protected function attach_upload()
	{
		if(isset($this->tags['file']) and isset($_FILES))
		{
			$file_size = (int)$_FILES['uploadedfile']['size'];
			$max_size = (int)Settings::Singleton()->get_setting('max_file_size');
			if($file_size <= $max_size)
			{
				$path = $_FILES['uploadedfile']['tmp_name'];
				$target = Settings::singleton()->get_setting('cms_folder_uploads').$this->id.'-'.$this->revision;
				return(filetransfer::move_file($path, $target));
			}
			return false;
		}
		return true;
	}
	
	private function save_upload()
	{
		if(isset($this->tags['file']))
		{
			return filetransfer::singleton()->save_file($this->id, $this->revision);
		}
	}
	
	
	public function toggle($what)
	{
		switch ($what)
		{
			case 'visibility':
				return( $this->toggle_visibility());
				break;

			case 'delete':
				return( $this->toggle_deleted());
				break;
			
		default:
			return false;
		}
			
	}
	
	protected function toggle_visibility()
	{
		$new_visibility = ($this->visible - 1)*(-1);
		return database::Singleton()->sql_update_by_id($this->id, 'tbl_elements', array('visible'), array($new_visibility));
	}
	
	protected function toggle_deleted()
	{
		if($this->deleted == (NULL or 0))
		{
			$this->deleted = CMS::time();
		}
		else
		{
			$this->deleted = NULL;
		}
		return database::Singleton()->sql_update_by_id($this->id, 'tbl_elements', array('deleted'), array($this->deleted));
	}
	
	protected function generate_search($input)
	{
		return strip_tags($input);
	}
	
	protected function process_save($xml, $title)
	{
		$this->save_upload();
		
		//TODO call search code
		$search = $this->generate_search($xml);
		//TODO Archiving
		
		database::Singleton()->sql_update_by_id($this->id, 'tbl_elements', array('edited', 'editor', 'title'), array(time(), User::Singleton()->id(), $title));
		database::Singleton()->sql_update_by_id($this->id, 'tbl_elements_data', array('data', 'search'), array($xml, $search));
		
		return true;
	}
	
	protected function submit_save_for_approval($xml, $title)
	{
		database::Singleton()->sql_update_or_insert('tbl_elements_approve', array('element_id', 'user_id'), array($this->id, User::Singleton()->id()), array('user_id', 'element_id', 'data', 'ts', 'title'), array(User::Singleton()->id(), $this->id, $xml, CMS::Singleton()->time(), $title));	
		return true;
	}
	
	//add extra data when in admin mode
	public function admin()
	{
		if(options::Singleton()->get_option('admin_mode'))
		{
			$this->get_revision();
			$this->element->setAttribute('created', $this->created);
			$this->element->setAttribute('creator', $this->creator);
			if($this->edited != NULL)
			{
				$this->element->setAttribute('edited', $this->edited);
				$this->element->setAttribute('editor', $this->editor);
			}
			if(isset($this->$approved))
			{
				$this->element->setAttribute('approved', $this->$approved);
			}
			
			if(isset($this->$approver))
			{
				$this->element->setAttribute('approver', $this->$approver);
			}
			
			$this->element->setAttribute('revision', $this->revision);
			$this->element->setAttribute('draft_available', $this->draft);
			$this->element->setAttribute('new_v_available', $this->approve);
			$this->element->setAttribute('image_edit', $this->image_edit);
			$this->element->setAttribute('visible', $this->visible);
			$this->element->setAttribute('deleted', $this->deleted);
		}
	}
	
	public function ajax_edit()
	{
		return common::node_from_xml($this->data, '/data/html');
	}
	
	//Over right this function in each element to transform the data XML to what ever needs to be sent to the XSLT processor
	protected function transform_data()
	{
		//$this->data = $this->data;
	}
	
	protected function get_data($data = NULL)
	{
		if($this->id != 'blank')
		{
			if($data == NULL)
			{
				$db = Database::Singleton()->sql_select_first(array('*'), 'tbl_elements_data', array('id'), array($this->id), NULL, NULL, options::Singleton()->get_option('admin_mode'));
				$data = $db['data'];
			}

			$dbxml = '<data>'.$data.'</data>';
			
			if(options::Singleton()->get_option('admin_mode'))
			{
				$built_xml = '<data>';
				foreach($this->tags as $key => $value)
				{
					//If can approve then check if there outstanding approvals waiting or
					//Check if the user has any outstanding approvals waiting
					$user_awaiting_approval = Database::Singleton()->sql_select_first(array('id'), 'tbl_elements_approve', array('element_id', 'user_id'), array($this->id, user::Singleton()->id()), NULL, NULL, TRUE);
					$user_draft = Database::Singleton()->sql_select_first(array('ts'), 'tbl_elements_draft', array('element_id', 'user_id'), array($this->id, user::Singleton()->id()), NULL, NULL, TRUE);
					
					if(Security::Singleton()->can_approve_element($this->id) )
					{
						$can_approve = Database::Singleton()->sql_select_first(array('ts'), 'tbl_elements_approve', array('element_id'), array($this->id), NULL, NULL, TRUE);
					}
					
					if(isset($can_approve) or (isset($user_awaiting_approval)) )
					{
						$this->approve = true;
					}
					
					if(isset($user_draft['ts']))
					{
						$this->draft = $user_draft['ts'];
					}
					if($key == 'title')
					{
						$data = Database::Singleton()->sql_select_first(array('title', 'visible', 'deleted', 'created', 'creator', 'edited', 'editor', 'approved', 'approver'), 'tbl_elements', array('id'), array($this->id), NULL, NULL, TRUE);
						
						$this->visible = $data['visible'];
						$this->deleted = $data['deleted'];
						$this->created = $data['created'];
						$this->creator = $data['creator'];
						$this->edited = $data['edited'];
						$this->editor = $data['editor'];
						$this->approved = $data['approved'];
						$this->aprover = $data['aprover'];
						$data = $data['title'];

					}
					else
					{
						$data = common::node_from_xml($dbxml, '/data/'.$key);
					}
					
					if($value == 'html') { $data = htmlentities($data); }
					$data = utf8_decode(strip_tags($data));
					$built_xml .= '<'.$key.' type="'.$value.'">'.$data.'</'.$key.'>';
				}
				$built_xml .= '</data>';
				//die($built_xml);
				$this->data = $built_xml;
			}
			else
			{
				$this->data = $dbxml;
			}
			
			
			$this->search = $db['search'];
		}
		else
		{
			$this->data = '<data>Blank</data>';
			$this->search = NULL;
		}
	}
	
	protected function get_revision()
	{
		$this->revision = Database::Singleton()->sql_count_where('tbl_element_data_archive', 'element_id', $this->id, options::Singleton()->get_option('admin_mode'));
		return $this->revision;
	}

	protected function add_header()
	{
			
	}
	
	protected function attach_extra_data()
	{
		return array();
	}
		
}
?>