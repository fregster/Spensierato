<?php
class cmspage_file extends cmspage
{
	public function run()
	{
		FileTransfer::Singleton()->get_public_file($this->value);
	}
}
?>