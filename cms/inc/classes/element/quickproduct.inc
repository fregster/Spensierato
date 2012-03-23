<?php
element::register('Quick Product', 'element_QuickProduct', false, 'basket');
class element_QuickProduct extends element_Default
{
	public function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Quick Product';
		$this->allowed_tags(array('Descption' => 'text', 'Shipping' => 'numeric', 'Price' => 'numeric', 'Tax_Rate' => 'numeric', 'file' => 'file', 'filename' => 'auto', 'filesize' => 'auto', 'mime' => 'auto', 'linktext' => 'auto' ));
		if (options::singleton()->get_option('admin_mode')) {
			$this->admin_mode_data['image'] = 'book_white';
			$this->admin_mode_data['image_edit'] = 'book_edit';
		}
	}

	public function cart_data()
	{
		$extra = array('title' => $this->title);
		return array_merge($this->data_array, $extra);
	}
	
	private function is_supported_image($mime)
	{
		if($mime == 'image/png'||'image/jpeg'||'image/gif')
		{
			return true;
		}
		return false;
	}
	
	protected function attach_extra_data()
		{
			$extra = array();
			if($_FILES['uploadedfile']['error'] == 0 && $this->is_supported_image($_FILES['uploadedfile']['type']))
			{
				$extra['filename'] = $_FILES['uploadedfile']['name'];
				$extra['filesize'] = $_FILES['uploadedfile']['size'];
				$extra['mime'] = $_FILES['uploadedfile']['type'];
			}
			else
			{
				$data = Database::Singleton()->sql_select_first(array('data'), 'tbl_elements_data', array('id'), array($this->id), NULL, NULL, options::Singleton()->get_option('admin_mode'));
				$data = json_decode($data['data'], TRUE);

				if(isset($data['filename'])){ //Check if there is some file information and merge as required
					$extra['filename'] = $data['filename'];
					$extra['filesize'] = $data['filesize'];
					$extra['mime'] = $data['mime'];
				}
			}
			return $extra;
		}

		protected function attach_upload()
		{
			if(parent::attach_upload())
			{
				return $this->save_preview();
			}
		}
	
		protected function save_preview()
		{
			if($_FILES['uploadedfile']['error'] == 0)
			{
				return image::generate_thumb($this->id.'-'.$this->admin_mode_data['revision'], 100, 100);
			}
			return true;
		}
}