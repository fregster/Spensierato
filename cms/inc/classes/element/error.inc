<?php
class element_Error extends element_Default
{
	public function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Error Element';
		$this->main = 1;
		if (options::singleton()->get_option('admin_mode')) {
			$this->admin_mode_data['image'] = 'access.png';
			$this->admin_mode_data['image_edit'] = '';
		}
	}

	protected function get_data()
	{
		$this->data = '<data>'. Common::html(file_get_contents(Settings::singleton()->get_setting('cms_root').DS.'langs'.DS.Settings::singleton()->get_setting('site_lang').DS.'error')) .'</data>';
		$this->search = NULL;
	}

}