<?
class elementImage extends elementDefault
{	
	function __construct()
	{
		$this->type = __CLASS__;
		$this->name = 'Image Element';
		$this->image = 'image.png';
		$this->allowed_tags(array('href' => 'href'));
		$this->edit_height = 600;
	}
}


?>