<?php
class cmspage_preview extends cmspage
{
	public function run()
	{
		/**
		 * Site images
		 * Need to get document details
		 * Check the related page details
		 * call file transfer retrieve
		 */

		$this->security_element_view();
		FileTransfer::singleton()->get_preview($this->value);
	}
}
?>