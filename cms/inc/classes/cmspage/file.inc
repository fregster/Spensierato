<?php
class cmspage_file extends cmspage
{
	public function initalise()
	{
		$this->value = Common::clean_and_combine_url(CMS::Singleton()->path_vars, 1);
		return true;
	}
	
	public function run()
	{
		FileTransfer::Singleton()->get_public_file($this->value);
	}
}
?>