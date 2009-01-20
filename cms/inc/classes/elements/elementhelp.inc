<?php
class elementHelp extends elementDefault
{
	function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Help Information Element';
		$this->image = 'help.png';
		$this->image_edit = '';
	}
	
	protected function get_data()
	{
			$html = 'Help information';
			$this->data = '<data>'. Common::html($html) .'</data>';
			$this->search = NULL;
	}

}
?>