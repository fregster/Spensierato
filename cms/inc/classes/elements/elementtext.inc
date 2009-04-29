<?php
element::register('Text Article', 'elementText');
class elementText extends elementDefault
{
	public function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Text Article';
		$this->allowed_tags(array('html' => 'html'));
		$this->admin_mode_data['image'] = 'book_white';
		$this->admin_mode_data['image_edit'] = 'book_edit';
	}

}
?>