<?
class elementDefault
{
	protected $id;
	protected $data;
	protected $type;
	protected $name;
	protected $image;
	protected $edit_height;
	private $tags = array('title');
	
	function __construct()
	{
		$this->type = __CLASS__;
		$this->name = 'Default Element';
		$this->image = 'default.png';
		$this->edit_height = 600;
	}
	
	protected function allowed_tags($tags)
	{
		foreach($tags as $tag)
		{
			array_push($this->tags, $tag);
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
		$element = $dom->createElement('element');
		$element->setAttribute('id', $this->id);
		$element->setAttribute('type', $this->type);
		$element->setAttribute('etype', $this->name);
		$element->setAttribute('eh', $this->edit_height);
		
		//Transform the data field
		$this->transform_data();
		
		//Create XML tag
		$element_data = $dom->createDocumentFragment();
		
		//Attach the data
		$element_data->appendXML($this->data);
		$element->appendChild($element_data);
		
		//Attach the element to the root dom
		$dom->appendChild($element);
		
		//Return the completed XML of the document
		return $dom->saveXML();
	}
	
	public function save($data)
	{
		//Check data
	}
	
	//Return data for editor
	public function edit()
	{
		//Create DOM
		$dom = new DOMDocument();
		$dom->formatOutput = false;
		
		//Create element and set required attributes
		$element = $dom->createElement('element');
		$element->setAttribute('id', $this->id);
		$element->setAttribute('type', $this->type);
		$element->setAttribute('etype', $this->name);
		$element->setAttribute('eh', $this->edit_height);
		$element->setAttribute('edited', '456');
		$element->setAttribute('editon', '123');
		$element->setAttribute('revision', 'draft');
		
		//Transform the data field
		$this->transform_data();
		
		//Create XML tag
		$element_data = $dom->createDocumentFragment();
		
		//Attach the data
		$element_data->appendXML($this->data);
		$element->appendChild($element_data);
		
		//Attach the element to the root dom
		$dom->appendChild($element);
		
		//Return the completed XML of the document
		return $dom->saveXML();
	}
	
	public function ajax_edit()
	{
		$dom = new DOMDocument();
		$dom->loadXML($this->data);
		$xp = new domxpath($dom);
		return htmlentities($xp->query("/data/html"));
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
			$this->data = '<data>'.$db['data'].'</data>';
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