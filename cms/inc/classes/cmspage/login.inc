<?php
class cmspage_login extends cmspage_html
{
	public function initalise()
	{
		session::Singleton();
		
		$this->value = CMS::Singleton()->path_vars(1);
		return parent::initalise();
	}

	public function run()
	{
//		print_r($this); die();
		//CMS Login

		//Check cookies are enabled first
		Common::cookie_check();
		if($_SERVER['REQUEST_METHOD'] == 'GET' and !get::variable('username', 'is_set'))
		{
			//If not loged in load box else redirect to $this->page_details['value']
			if(User::Singleton()->is_logged_in())
			{
				if(isset($this->page_details['value'])){
					Common::redirect($this->page_details['value']);
				}
				Common::redirect('/');
			}
			else
			{
				$this->load_login_box();
			}
		}
		else
		{
			if(post::variable('email', 'is_set')){
				User::Singleton()->forgotten_password();
				$this->load_login_box();
			}
			//Do login function
			User::Singleton()->login();
			Common::redirect($this->value);
		}
	}
	
	private function load_login_box(){
		//Load CMS Login box
		$element = Element::type('elementLogin');
		$element->build('login');
		Render::Singleton()->render_html(Content::Singleton()->build());
	}
}