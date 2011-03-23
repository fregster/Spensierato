<?php
class cmspage_dashboard extends cmspage
{
	public function initalise()
	{
		return true;
	}
	
	public function run()
	{
		if(user::Singleton()->is_admin() && options::Singleton()->get_option('admin_mode'))
		{
			Render::Singleton()->render_dashboard(Content::Singleton()->build());
			return true;
		}
		error::do403();
	}
}