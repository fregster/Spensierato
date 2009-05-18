<?php
class cmspage_loading extends cmspage
{
	public function initalise()
	{
		$this->value = Common::clean_and_combine_url(CMS::Singleton()->path_vars, 1);
		return true;
	}
	
	public function run()
	{
		include(Settings::Singleton()->get_setting('cms_root').'/js/submodal/loading.inc');
		exit;
	}
}
?>