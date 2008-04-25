<?
class elementDownload extends elementDefault
{
	function __construct()
	{
		$this->allowed_tags(array('filename' => 'text', 'file' => 'file', 'filesize' => 'auto', 'mime' => 'auto', 'description' => 'html'));
		$this->type = __CLASS__;
		$this->name = 'File Download Element';
		$this->image = 'download.png';
		$this->edit_height = 600;
	}
}


?>