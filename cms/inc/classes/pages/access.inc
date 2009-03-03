<?php
class cmspage_access extends cmspage
{
	public function initalise()
	{
		$this->value = Page::Singleton()->load(CMS::Singleton()->path_vars[1]);
		return true;
	}
	
	public function run()
	{
		$element = Element::type('elementAccess');
		$element->build('access');
		Render::Singleton()->render_html(Content::Singleton()->build());
	}
}
?>