<?php
element::register('Tag Tree', 'element_TagTree');
class element_TagTree extends element_Default
{

	public function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Tag Tree';
		$this->allowed_tags(array('text' => 'text'));
		if (options::singleton()->get_option('admin_mode')) {
			$this->admin_mode_data['image'] = 'link';
			$this->admin_mode_data['image_edit'] = 'link_edit';
		}
	}

}