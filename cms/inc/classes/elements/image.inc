<?php
if(class_exists('elementDownload'))
{
	element::register('Image', 'elementImage');
	class elementImage extends elementDownload
	{	
		function __construct()
		{
			parent::__construct();
			$this->type = __CLASS__;
			$this->name = 'Image';
			$this->image = 'image.png';
			$this->image_edit = '';
			$this->allowed_tags(array('href' => 'href'));
		}
	
	}
}
?>