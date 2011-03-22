<?php
//Setting facebook_api_key
//Required for the use of facebook OAuth
Settings::Singleton()->new_setting('facebook_api_key', null, false, false, true, SETTING_EXTERNAL_API_KEYS);

class authentication_facebook extends authentication_default
{
	public function auth()
	{
		
	}
	
	public function login_html()
	{
		return '<script src="http://connect.facebook.net/en_US/all.js"></script>
      <script>
         FB.init({ 
            appId:\''.settings::singleton()->get_setting('facebook_api_key').'\', cookie:true, 
            status:true, xfbml:true 
         });
      </script>
      <fb:login-button>Login with Facebook</fb:login-button>
		';
	}
}