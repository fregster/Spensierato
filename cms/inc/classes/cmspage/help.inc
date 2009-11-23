<?php
class cmspage_help extends cmspage_html
{
	public function run()
	{
		$element = Element::type('element_Help');
		$element->build('help');
		Render::Singleton()->render_html(Content::Singleton()->build());
	}
}