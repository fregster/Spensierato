<?php
class cmspage_help extends cmspage_html
{
	public function run()
	{
		$element = Element::type('elementHelp');
		$element->build('help');
		Render::Singleton()->render_html(Content::Singleton()->build());
	}
}
?>