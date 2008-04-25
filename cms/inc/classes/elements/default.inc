<?
class elementDefault
{
	protected $id;
	protected $data;
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
		$this->get_data();
		Content::Singleton()->attach_content($this->render());
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
	
	public function save($data)
	{
		//Check data
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
			
			$built_xml = '<data>';
			foreach($tags as $key => $value)
			{
				$built_xml .= '<'.$key.' type="'.$value.'">'.common::node_from_xml($dbxml, '/data/'.$key).'</'.$key.'>';
			}
			$built_xml .= '</data>';
			
			$this->data = $built_xml;
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