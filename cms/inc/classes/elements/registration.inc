<?
class elementRegister extends elementDefault
{
	function __construct()
	{
		$this->type = __CLASS__;
		$this->name = 'Registration Box Element';
		$this->image = 'login.png';
		$this->edit_height = 600;
	}
	
	protected function get_data()
	{
		$template = 'templates/register.html';
		
		$this->data = '<data>'. htmlentities(Template::Singleton()->get_file_content($template), ENT_QUOTES, Settings::Singleton()->get_setting('char_set')) .'</data>';
		$this->search = NULL;
	}

}


?>