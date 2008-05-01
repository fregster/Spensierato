<?
class elementDefault
{
	protected $id;
	protected $data;
	protected $details;
	protected $type;
	protected $name;
	protected $image;
	protected $edit_height;
	private $tags = array('title' => 'text');
	private $element;
	
	function __construct()
	{
		$this->type = __CLASS__;
		$this->name = 'Default Element';
		$this->image = 'default.png';
		$this->edit_height = 600;
	}
	
	protected function allowed_tags($tags)
	{
		foreach($tags as $key => $tag)
		{
			$this->tags[$key] = $tag;
		}
	}

	public function build($id)
	{
		$this->id = $id;
		$this->link = Database::Singleton()->sql_select_first(array('*'), 'tbl_page_elements', array('element_id', 'main'), array($id, 1));
		$this->get_data();
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
		//Build posted data as XML structure
		$xml = '<xml>';
		foreach($_POST as $key => $value)
		{
			if($this->tags[$key] == 'html') { $value = htmlentities($value, ENT_QUOTES); }
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
				print_r($title);
				die('chicken');
				$title = $title->xml->title;				
			}
			else
			{
				$data = common::node_from_xml($xml, '/xml/'.$key);
				$built_xml .= '<'.$key.'>'.$data.'</'.$key.'>';
			}
		}
		
		if($draft)
		{
			die('draft');
			//database::Singleton()->sql_update_or_insert('cms_tbl_elements_draft', array('element_id', 'user_id'), array(1,1), array('data', 'ts'), array('html', 1234567));
			//database::Singleton()->sql_update_by_id($this->id, $table, $fields = array(), $values = array())
		}
		else
		{
			if($this->process_save($built_xml, $title));
			{
				return true;
			}
			return false;
		}
				
	}
	
	protected function process_save($xml, $title)
	{
		//TODO call search code
		$search = 'search text';
		//TODO Archiving
		
		database::Singleton()->sql_update_by_id($this->id, 'cms_tbl_elements', array('edited', 'editor', 'title'), array(time(), User::Singleton()->id(), $title));
		print_r (array(time(), User::Singleton()->id(), $title));
		die();
		database::Singleton()->sql_update_by_id($this->id, 'cms_tbl_elements_data', array('data', 'search'), array($xml, $search));
		
		return true;
	}
	
	//add extra data when in admin mode
	public function admin()
	{
		if(options::Singleton()->get_option('admin_mode'))
		{
			$this->element->setAttribute('edited', '456');
			$this->element->setAttribute('editon', '123');
			$this->element->setAttribute('revision', 'draft');
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
	
	protected function get_data()
	{
		if($this->id != 'blank')
		{
			$db = Database::Singleton()->sql_select_first(array('*'), 'tbl_elements_data', array('id'), array($this->id));
			$dbxml = '<data>'.$db['data'].'</data>';
			
			if(options::Singleton()->get_option('admin_mode'))
			{
				$built_xml = '<data>';
				foreach($this->tags as $key => $value)
				{
					$data = common::node_from_xml($dbxml, '/data/'.$key);
					if($value == 'html') { $data = htmlentities($data); }
					strip_tags($data);
					$built_xml .= '<'.$key.' type="'.$value.'">'.$data.'</'.$key.'>';
				}
				$built_xml .= '</data>';
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

}
?>