<?php
class elementLogin extends elementDefault
{
	function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Login Box Element';
		$this->image = 'login.png';
		$this->image_edit = '';
	}
	
	protected function get_data()
	{
		if(Settings::Singleton()->get_setting('login_capthca'))
		{
			if((Settings::Singleton()->get_setting('login_capthca_remember')) and (cookie::Singleton()->get_cookie_value('username') != NULL) )
			{
				$template = 'templates/login.html';
			}
			else
			{
				if(enhanced_login::singleton()->suported())
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

		$html = str_replace('**CMS**remembered', $checked, $html);
		
		$this->data = '<data>'. Common::html($html) .'</data>';
		$this->search = NULL;
	}

	protected function add_header()
		{
			$js = Settings::singleton()->get_setting('site_folder_js_direct').'login_packed.js';
			content::singleton()->add_custom_head('script', array('type' => 'text/javascript', 'src'=> $js));
		}

}


?>