<?php
class element_Payment extends element_Default
{
	public function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Payment Gateway';
		$this->title = 'Payment Gateway';
		$this->main = 1;
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