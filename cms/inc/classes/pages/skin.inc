<?php
class cmspage_skin extends cmspage
{
	public function initalise()
	{
		$this->value = CMS::Singleton()->path_vars[1];
		return true;
	}
	
	public function run()
	{
		//Skin images NO security check
		/**
		 * Site images
		 * Need to get document details
		 * Check the related page details
		 * call file transfer retrieve
		 */

		$file_search = array(
		Settings::Singleton()->get_setting('cms_root').'/skins/'.Settings::Singleton()->get_setting('skin').'/'.$this->value,
		Settings::Singleton()->get_setting('cms_root').'/skins/'.Settings::Singleton()->get_setting('skin') .'/graphics/'.$this->value,
		Settings::Singleton()->get_setting('cms_root').'/skins/'.Settings::Singleton()->get_setting('skin') .'/pre-load-images/'.$this->path_vars[2],
		Settings::Singleton()->get_setting('cms_root').'/skins/default/pre-load-images/'.$this->path_vars[2],
		Settings::Singleton()->get_setting('cms_root').'/skins/default/pre-load-images/'.$this->path_vars[1],
		Settings::Singleton()->get_setting('cms_root').'/skins/default/graphics/'.$this->path_vars[1],
		);

		foreach($file_search as $file)
		{
			if(is_file($file))
			{
				FileTransfer::singleton()->get_file($file, true, false);
				exit;
			}
		}
		error::do404();
	}
}
?>