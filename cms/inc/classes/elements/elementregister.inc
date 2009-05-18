<?php
class elementRegister extends elementDefault
{
	public function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Registration Box Element';
		$this->title = 'Register';
		if (options::singleton()->get_option('admin_mode')) {
			$this->admin_mode_data['image'] = 'login.png';
			$this->admin_mode_data['image_edit'] = '';
		}
	}

	protected function get_data()
	{
		$template = 'templates/register.html';
		$html = Template::Singleton()->get_file_content($template);
		$this->data = '<data>'. Common::html($html) .'</data>';
		$this->search = NULL;
	}

}


?>