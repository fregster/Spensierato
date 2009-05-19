<?php
class cmspage_skin extends cmspage
{
	public function initalise()
	{
		$this->value = CMS::Singleton()->path_vars(1);
		return true;
	}
	
	public function run()
	{
//		if($this->value == 'transparent-gif.gif') { header("HTTP/1.0 204 No Content"); die(); } //Return a no content code for fast lightwight response, useful for sprites

		//Skin images NO security check
		/**
		 * Site images
		 * Need to get document details
		 * Check the related page details
		 * call file transfer retrieve
		 */

		$file_search = array(
		Template::Singleton()->path().'/'.$this->value,
		Template::Singleton()->path().'/graphics/'.$this->value,
		Template::Singleton()->path().'/pre-load-images/'.$this->value,
		Settings::Singleton()->get_setting('cms_root').'/skins/default/pre-load-images/'.$this->value,
		Settings::Singleton()->get_setting('cms_root').'/skins/default/pre-load-images/'.$this->value,
		Settings::Singleton()->get_setting('cms_root').'/skins/default/graphics/'.$this->value,
		);

		foreach($file_search as $file)
		{
			if(is_file($file))
			{
				FileTransfer::singleton()->get_file($file, true, false);
				exit;
			}
//			echo($file);
		}
		error::do404();
	}
}
?>