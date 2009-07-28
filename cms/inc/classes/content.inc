<?php
class Content
{
	private static $instance;
	private $extra_elements;
	private $lastmodified = 0;
	private $e_string = '';
	private $dom;
	private $deleted = 0;
	private $visible;

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

	private $head;
	private $head_data;

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
		$this->shoppingcart_data = $this->dom->createDocumentFragment();
		$this->warnings_data = $this->dom->createDocumentFragment();

		$this->debug = $this->dom->createElement('debug');
		$this->debug_data = $this->dom->createDocumentFragment();

		if(Options::Singleton()->get_option('admin_mode') and Security::Singleton()->can_edit())
		{
			if(Options::Singleton()->get_option('show_deleted'))
			{
				$this->deleted = array(0,1);
			}
			$this->visible = array(0, 1);
		}
		else
		{
			$this->visible = 1;
			$this->deleted = 0;
		}

		//TODO update to use page class when I upload it!
		$page_details = currentPage::Singleton()->page_details();
		$page_info = currentpage::Singleton()->page_information();
		$this->set_attribute('title', cmspage::Singleton()->title());
		$this->set_attribute('visible', $page_info['visible']);
		$this->set_attribute('deleted', $page_info['deleted']);
		$this->set_attribute('content', 'blank');
		$this->set_attribute('render_mode', Render::Singleton()->renderMode());
		$this->set_attribute('id', cmspage::singleton()->id());
		$this->set_attribute('type', $page_details['type']);

	}

	public function add_custom_head($tag, $attributes, $value = NULL)
	{
		if(!is_array($this->head))
		{
			$this->head = array();
		}
//		$atrib = array();
//		foreach($attributes as $key => $value)
//		{
//			array_push($atrib, array('attrib' => $key, 'value' => $value));
//		}
		$values = array('tag' => $tag, 'attributes' => $attributes, 'value'=>$value);

		//FIXME Dirty hack to prevent values been entered multiple time, such as  2 video elements on 1 page
		$this->head[md5(serialize($values))] = $values;
	}

	public function build()
	{
		//Call the template class as it loads the template which might make changes to the settings
		$template = Template::Singleton();

		$this->attach_menu();
		$this->load_elements();
		$this->attach_head();
		$this->attach_settings();
		$this->attach_user();
		$this->attach_security();
		$this->attach_options();
		$this->attach_cart();
		$this->attach_warnings(); //This should be the last but one
		$this->attach_debug(); //This should be the last one
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
	
	private function attach_cart()
	{
		if(Settings::singleton()->get_setting('shop') && shoppingcart::get_number_of_items())
		{
			$this->shoppingcart_data->appendXML(shoppingcart::get_cart());
			$this->lastmodified = max($this->lastmodified, shoppingcart::lastmodified());
		}
	}

	private function attach_head()
	{
		if(isset($this->head))
		{
			$head = array();
			foreach($this->head as $value)
			{
				array_push($head, $value);
			}

			Settings::Singleton()->new_setting('header_extra', $head);
		}
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
		if(isset($this->shoppingcart_data))
		{
			@$this->page->appendChild($this->shoppingcart_data);
		}
		if(isset($this->head_data))
		{
			$this->page->appendChild($this->head_data);
		}
		$this->page->appendChild($this->debug);

		$this->dom->appendChild($this->page);
	}

	private function load_elements()
	{
		//Only load elements from DB if page type = html
		if(CMS::Singleton()->page_details['type'] == 'html')
		{
			$elements = Database::Singleton()->sql_select(array('*'), 'tbl_page_elements', array('page_id', 'visible'), array(cmspage::Singleton()->id(), $this->visible), array('id'), array('sort'), options::Singleton()->get_option('admin_mode'));
			$id_string = '';

			if(isset($elements[0]))
			{
				$ids = array();
				foreach($elements as $id)
				{
					$ids[] = $id['element_id'];
				}

				$fields = array('id', 'visible', 'deleted');
				$values = array($ids, $this->visible, $this->deleted);

				//display deleted elements in admin mode
				if (options::Singleton()->get_option('admin_mode')) {
					array_pop($fields);
					array_pop($values);
				}

				$data = Database::Singleton()->sql_select(array('*'), 'tbl_elements', $fields, $values, array('id'), NULL, options::Singleton()->get_option('admin_mode'));
				$element_data = Database::Singleton()->sql_select(array('*'), 'tbl_elements_data', array('id'), array($ids), array('id'), NULL, options::Singleton()->get_option('admin_mode'));

				if(isset($data[0]))
				{
					foreach($data as $key => $data)
					{
						if(isset($element_data[$key]) && $element_data[$key] != NULL)
						{
							$element = Element::type($data['type']);
							$element->build($elements[$key]['element_id'], $element_data[$key]);
	
							$this->lastmodified = max($this->lastmodified, $data['edited'], $data['created'], $elements[$key]['ts']);
							$this->e_string .=$elements[$key]['element_id'];
						}
					}
				}
			}
			else
			{
				if(($this->elements_attached != true) and (!options::singleton()->get_option('admin_mode')))
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