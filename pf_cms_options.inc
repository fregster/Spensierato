<?
//Site Settings

//Setting $GLOBALS['protect_db_settings']
//Alows overwriting of the db settings with settings from this file
//Must be GLOBAL as this var is called in the construct function
//true / false
Settings::Singleton()->new_setting('protect_db_settings', true);

//Setting Intro text
//String
Settings::Singleton()->new_setting('intoduction_text', 'Welcome To ');

//Setting site_name_long
//String
Settings::Singleton()->new_setting('site_name_long', 'Default CMS');

//Setting site_name_short
//Strring default = site_name_long
Settings::Singleton()->new_setting('site_name_short', 'CMS');

//Setting site_lang
//For language template (Not implemented)
Settings::Singleton()->new_setting('site_lang', 'en');

//Setting char_set
//Default encodeing set
Settings::Singleton()->new_setting('char_set', 'UTF-8');

//Setting homepage
//int default 0
Settings::Singleton()->new_setting('homepage', 1);

//Setting menu_level
//int default 0
Settings::Singleton()->new_setting('menu_level', 0);

//Setting menu_max_levels
// int default 2
Settings::Singleton()->new_setting('menu_max_levels', 2);

//Setting show_copyright
//true / false
Settings::Singleton()->new_setting('show_copyright', true);

//Setting skin
//Set this to the path of the skin you wish to use
//Folder name from cms/skins/
Settings::Singleton()->new_setting('skin', 'default');

//Setting search
//Allow site searching
//true / false
Settings::Singleton()->new_setting('search', true);

//Setting search_with_security
//Allow searching when a user is logged in to include pages with security
//Slower but gives more results
//true / false
Settings::Singleton()->new_setting('search_with_security', true);

//Setting cache_use_ETags
//Might only work on unix style file systems
Settings::Singleton()->new_setting('cache_use_ETags', true);

//Setting site_option_showpagetime
//true / false
Settings::Singleton()->new_setting('site_option_showpagetime', true);

//Setting tidy
//Askes the render method to call tidy on the html output, the default skin should not need this but its here anyway.
//true / false
Settings::Singleton()->new_setting('tidy', true);

//Site file options
Settings::Singleton()->new_setting('site_option_css', ($GLOBALS['site_root'] . '/css'));
Settings::Singleton()->new_setting('site_folder_graphics', ($GLOBALS['site_root'] . '/graphics/'));
Settings::Singleton()->new_setting('site_folder_uploads', ($GLOBALS['site_root'] . '/documents/uploads/tmp/'));
Settings::Singleton()->new_setting('site_folder_downloads', ($GLOBALS['site_root'] . '/documents/downloads/')); 
Settings::Singleton()->new_setting('site_folder_js', ($GLOBALS['site_root'] . '/js/'));

/* Session security */
//Setting session_security_enhanced
//Requires that the headers for the user_agent is constant with when the session was created
//true / false
Settings::Singleton()->new_setting('session_security_enhanced', true);

//Setting session_security_ip
//Requires the origonating IP to be constant with when the session was created
//true / false
Settings::Singleton()->new_setting('session_security_ip', false);

//Setting login_capthca
//Requires the user to enter a security code when login in
//true / false
Settings::Singleton()->new_setting('login_capthca', true);

//Setting login_capthca_length
//Sets the length of the image
//int max 8
Settings::Singleton()->new_setting('login_capthca_length', 5);

//Setting login_capthca_remember
//If the user logs in with remember me do not use capthca
//Defaults to true so capthca is not used if the cookie is authenticated
//true / false
Settings::Singleton()->new_setting('login_capthca_remember', true);

//Site options
//Setting debug
//int for debug level
//Default 	0 Off
//			1 Log SQL (Non cached only)
//			2 Log SQL (All)
//			5 Force Displaying SQL
//			10 Save debug to file
Settings::Singleton()->new_setting('debug', 5);

//Seyying debug_log_folder
//string path to the debug log folder
//Default = '/var/log/cms'
Settings::Singleton()->new_setting('debug_log_folder', '/var/log/cms');

//Encryption hash type used for passwords standard is md5 or sha1 (min recomended) other types may require PECL hash installing
//Note changing this once users have entered there passwords will require reseting ALL there accounts.
Settings::Singleton()->new_setting('site_option_encryption', 'sha256');

?>