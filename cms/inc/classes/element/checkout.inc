<?php
class element_Checkout extends element_Default
{
	public function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Shopping Cart Checkout';
		$this->title = 'Checkout';
		if (options::singleton()->get_option('admin_mode')) {
			$this->admin_mode_data['image'] = 'login.png';
			$this->admin_mode_data['image_edit'] = '';
		}
	}

	protected function get_data()
	{
		$this->data = '<data/>';
		$this->search = NULL;
	}
}


?>