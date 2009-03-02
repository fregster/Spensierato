<?php
class cmspage_access extends cmspage
{
	public function run()
	{
		$element = Element::type('elementAccess');
		$element->build('access');
		Render::Singleton()->render_html(Content::Singleton()->build());
	}
}
?>