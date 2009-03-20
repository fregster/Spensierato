<?php
class elementRegister extends elementDefault
{
	function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Registration Box Element';
		$this->image = 'login.png';
		$this->image_edit = '';
		$this->title = 'Register';
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