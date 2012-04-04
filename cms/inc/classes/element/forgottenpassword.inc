<?php
class element_forgottenpassword extends element_Default
{
	public function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Forgotten Password Element';
		$this->title = 'FPassword';
		$this->main = 1;
		if (options::singleton()->get_option('admin_mode')) {
			$this->admin_mode_data['image'] = 'login.png';
			$this->admin_mode_data['image_edit'] = '';
		}
	}

	protected function get_data()
	{
		$html = null;
		if(post::variable('username', 'isset')){
			$html = 'sent';
		}
		
		$this->data = '<data>'. Common::html($html) .'</data>';
		$this->search = NULL;
	}
	
}