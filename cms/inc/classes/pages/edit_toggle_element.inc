<?php
class cmspage_edit_toggle_element extends cmspage
{
	public function initalise()
	{
		$this->value = CMS::Singleton()->path_vars;
		return true;
	}
	
	public function run()
	{
		if(Security::singleton()->can_approve_element($this->value[1]))
			{
				$element = Element::type($this->value[1]);
				$element->build($this->value[1]);

				$element->toggle($this->value[2]);
				common::redirect($element->main());
			}
			else
			{
				error::do403();
			}
	}
}
?>