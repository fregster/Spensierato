<?
class elementRegister extends elementDefault
{
	function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Registration Box Element';
		$this->image = 'login.png';
		$this->image_edit = '';
	}
	
	protected function get_data()
	{
		$template = 'templates/register.html';
		
		$this->data = '<data>'. Common::html($html) .'</data>';
		$this->search = NULL;
	}

}


?>