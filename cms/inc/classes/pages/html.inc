<?php
class cmspage_html extends cmspage
{
	public function initalise()
	{
		if(CMS::Singleton()->page_exists(CMS::Singleton()->path_vars(0)) == true)
		{
			$this->value = CMS::Singleton()->path_vars(0);
			return true;
		}
		return false;
	}
	
	public function run()
	{
		//Database file, call content singleton
		$this->security_page_view();
		
		Options::singleton()->admin_check();
		Render::Singleton()->render_html(Content::Singleton()->build(), true);
	}
}
?>