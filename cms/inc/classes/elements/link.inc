<?
element::register('Link', 'elementLink');
class elementLink extends elementDefault
{

	function __construct()
	{
		$this->type = __CLASS__;
		$this->name = 'Link';
		$this->image = 'link.png';
		$this->allowed_tags(array('text' => 'text', 'href' => 'href'));
		$this->edit_height = 600;
	}
}


?>