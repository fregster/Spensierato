<?
class elementText extends elementDefault
{
	function __construct()
	{
		$this->type = __CLASS__;
		$this->name = 'Text Element';
		$this->image = 'text.png';
		$this->allowed_tags(array('html'));
		$this->edit_height = 600;
	}
}


?>