<?php
if(Settings::Singleton()->get_setting('uploads_enabled'))
{
	element::register('Gallery', 'element_Gallery', false, 'picture');
	class element_Gallery extends element_Default
	{
		public function __construct()
		{
			parent::__construct();
			$this->type = __CLASS__;
			$this->name = 'Image Gallery';
			$this->allowed_tags(array('files' => 'files', 'directory' => 'folder'));
			$this->options = array(
				'mode' => array('circle', 'line', 'callback'),
				'steps' => array(null, 1,2,3,4,5),
				'speed' => array(null, 0.25, 0.3, 0.5, 0.6, 1, 1.5, 2, 2.5, 5, 10, 20, 30, 60),
				'direction' => array('horizontal', 'vertical'),
				'random' => array('false', 'true'),
				'autoplay' => array('true', 'false'),
			);
		}

		protected function add_late_header()
		{
			$options = '';
			if(is_array($this->data_array['options'])){
				foreach($this->data_array['options'] as $option => $value){
					if($value !== 'true' && $value !== 'false'){ $value = '"'.$value.'"'; }
					$options .= " $option: $value,";
				}
			}
			$js = '
			window.addEvent("domready", function() { 
				var gallery'.$this->id.' = new slideGallery($$(".gallery'.$this->id.'"), { '.$options.' });
			}
			);';
			
			content::singleton()->add_custom_head('link', array('rel' => 'stylesheet', 'type' => 'text/css', 'href' => Settings::singleton()->get_setting('document_root').'/skin/gallery.css'));
			content::singleton()->add_custom_head('script', array('type' => 'text/javascript', 'src'=> Settings::singleton()->get_setting('site_folder_js_direct').'gallery/slide.js'));
			content::singleton()->add_custom_head('script', array('type' => 'text/javascript'), $js);
		}
		
		protected function get_data()
		{
			parent::get_data();
			if(isset($this->data['directory']) && strlen($this->data['directory']) > 0){
				$files = glob(Settings::Singleton()->get_setting('cms_folder_downloads').DS.'mediamanager'.DS.$this->data['directory'].DS.'*');
				foreach($files as $file){
					$this->data['files'] .= '<file>'.$this->data['directory'].DS.basename($file).'</file>';
				}
			}
		}
		
		protected function attach_extra_data()
		{
//			return array_merge(parent::attach_extra_data(), $this->get_files());
			return parent::attach_extra_data();
		}
	}
}