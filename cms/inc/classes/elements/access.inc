<?
class elementAccess extends elementDefault
{
	function construct()
	{
		$this->type = __CLASS__;
		$this->name = 'Accessability Element';
		$this->image = 'access.png';
		$this->edit_height = 600;
	}
	
	protected function get_data()
	{
			$html = 'Accessability <strong>information</strong>';
			$this->data = '<data>'. htmlentities($html, ENT_QUOTES, Settings::Singleton()->get_setting('char_set')) .'</data>';
			$this->search = NULL;
	}
}


?>