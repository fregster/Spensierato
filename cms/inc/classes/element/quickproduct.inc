<?php
element::register('Quick Product', 'element_QuickProduct');
class element_QuickProduct extends element_Default
{
	public function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Quick Product';
		$this->allowed_tags(array('Descption' => 'text', 'Shipping' => 'value', 'Price' => 'value', 'Tax_Rate' => 'numeric'));
		if (options::singleton()->get_option('admin_mode')) {
			$this->admin_mode_data['image'] = 'book_white';
			$this->admin_mode_data['image_edit'] = 'book_edit';
		}
	}

}
?>