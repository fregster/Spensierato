<?php
if(class_exists('download'))
{
	element::register('Flash', 'elementFlash');
	class elementFlash extends elementDownload
	{
		function __construct()
		{
			parent::__construct();
			$this->type = __CLASS__;
			$this->allowed_tags(array('file' => 'file', 'filename' => 'auto', 'filesize' => 'auto', 'mime' => 'auto', 'description' => 'html', 'linktext' => 'auto'));
			$this->type = __CLASS__;
			$this->name = 'Flash';
			$this->image = 'disk';
			$this->image_edit = '';
		}
	}
}
?>