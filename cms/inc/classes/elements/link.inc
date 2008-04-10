<?
class elementLink extends elementDefault
{

	function __construct()
	{
		$this->type = __CLASS__;
		$this->name = 'Link Element';
		$this->image = 'link.png';
		$this->allowed_tags(array('text', 'href'));
	}
}


?>