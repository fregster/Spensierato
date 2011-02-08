<?php
class cmspage_cookiecheckfailed extends cmspage_html
{
	public function initalise()
	{
		session::Singleton();
		if(User::Singleton()->is_logged_in()){
			Common::redirect('home');
		}
		
		$this->value = CMS::Singleton()->path_vars(1);
		return parent::initalise();
	}

	public function run()
	{
		//Load CMS Login box
		$element = Element::type('elementCookieCheckFailed');
		$element->build('login');
		Render::Singleton()->render_html(Content::Singleton()->build());
	}
}