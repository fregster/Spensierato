<?
class elementDownload extends elementDefault
{
	function __construct()
	{
		$this->allowed_tags(array('filename','data', 'size', 'mime', 'description'));
		$this->type = __CLASS__;
		$this->name = 'File Download Element';
		$this->image = 'download.png';
	}
}


?>