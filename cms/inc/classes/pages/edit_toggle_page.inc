<?php
class cmspage_edit_toggle_page extends cmspage
{
	public function initalise()
	{
		$this->value = CMS::Singleton()->path_vars;
		return true;
	}
	
	public function run()
	{
		if(Security::singleton()->can_approve($this->value[1]))
		{
			if($this->value[2] == 'visibility')
			{
				database::singleton()->sql_toggle_field_by_id($this->value[1], 'tbl_pages', 'visible');
			}

			if($this->value[2] == 'delete')
			{
				database::singleton()->sql_toggle_field_by_id($this->value[1], 'tbl_pages', 'deleted');
			}
			common::redirect($this->value[1]);
		}
		else
		{
			error::do403();
		}
	}
}
?>