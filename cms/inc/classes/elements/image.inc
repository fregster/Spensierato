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
			$this->image = 'image';
			$this->image_edit = 'image_edit';
			$this->allowed_tags(array('href' => 'href'));
		}
		
		protected function attach_upload()
		{
			if(parent::attach_upload())
			{
	//			$height = Common::get_node_from_xml($data_xml, 'height');
	//			$width = Common::get_node_from_xml($data_xml, 'width');
				return $this->save_preview();
			}
		}
		
		protected function save_preview()
		{
			return image::singleton()->generate_thumb($this->id);
		}
	}
}
?>