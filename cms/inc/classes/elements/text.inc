<?
element::register('Text Article', 'elementText');
class elementText extends elementDefault
{
	function __construct()
	{
		$this->type = __CLASS__;
		$this->name = 'Text Article';
		$this->image = 'page_white';
		$this->image_edit = 'page_edit';
		$this->allowed_tags(array('html' => 'html'));
		$this->edit_height = 600;
	}
	
}
?>