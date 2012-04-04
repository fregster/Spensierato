<?php
class cmspage_logout extends cmspage
{
	public function run()
	{
		//CMS Logout
		log::record('logout', 'requested');
		session::Singleton()->destroy_session($this->value);
	}
}