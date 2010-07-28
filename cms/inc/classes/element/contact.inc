<?php
element::register('Page Contact', 'elementContact', false, 'user', 'user_edit');
class element_Contact extends element_Default
{
	public function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Page Contact';
		$this->allowed_tags(array('name' => 'text', 'email' => 'email', 'userid' => 'numeric'));
	}

	protected function attach_extra_data()
	{
		//return array('title' => '');
		return array();

	}

}