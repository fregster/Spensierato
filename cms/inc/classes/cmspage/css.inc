<?php
class cmspage_css extends cmspage
{
	public function initalise()
	{
		$this->value = CMS::Singleton()->path_vars(1);
		return true;
	}
	
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