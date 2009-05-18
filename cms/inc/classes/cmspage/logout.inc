<?php
class cmspage_logout extends cmspage
{
	public function run()
	{
		//CMS Logout
		CMS::Singleton()->destroy_session($this->value);
	}
}
?>