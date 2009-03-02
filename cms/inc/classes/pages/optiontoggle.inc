<?php
class cmspage_optiontoggle extends cmspage
{	
	public function run()
	{
		// Toggle option value
		options::Singleton()->toggle_option($this->value);
		Common::bounce_to_referer();
	}
}
?>