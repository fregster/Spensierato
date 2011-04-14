<?php
class element_Help extends element_Default
{
	public function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Help Information Element';
		$this->main = 1;
		if (options::singleton()->get_option('admin_mode')) {
			$this->admin_mode_data['image'] = 'help.png';
			$this->admin_mode_data['image_edit'] = '';
		}
	}

	protected function get_data()
	{
			$html = 'Help information';
			$this->data = '<data>'. Common::html($html) .'</data>';
			$this->search = NULL;
	}

}