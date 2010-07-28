<?php
if(Settings::Singleton()->get_setting('uploads_enabled'))
{
	element::register('Gallery', 'element_Gallery', false, 'picture');
	class element_Gallery extends element_Image
	{
		private $video_extended_details;
		private $preview;

		public function __construct()
		{
			parent::__construct();
			$this->type = __CLASS__;
			$this->name = 'Image Gallery';
			$this->allowed_tags(array('elementid'));
		}

		protected function add_header()
		{
			$js = Settings::singleton()->get_setting('site_folder_js_direct').'imageflow.js';
			$css = Settings::singleton()->get_setting('document_root').'/skin/imageflow.css';

			content::singleton()->add_custom_head('link', array('rel' => 'stylesheet', 'type' => 'text/css', 'href' => $css, 'media'=>'screen', 'title'=>'Standard'));
			content::singleton()->add_custom_head('script', array('type' => 'text/javascript', 'src'=> $js));
		}
	}
}