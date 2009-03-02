<?php
class cmspage_login extends cmspage
{
	public function run()
	{
		//CMS Login

		//Check cookies are enabled first
		Common::cookie_check();
		if($_SERVER['REQUEST_METHOD'] == 'GET'and !get::variable('is_set','username'))
		{
			//If not loged in load box else redirect to $this->page_details['value']
			if(User::Singleton()->is_logged_in())
			{
				Common::redirect($this->page_details['value']);
			}
			else
			{
				//Load CMS Login box
				$element = Element::type('elementLogin');
				$element->build('login');
				Render::Singleton()->render_html(Content::Singleton()->build());
			}
		}
		else
		{
			//Do login function
			User::Singleton()->login();
			Common::redirect($this->value);
		}
	}
}
?>