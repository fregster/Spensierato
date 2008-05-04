<?
element::register('Text Article', 'elementText');
class elementText extends elementDefault
{
	function __construct()
	{
		$this->type = __CLASS__;
		$this->name = 'Text Article';
		$this->image = 'text.png';
		$this->allowed_tags(array('html' => 'html'));
		$this->edit_height = 600;
	}
	
}
?>