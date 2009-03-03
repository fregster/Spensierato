<?php
class cmspage_account extends cmspage
{
	public function initalise()
	{
		$this->value = Page::Singleton()->load(CMS::Singleton()->path_vars[1]);
		return true;
	}
	
	public function run()
	{
		$element = Element::type('elementAccount');
		$element->build('account');
		Render::Singleton()->render_html(Content::Singleton()->build());
	}
}
?>