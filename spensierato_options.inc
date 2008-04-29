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
Settings::Singleton()->new_setting('site_name_long', 'Another Spensierato Site');

//Setting site_name_short
//Strring default = site_name_long
Settings::Singleton()->new_setting('site_name_short', 'Spensierato');

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

//Setting send_as_xml
//Set this to send the page as un-rendered XML (sends link to the stylesheet with XML)
//Defaults to false and uses PHP XSLT parser to generate the HTML
//false / true
Settings::Singleton()->new_setting('send_as_xml', false);


//Setting max_file_size
//Sets the maximum file upload size
//100000
Settings::Singleton()->new_setting('max_file_size', 100000);

//Setting js_minify /* Currently breaks JS */
//clean the javascript to reduce the file size defaults to internal (1) just removes comments and break lines
//0 no cleaning
//1 use internal cleaner
//2 use JSMin
Settings::Singleton()->new_setting('js_minify', 1);

//Setting search
//Allow site searching
//true / false
Settings::Singleton()->new_setting('search', true);

//Setting related_pages
//Allow site related_pages searching
//true / false
Settings::Singleton()->new_setting('related_pages', true);

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
Settings::Singleton()->new_setting('site_folder_js', ($GLOBALS['site_root'] . '/javascript/'));
Settings::Singleton()->new_setting('site_folder_js_direct', ($GLOBALS['site_root'] . '/js/'));

//Temporay uploads folder
Settings::Singleton()->new_setting('cms_folder_uploads', 'documents/uploads/tmp/');

//Permanant uploads folder
Settings::Singleton()->new_setting('cms_folder_downloads', 'documents/uploads/');

//Unscaled images
Settings::Singleton()->new_setting('cms_folder_orig_images', 'documents/uploads/original_images/');

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
Settings::Singleton()->new_setting('debug', 1);

//Setting debug_log_folder
//string path to the debug log folder
//Default = '/var/log/cms'
Settings::Singleton()->new_setting('debug_log_folder', '/var/log/cms');

//Encryption hash type used for passwords standard is md5 or sha1 (min recomended) other types may require PECL hash installing
//Note changing this once users have entered there passwords will require reseting ALL there accounts.
Settings::Singleton()->new_setting('site_option_encryption', 'sha256');

//Setting google_ads
//This requires google_ad_slot and google_ad_client to also be set
//Use mine if you want to help sponsor me!
Settings::Singleton()->new_setting('google_ads', false);
Settings::Singleton()->new_setting('google_ad_slot', '6057605872');
Settings::Singleton()->new_setting('google_ad_client', 'pub-6675893347627358');
Settings::Singleton()->new_setting('google_ad_width', 180);
Settings::Singleton()->new_setting('google_ad_height', 150);

//Setting css_mobile
//Enable the mobile CSS, this overwrites css styles with non positional css
Settings::Singleton()->new_setting('css_mobile', true);

//Load the random key into the settings singleton
Settings::Singleton()->new_setting('random_key', $GLOBALS['random_key']);
?>