<?
class elementImage extends elementDownload
{	
	function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Image Element';
		$this->image = 'image.png';
		$this->allowed_tags(array('href' => 'href'));
	}
}


?>