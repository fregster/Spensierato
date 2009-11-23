<?php
if(class_exists('element_Download'))
{
	element::register('Flash', 'element_Flash');
	class element_Flash extends element_Download
	{
		public function __construct()
		{
			parent::__construct();
			$this->type = __CLASS__;
			$this->allowed_tags(array('file' => 'file', 'filename' => 'auto', 'filesize' => 'auto', 'mime' => 'auto', 'description' => 'html', 'linktext' => 'auto'));
			$this->name = 'Flash';
			if (options::singleton()->get_option('admin_mode')) {
				$this->admin_mode_data['image'] = 'disk';
				$this->admin_mode_data['image_edit'] = '';
			}
		}

		protected function add_header()
		{
			$js = Settings::singleton()->get_setting('site_folder_js').'swfobject.js';
			content::singleton()->add_custom_head('script', array('type' => 'text/javascript', 'src'=> $js));
		}
	}
}