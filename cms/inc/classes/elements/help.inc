<?
class elementHelp extends elementDefault
{
	function __construct()
	{
		$this->type = __CLASS__;
		$this->name = 'Help Information Element';
		$this->image = 'help.png';
		$this->edit_height = 600;
	}
	
	protected function get_data()
	{
			$html = 'Help information';
			$this->data = '<data>'. htmlentities($html, ENT_QUOTES, Settings::Singleton()->get_setting('char_set')) .'</data>';
			$this->search = NULL;
	}

}
?>