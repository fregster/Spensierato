<?php
class cmspage_download extends cmspage
{
	public function run()
	{
		// Site downloads
		// Need to get document details
		// Check the related page details
		// call file transfer retrieve

		$this->security_element_view();
		FileTransfer::singleton()->get_download($this->value);
	}
}
?>