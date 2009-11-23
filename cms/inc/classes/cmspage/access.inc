<?php
class cmspage_access extends cmspage_html
{
	public function initalise()
	{
		$this->value = CMS::Singleton()->path_vars(1);
		return parent::initalise();
	}
	
	public function run()
	{
		$element = Element::type('element_Access');
		$element->build('access');
		Render::Singleton()->render_html(Content::Singleton()->build());
	}
}