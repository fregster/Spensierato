<?php
class cmspage_error extends cmspage_html
{
	public function initalise()
	{
		$this->value = CMS::Singleton()->path_vars(1);
		return parent::initalise();
	}
	
	public function run()
	{
		$element = Element::type('element_Error');
		$element->build('error');
		Render::Singleton()->render_html(Content::Singleton()->build());
	}
}