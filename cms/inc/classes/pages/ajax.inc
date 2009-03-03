<?php
class cmspage_ajax extends cmspage
{
	public function initalise()
	{
		$this->value = CMS::Singleton()->path_vars;
		return true;
	}
	
	public function run()
	{
		//Pass directly to the AJAX class for processing
		ajax::process($this->value);
	}
}
?>