<?php
class cmspage_loading extends cmspage
{
	public function run()
	{
		$this->send_loading($this->value);
	}
}
?>