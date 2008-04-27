<?
class elementWarnings extends elementDefault
{
	function __construct()
	{
		$this->type = __CLASS__;
		$this->name = 'CMS Warnings Element';
		$this->image = '';
		$this->edit_height = 450;
	}
	
	protected function get_data()
	{
			$html = 'Warnings';
			$this->data = '<data>'. warnings::Singleton()->get_as_xml() .'</data>';
			$this->search = NULL;
	}
}


?>