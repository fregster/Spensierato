<?php
element::register('Text Article', 'elementText');
class elementText extends elementDefault
{
	function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Text Article';
		$this->image = 'book_white';
		$this->image_edit = 'book_edit';
		$this->allowed_tags(array('html' => 'html'));
	}
	
}
?>