<?php
class elementHelp extends elementDefault
{
	public function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Help Information Element';
		$this->admin_mode_data['image'] = 'help.png';
		$this->admin_mode_data['image_edit'] = '';
	}

	protected function get_data()
	{
			$html = 'Help information';
			$this->data = '<data>'. Common::html($html) .'</data>';
			$this->search = NULL;
	}

}
?>