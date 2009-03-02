<?php
class cmspage_javascript extends cmspage
{
	public function run()
	{
		javascript::run($this->value);
		die();
	}
}
?>