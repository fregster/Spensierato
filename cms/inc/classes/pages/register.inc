<?php
class cmspage_register extends cmspage
{
	public function run()
	{
		//If not loged in load box else redirect to $this->page_details['value']
		if(User::Singleton()->is_logged_in())
		{
			Common::redirect($this->page_details['value']);
		}
		else
		{
			if(Settings::singleton()->get_setting('allow_registration'))
			{
				if(get::variable('is_set','key'))
				{
					register::singleton()->verify_register();
					exit;
				}
				else
				{
					//Check cookies are enabled first
					Common::cookie_check();

					//Load CMS Registration box
					$element = Element::type('elementRegister');
					$element->build('Register');
					$this->get_html($this->value);
				}
			}
			else
			{
				Error::do403();
			}
		}
	}
}
?>