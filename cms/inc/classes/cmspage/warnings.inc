<?php
class cmspage_warnings extends cmspage
{
	public function initalise()
	{
		$this->value = CMS::Singleton()->path_vars(1);
		return true;
	}
	
	public function run()
	{
		$element = Element::type('elementWarnings');
		$element->build('Warnings');
		Render::Singleton()->render_edit(Content::Singleton()->build());
	}
}
?>