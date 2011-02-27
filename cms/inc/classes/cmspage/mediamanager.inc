<?php
class cmspage_mediamanager extends cmspage
{
	public function initalise()
	{
		$this->value = Settings::Singleton()->get_setting('cms_folder_downloads').DS.'mediamanager'.DS.Common::clean_and_combine_url(CMS::Singleton()->path_vars, 1);
		return true;
	}
	
	public function run()
	{
		FileTransfer::singleton()->get_file($this->value);
	}
}