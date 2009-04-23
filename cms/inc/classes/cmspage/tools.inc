<?php
class cmspage_tools extends cmspage
{
	public function initalise()
	{
		$this->value = CMS::Singleton()->path_vars(1);
		return true;
	}
	
	public function run()
	{
		//CMS Included Functions no security checks
		$file = CMS::Singleton()->cms_root() . '/tools/' . Common::clean_path($this->value) .'.inc'; //FIXME path needs made public, a method writing or ref the GLOBAL
		if(file_exists($file))
		{
			include($file);
			exit;
		}
		else
		{
			error::do404();
		}
	}
}
?>