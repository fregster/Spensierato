<?php
class element_Account extends element_Default
{
	public function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Account Information Element';
		if (options::singleton()->get_option('admin_mode')) {
			$this->admin_mode_data['image'] = 'account.png';
			$this->admin_mode_data['image_edit'] = '';
			$this->admin_mode_data['edit_height'] = 600;
		}
	}

	protected function get_data()
	{
		/*
		$html = 'Account information';
		$this->data = '<data>'. htmlentities($html, ENT_QUOTES, Settings::Singleton()->get_setting('char_set')) .'</data>';
		$this->search = NULL;
		*/

		$this->data = '
		<data>
			<lists>
				<list>1</list>
			</lists>
		</data>';
	}
}