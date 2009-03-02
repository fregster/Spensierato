<?php
class cmspage_stylesheet extends cmspage
{
	public function run()
	{
		render::Singleton()->send_xslt();
	}
}
?>