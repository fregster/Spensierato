<?php
class cmspage_logout extends cmspage
{
	public function run()
	{
		//CMS Logout
		session::Singleton()->destroy_session($this->value);
	}
}