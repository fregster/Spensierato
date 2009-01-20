<?php
element::register('Page Contact', 'elementContact');
class elementContact extends elementDefault
{
	function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Page Contact';
		$this->image = 'user';
		$this->image_edit = 'user-edit';
		$this->allowed_tags(array('name' => 'text', 'email' => 'email', 'userid' => 'numeric'));
	}
	
	protected function attach_extra_data()
	{
		return array('title' => '');
		
	}

}
?>