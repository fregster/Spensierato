<?php
class cmspage_stream extends cmspage
{
	public function initalise()
	{
		$this->value = CMS::Singleton()->path_vars(1);
		return true;
	}
	
	public function run()
	{
		$this->security_element_view();
		Stream::stream($this->value);
	}
}