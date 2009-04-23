<?php
class cmspage_admin extends cmspage
{
	public function run()
	{
		//Admin page check admin security
		if(!Security::singleton()->is_admin())
		{
			Error::do403();
		}
		$this->get_admin($this->page_details['value']);
	}
}
?>