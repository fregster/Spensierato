<?php
class cmspage_toggle extends cmspage
{	
	public $bounce;
	
	public function initalise()
	{
		$this->value = CMS::Singleton()->path_vars[1];
		$this->bounce = CMS::Singleton()->path_vars[2];
		return true;
	}
	
	public function run()
	{
		// Toggle option value
		options::Singleton()->toggle_option($this->value);
		Common::bounce_to_referer();
	}
}
?>