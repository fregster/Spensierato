<?php
if(class_exists('elementDownload'))
{
	element::register('Image', 'elementImage');
	class elementImage extends elementDownload
	{
		public function __construct()
		{
			parent::__construct();
			$this->type = __CLASS__;
			$this->name = 'Image';
			$this->allowed_tags(array('href' => 'href'));
			if (options::singleton()->get_option('admin_mode')) {
				$this->admin_mode_data['image'] = 'image';
				$this->admin_mode_data['image_edit'] = 'image_edit';
			}
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
			return image::generate_thumb($this->id.'-'.$this->admin_mode_data['revision']);
		}
	}
}
?>