<?php
class cmspage_tool extends cmspage
{
	public function run()
	{
		//CMS Included Functions no security checks
		$file = $this->cms_root . '/tools/' . Common::clean_path($this->value) .'.inc'; //FIXME path needs made public, a method writing or ref the GLOBAL
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