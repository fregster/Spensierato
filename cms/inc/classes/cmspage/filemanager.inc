<?php
ini_set('display_errors', 0);
error_reporting(E_ERROR);
require_once DIR_EXTERNAL.'filemanager'.DS.'FileManager.php';

class cmspage_filemanager extends cmspage
{
	private $filepath;
	public function initalise()
	{
		if(!User::singleton()->is_admin()){
			return false;
		}
		$this->filepath = Settings::Singleton()->get_setting('cms_folder_downloads').DS.'mediamanager'.DS;
		if(!file_exists($this->filepath) && is_writable(Settings::Singleton()->get_setting('cms_folder_downloads'))){
			mkdir ($this->filepath);
			chmod ($this->filepath, 0774);
		}
		return true;
	}
	
	public function run()
	{
		$browser = new FileManager(array(
			'directory' => $this->filepath,
			'assetBasePath' => Settings::Singleton()->get_setting('document_root').Settings::Singleton()->get_setting('site_folder_js_direct').'/fileman/',
			'thumbnailPath' => $this->filepath.DS.'Thumbnails',
		));
		
		$browser->fireEvent(!empty($_GET['event']) ? $_GET['event'] : null);
		exit;
	}
}