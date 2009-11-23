<?php
class cmspage_register_details extends cmspage_html
{
	public function run()
	{
		if(User::Singleton()->is_logged_in())
		{
			Common::redirect($this->page_details['value']);
		}
		else
		{
			if(Settings::singleton()->get_setting('allow_registration'))
			{
				//Do registration function
				register::singleton()->new_registration();
				$element = Element::type('elementRegister');
				$element->build('Register');
				$this->get_html($this->value);
			}
			else
			{
				Error::do403();
			}
		}
	}
}