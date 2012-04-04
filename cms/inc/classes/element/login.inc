<?php
class element_Login extends element_Default
{
	public function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Login Box Element';
		$this->title = 'Login';
		$this->main = 1;
		if (options::singleton()->get_option('admin_mode')) {
			$this->admin_mode_data['image'] = 'login.png';
			$this->admin_mode_data['image_edit'] = '';
		}
	}

	protected function get_data()
	{
		if(Settings::Singleton()->get_setting('login_capthca'))
		{
			if((Settings::Singleton()->get_setting('login_capthca_remember')) && (cookie::Singleton()->get_cookie_value('username') != NULL)  || !Capthca::enabled())
			{
				$template = 'templates/login.html';
			}
			else
			{
				if(enhancedLogin::singleton()->suported())
				{
					$template = 'templates/login_enhanced.html';
				}
				else
				{
					$template = 'templates/login_with_capthca.html';
				}
			}
		}
		else
		{
			$template = 'templates/login.html';
		}

		if(cookie::Singleton()->get_cookie_value('username') != NULL)
		{
			$checked = "checked=\"checked\"";
		}
		else
		{
			$checked = NULL;
		}
		
		

		$html = '<div id="login_background">'.str_replace('**CMS**username', cookie::Singleton()->get_cookie_value('username'), Template::Singleton()->get_file_content($template)).'</div>';

		$html = str_replace(array('**CMS**remembered', '**CMS**error'), array($checked, User::Singleton()->retrieve('login_error')), $html).'<div><a href="'.Settings::Singleton()->get_Setting('document_root').'/forgottenpassword">Forgotten your password?</a></div>';

		$this->data = '<data>'. Common::html($html) .'</data>';
		$this->search = NULL;
//		var_dump($this); die();
	}
	
	protected function build_data_xml(){
		
	}

	protected function add_header()
		{
			$js = Settings::singleton()->get_setting('static_host_js').Settings::singleton()->get_setting('site_folder_js_direct').'login_packed.js';
			content::singleton()->add_custom_head('script', array('type' => 'text/javascript', 'src'=> $js));
		}

}