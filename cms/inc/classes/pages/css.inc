<?php
class cmspage_css extends cmspage
{
	public function run()
	{
		//CSS Files NO security check
		/**
		 * Calls the template class which compresses and sends the CSS file
		 */
		Render::Singleton()->send_css();
	}
}
?>