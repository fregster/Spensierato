<?php
class cmspage_ajax extends cmspage
{
	public function run()
	{
		//Pass directly to the AJAX class for processing
		ajax::process($this->value);
	}
}
?>