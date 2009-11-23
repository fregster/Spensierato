<?php
class element_Payment_PayPal extends element_Default
{
	public function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'PayPal Payment Gateway';
		$this->title = 'PayPal Payment Gateway';
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