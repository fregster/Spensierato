<?
element::register('File Download', 'elementDownload');
class elementDownload extends elementDefault
{
	function __construct()
	{
		$this->allowed_tags(array('filename' => 'auto', 'file' => 'file', 'filesize' => 'auto', 'mime' => 'auto', 'description' => 'html'));
		$this->type = __CLASS__;
		$this->name = 'File Download';
		$this->image = 'download.png';
		$this->edit_height = 600;
	}


}
?>