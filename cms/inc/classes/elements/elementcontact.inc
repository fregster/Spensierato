<?php
element::register('Page Contact', 'elementContact');
class elementContact extends elementDefault
{
	public function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Page Contact';
		$this->allowed_tags(array('name' => 'text', 'email' => 'email', 'userid' => 'numeric'));
		$this->admin_mode_data['image'] = 'user';
		$this->admin_mode_data['image_edit'] = 'user_edit';
	}

	protected function attach_extra_data()
	{
		//return array('title' => '');
		return array();

	}

}
?>