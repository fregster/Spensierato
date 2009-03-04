<?php
class cmspage_register extends cmspage_html
{
	public function initalise()
	{
		$this->value = CMS::Singleton()->path_vars(1);
		return parent::initalise();
	}
	
	public function run()
	{
		//If not loged in load box else redirect to $this->page_details['value']
		if(User::Singleton()->is_logged_in())
		{
			Common::redirect($this->page_details['value']);
		}
		else
		{
			if(!Settings::singleton()->get_setting('allow_registration'))
			{
				Error::do403();
			}

			if(get::variable('is_set','key'))
			{
				register::singleton()->verify_register();
				exit;
			}
			
			//Check cookies are enabled first
			Common::cookie_check();

			//Load CMS Registration box
			$element = Element::type('elementRegister');
			$element->build('Register');
			Render::Singleton()->render_html(Content::Singleton()->build());
		}
	}
}
?>