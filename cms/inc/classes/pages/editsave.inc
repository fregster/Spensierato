<?php
class cmspage_editsave extends cmspage
{
	public function run()
	{
		//Is this an update or createing a new element
		if($this->value == 'account')
		{
			User::Singleton()->update_from_post();
			Common::redirect('account');
		}

		if(post::variable('new_type', 'is_set'))
		{
			$this->security_page_edit();

			if(post::variable('new_type') === 'newPage')
			{
				$this->security_page_approve();
				$this->create_new_page($this->page_details['value']);
			}
			else
			{
				$element = Element::type(post::variable('new_type', 'string'));
				$element->create($this->value);
			}
		}
		else
		{
			$this->security_element_edit();
			
			$element = Element::type($this->page_details['value']);
			$element->build($this->page_details['value']);
			if($element->save(false))
			{
				//Save worked bounce to main page for element
				$this->saved($element->main());
			}
			else
			{
				//Save failed show error page
			}
		}
		common::redirect($this->page_details['value']);
	}
}
?>