<?php
class cmspage_stream extends cmspage
{
	public function run()
	{
		$this->security_element_view();
		Stream::stream($this->value);
	}
}
?>