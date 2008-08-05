<?php
if(Settings::Singleton()->get_setting('uploads_enabled'))
{
	element::register('File Download', 'elementDownload');
	class elementDownload extends elementDefault
	{
		function __construct()
		{
			parent::__construct();
			$this->type = __CLASS__;
			$this->allowed_tags(array('filename' => 'auto', 'file' => 'file', 'filesize' => 'auto', 'mime' => 'auto', 'description' => 'html', 'linktext' => 'auto'));
			$this->type = __CLASS__;
			$this->name = 'File Download';
			$this->image = 'disk';
			$this->image_edit = '';
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
				$data = Database::Singleton()->sql_select_first(array('*'), 'tbl_elements_data', array('id'), array($this->id), NULL, NULL, options::Singleton()->get_option('admin_mode'));
				$data = '<data>'.$data['data'].'</data>';
				
				$extra['filename'] = common::node_from_xml($data, '/data/filename');
				$extra['filesize'] = common::node_from_xml($data, '/data/filesize');
				$extra['mime'] = common::node_from_xml($data, '/data/mime');				
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