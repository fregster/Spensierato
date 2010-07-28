<?php
element::register('Link', 'element_Link', false, 'link');
class element_Link extends element_Default
{

	public function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Link';
		$this->allowed_tags(array('text' => 'text', 'href' => 'href'));
		if (options::singleton()->get_option('admin_mode')) {
			$this->admin_mode_data['image'] = 'link';
			$this->admin_mode_data['image_edit'] = 'link_edit';
		}
	}

}