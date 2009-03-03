<?php
class cmspage_warnings extends cmspage
{
	public function initalise()
	{
		$this->value = Page::Singleton()->load(CMS::Singleton()->path_vars[1]);
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