<?php
class cmspage_account extends cmspage_html
{
	public function initalise()
	{
		$this->value = CMS::Singleton()->path_vars(1);
		return parent::initalise();
	}
	
	public function run()
	{
		$element = Element::type('element_Account');
		$element->build('account');
		Render::Singleton()->render_html(Content::Singleton()->build());
	}
}