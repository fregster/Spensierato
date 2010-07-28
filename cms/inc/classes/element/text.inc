<?php
element::register('Text Article', 'elementText', false, 'book', 'book_edit');
class element_Text extends element_Default
{
	public function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Text Article';
		$this->allowed_tags(array('html' => 'html'));
	}

}