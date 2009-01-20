<?php
element::register('Link', 'elementLink');
class elementLink extends elementDefault
{

	function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Link';
		$this->image = 'link';
		$this->image_edit = 'link_edit';
		$this->allowed_tags(array('text' => 'text', 'href' => 'href'));
	}

}
?>