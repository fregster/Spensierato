<?php
element::register('Link', 'elementLink');
class elementLink extends elementDefault
{

	public function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Link';
		$this->allowed_tags(array('text' => 'text', 'href' => 'href'));
		$this->admin_mode_data['image'] = 'link';
		$this->admin_mode_data['image_edit'] = 'link_edit';
	}

}
?>