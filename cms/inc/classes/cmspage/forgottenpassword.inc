<?php
class cmspage_forgottenpassword extends cmspage_html
{
	public function initalise()
	{
		session::Singleton();
		
		$this->value = CMS::Singleton()->path_vars(1);
		return parent::initalise();
	}

	public function run()
	{
		if($_SERVER['REQUEST_METHOD'] == 'POST' && csrf::check_token()){
			$id = database::singleton()->sql_select_first('id', 'tbl_users', array('username'), array(post::variable('username', 'string')));
			
			if(is_numeric($id['id']))
			{
				$user = new user;
				$user->force_id($id['id']);
				$user->forgotten_password();
			}
		}
		
		if(get::variable('fpuuid', 'isset') && get::variable('id', 'isset')){
			if(get::variable('id', 'is_numeric'))
			{
				$user = new user;
				$user->force_id(get::variable('id'));
				$user->remote_reset_password();
			}
		}
		
		$element = Element::type('elementforgottenpassword');
		$element->build('FPassword');
		//Add a page level form
		Content::Singleton()->build_form('Forgotten Passsword', array('username' => 'text'), null, 'Forgotten Password Form');
		Render::Singleton()->render_html(Content::Singleton()->build());
	}
}