<?php
class cmspage_edit_update_page extends cmspage
{
	public function run()
	{
		if(Security::singleton()->can_approve($this->value[1]))
		{
			if(post::variable('is_set','title'))
			{
				database::singleton()->sql_update_by_id($this->value[1], 'tbl_pages', array('title'), array(post::variable('title', 'sql')));
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