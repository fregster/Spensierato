<?php
if(class_exists('element_Download'))
{
	element::register('Flash', 'element_Flash', false, 'page_white_acrobat');
	class element_Flash extends element_Download
	{
		public function __construct()
		{
			parent::__construct();
			$this->type = __CLASS__;
			$this->allowed_tags(array('file' => 'file', 'filename' => 'auto', 'filesize' => 'auto', 'mime' => 'auto', 'description' => 'html', 'linktext' => 'auto'));
			$this->name = 'Flash';
		}

		protected function add_header()
		{
			$js = Settings::singleton()->get_setting('site_folder_js').'swfobject.js';
			content::singleton()->add_custom_head('script', array('type' => 'text/javascript', 'src'=> $js));
		}
	}
}