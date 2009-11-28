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
		
		$path = Template::Singleton()->inhearited_paths();
		$file_search = array();
		foreach($path as $p){
			$file_search[] = $p.'/'.$this->value;
			$file_search[] = $p.'/graphics/'.$this->value;
			$file_search[] = $p.'/pre-load-images/'.$this->value;
		}

		foreach($file_search as $file)
		{
			if(is_file($file))
			{
				FileTransfer::singleton()->get_file($file, true, false);
				exit;
			}
//			echo('<br/>'.$file);
		}
		error::do404();
	}
}