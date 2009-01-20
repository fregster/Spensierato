<?php
class elementAccess extends elementDefault
{
	function construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Accessability Element';
		$this->image = 'access.png';
		$this->image_edit = '';
	}
	
	protected function get_data()
	{
			$html = 'Accessability <strong>information</strong>';
			$this->data = '<data>'. Common::html($html) .'</data>';
			$this->search = NULL;
	}

}
?>