<?php
class elementAccess extends elementDefault
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
			$html = 'Accessability <strong>information</strong>';
			$this->data = '<data>'. Common::html($html) .'</data>';
			$this->search = NULL;
	}

}
?>