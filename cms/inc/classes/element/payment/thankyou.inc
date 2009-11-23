<?php
class element_Payment_thankyou extends element_Default
{
	public function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Payment Thank You';
		$this->title = 'Payment Thank You';
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