<?php
class element_Warnings extends element_Default
{
	public function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'CMS Warnings Element';
		if (options::singleton()->get_option('admin_mode')) {
			$this->admin_mode_data['image'] = '';
			$this->admin_mode_data['image_edit'] = '';
			$this->admin_mode_data['edit_height'] = 450;
		}
	}

	protected function get_data()
	{
			$html = 'Warnings';
			$this->data = '<data>'. warnings::Singleton()->get_as_xml() .'</data>';
			$this->search = NULL;
	}

}
?>