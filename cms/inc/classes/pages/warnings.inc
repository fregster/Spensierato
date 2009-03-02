<?php
class cmspage_warnings extends cmspage
{
	public function run()
	{
		$element = Element::type('elementWarnings');
		$element->build('Warnings');
		Render::Singleton()->render_edit(Content::Singleton()->build());
	}
}
?>