<?
class Content
{
	protected static $instance;
	private $extra_elements;
	private $lastmodified = 0;
	private $e_string = '';
	private $dom;

	private $page;
	private $page_data;
	
	private $content;
	private $content_data;
	
	private $menu;
	private $menu_data;
	
	private $settings;
	private $settings_data;
	
	private $security;
	private $security_data;
	
	private $debug;
	private $debug_data;
	
	private $warnings;
	private $warnings_data;
	
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
		$this->elements_attached = false;
		$this->dom = new DOMDocument();
		$this->dom->formatOutput = true;
		
		$this->page = $this->dom->createElement('page');
		$this->page_data = $this->dom->createDocumentFragment();
		
		$this->menu = $this->dom->createElement('menu');
		$this->menu_data = $this->dom->createDocumentFragment();
	
		$this->content = $this->dom->createElement('body');
		$this->content_data = $this->dom->createDocumentFragment();
		
		$this->security_data = $this->dom->createDocumentFragment();
		$this->settings_data = $this->dom->createDocumentFragment();
		$this->user_data = $this->dom->createDocumentFragment();
		$this->options_data = $this->dom->createDocumentFragment();
		$this->warnings_data = $this->dom->createDocumentFragment();
		
		$this->debug = $this->dom->createElement('debug');
		$this->debug_data = $this->dom->createDocumentFragment();
		
		$this->set_attribute('title', currentPage::Singleton()->title());
		$this->set_attribute('visible', 1);
		$this->set_attribute('deleted', 0);
		$this->set_attribute('content', 'blank');
		$this->set_attribute('render_mode', Render::Singleton()->renderMode());
		$this->set_attribute('id', currentPage::Singleton()->id());
		
	}
	
	public function build()
	{
		//Call the template class as it loads the template which might make changes to the settings
		$template = Template::Singleton();
		
		$this->attach_menu();	
		$this->load_elements();
		$this->attach_settings();
		$this->attach_user();
		$this->attach_debug();
		$this->attach_security();
		$this->attach_options();
		$this->attach_warnings();
		$this->build_xml();
		
		return $this->dom->saveXML();
	}
	
	public function last_updated()
	{
		return $this->lastmodified;
	}
	
	public function e_string()
	{
		return $this->e_string;
	}
	
	public function set_attribute($name, $value)
	{
		$this->page->setAttribute($name, utf8_encode($value));		
	}
	
	private function attach_menu()
	{
		$this->menu_data->appendXML(Menu::Singleton()->build());
		$this->menu->appendChild($this->menu_data);
	}
	
	private function attach_security()
	{
		$this->security_data->appendXML(Security::Singleton()->security_as_xml());
	}
	
	private function attach_debug()
	{
		$this->debug_data->appendXML(Debug::Singleton()->render());
		$this->debug->appendChild($this->debug_data);
	}
	
	private function attach_settings()
	{
		$this->settings_data->appendXML(Settings::Singleton()->settings_as_xml());
	}
	
	private function attach_warnings()
	{
		$this->settings_data->appendXML(Warnings::Singleton()->get_as_xml());
	}
	
	private function attach_user()
	{
		$this->user_data->appendXML(User::Singleton()->user_details_as_xml());
	}
	
	private function attach_options()
	{
		$this->options_data->appendXML(options::Singleton()->options_as_xml());
	}
	
	public function attach_content($xml)
	{
		$xml = str_replace("<?xml version=\"1.0\"?>", '', $xml);
		$this->content_data->appendXML($xml);
		$this->content->appendChild($this->content_data);
		$this->elements_attached = true;
	}
	
	private function build_xml()
	{
		$this->page->appendChild($this->menu);
		$this->page->appendChild($this->content);
		$this->page->appendChild($this->settings_data);
		$this->page->appendChild($this->user_data);
		$this->page->appendChild($this->security_data);
		$this->page->appendChild($this->options_data);
		$this->page->appendChild($this->debug);
		$this->dom->appendChild($this->page);
	}
	
	private function load_elements()
	{
		//Only load elements from DB if page type = html
		if(CMS::Singleton()->page_details['type'] == 'HTML')
		{
			$elements = Database::Singleton()->sql_select(array('*'), 'tbl_page_elements', array('page_id'), array(currentPage::Singleton()->id()), array('sort'));
			$id_string = '';
			
			if(isset($elements[0]))
			{
				foreach($elements as $node)
				{
					$data = Database::Singleton()->sql_select_first(array('*'), 'tbl_elements', array('id'), array($node['element_id']));
					$element = Element::type($data['type']);
					$element->build($node['element_id']);
					
					$this->lastmodified = max($this->lastmodified, array($data['edited'],$data['created']));
					$this->e_string .=$node['element_id'];
				}
			}
			else
			{
				if($this->elements_attached != true)
				{
					//$this->insert_xml(Render::Singleton()->render_element('STRING', 'There are no elements on this page yet'));
					$element = Element::type('elementText');
					$element->build('blank');
				}
			}
			$this->set_attribute('lastmodified', $this->lastmodified); 
		}
	}
}
?>