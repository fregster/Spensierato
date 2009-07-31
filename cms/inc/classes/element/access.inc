<?php
class element_Access extends element_Default
{
	public function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Accessability Element';
		if (options::singleton()->get_option('admin_mode')) {
			$this->admin_mode_data['image'] = 'access.png';
			$this->admin_mode_data['image_edit'] = '';
		}

	}

	protected function get_data()
	{
			$this->data = '<data>'. Common::html(file_get_contents(Settings::singleton()->get_setting('cms_root').'/langs/'.Settings::singleton()->get_setting('site_lang').'/access')) .'</data>';
			$this->search = NULL;
	}

}
?>