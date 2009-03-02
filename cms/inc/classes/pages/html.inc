<?php
class cmspage_html extends cmspage
{
	public function run()
	{
		//Database file, call content singleton
		$this->security_page_view();
		
		Options::singleton()->admin_check();
		Render::Singleton()->render_html(Content::Singleton()->build(), true);
	}
}
?>