<?php
if(Settings::Singleton()->get_setting('uploads_enabled'))
{
	element::register('File Download', 'element_Download');
	class element_Download extends element_Default
	{
		public function __construct()
		{
			parent::__construct();
			$this->type = __CLASS__;
			$this->allowed_tags(array('file' => 'file', 'filename' => 'auto', 'filesize' => 'auto', 'mime' => 'auto', 'description' => 'html', 'linktext' => 'auto'));
			$this->name = 'File Download';
			if (options::singleton()->get_option('admin_mode')) {
				$this->admin_mode_data['image'] = 'disk';
				$this->admin_mode_data['image_edit'] = '';
			}
		}

		protected function attach_extra_data()
		{
			$extra = array('linktext' => $_POST['title']);
			if($_FILES['uploadedfile']['error'] == 0)
			{
				$extra['filename'] = $_FILES['uploadedfile']['name'];
				$extra['filesize'] = $_FILES['uploadedfile']['size'];
				$extra['mime'] = $_FILES['uploadedfile']['type'];
			}
			else
			{
				$data = Database::Singleton()->sql_select_first(array('data'), 'tbl_elements_data', array('id'), array($this->id), NULL, NULL, options::Singleton()->get_option('admin_mode'));
				$data = json_decode($data['data'], TRUE);

				$extra['filename'] = $data['filename'];
				$extra['filesize'] = $data['filesize'];
				$extra['mime'] = $data['mime'];
			}
			return $extra;
		}

		protected function attach_upload()
		{
			return parent::attach_upload();
		}
	}
}
?>