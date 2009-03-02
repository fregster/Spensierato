<?php
class cmspage_account extends cmspage
{
	public function run()
	{
		$element = Element::type('elementAccount');
		$element->build('account');
		Render::Singleton()->render_html(Content::Singleton()->build());
	}
}
?>