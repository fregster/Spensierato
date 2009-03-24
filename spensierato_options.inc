<?php
//Site Settings
//Settings::Singleton()->new_setting($name, $value, $protected = false, $hidden = false)
//Protected stops the value been overwriten in the future by DB settings
//Hidden stops the settings been viewed in the XML, useful for hiding directory structure

//Setting $GLOBALS['protect_db_settings']
//Alows overwriting of the db settings with settings from this file
//Must be GLOBAL as this var is called in the construct function
//true / false
Settings::Singleton()->new_setting('protect_db_settings', true, true, true);

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

//Setting meta_description
//
Settings::singleton()->new_setting('meta_description', 'A free and easy to use, open source content managment system');

//Setting meta_keywords
//String of Comma seperated values
Settings::Singleton()->new_setting('meta_keywords', 'cms, open, free, gpl, spensierato, content, managment, system');

//Setting meta_verify-v1
//Google verification string
Settings::Singleton()->new_setting('meta_verify-v1', NULL);

//Setting char_set
//Default encodeing set
Settings::Singleton()->new_setting('char_set', 'UTF-8');

//Setting homepage
//int default sql select lowest value from db set to reduce sql hits or force to defaut to another page
//Settings::Singleton()->new_setting('homepage', 1);

//Settings content_require_approval
//Allow contributors to make changes but they must wait for an editor to approve the content before it gets published
Settings::Singleton()->new_setting('content_require_approval', true, false, true);

//Settings content_auto_publish
//If you can aprove your own content on saving it will be automaticaly published
Settings::Singleton()->new_setting('content_auto_publish', true, false, true);

//Setting show_footer
//false / true
Settings::Singleton()->new_setting('show_footer', true);

//Setting menu_level
//int default 0
Settings::Singleton()->new_setting('menu_level', 0);

//Setting menu_max_levels
// int default 2
Settings::Singleton()->new_setting('menu_max_levels', 2);

//Setting show_copyright
//true / false
Settings::Singleton()->new_setting('show_copyright', true);

//Setting allow_registration
//true / false
Settings::Singleton()->new_setting('allow_registration', true);

//Setting show_login
//true / false
Settings::Singleton()->new_setting('show_login', true);

//Setting show_help
//true / false
Settings::Singleton()->new_setting('show_help', true);

//Setting show_access
//true / false
Settings::Singleton()->new_setting('show_access', true);

//Setting skin
//Set this to the path of the skin you wish to use
//Folder name from cms/skins/
Settings::Singleton()->new_setting('skin', 'default', false, true);

//Setting skin_alternative_css
//Set this to the css file if not css.css in the skins folder
//Folder name from cms/skins/
//Settings::Singleton()->new_setting('skin_alternative_css', 'alternative.css', false, true);

//Setting send_as_xml
//Set this to send the page as un-rendered XML (sends link to the stylesheet with XML)
//Defaults to false and uses PHP XSLT parser to generate the HTML
//false / true
Settings::Singleton()->new_setting('send_as_xml', false);


//Setting max_file_size
//Sets the maximum file upload size this might also need to be set in the php.ini
//25000000
Settings::Singleton()->new_setting('max_file_size', 25000000);

//Setting js_minify /* Currently breaks JS if not well formed */
//clean the javascript to reduce the file size defaults to internal (1) just removes comments
//0 no cleaning
//1 use internal cleaner coments only
//2 use internal cleaner comments and line breaks //This and higher require wellformed JS
//3 use JSMin
Settings::Singleton()->new_setting('js_minify', 0, false, true);


//Setting js_cache
//This setting allows the javascript once built to be cached on the file system
//The reduces the number of external scripts called, improves compression and reduces load on your server
//The folder with in the CMS 'writable' or '/tmp/cms' needs to be writeable by the web server
Settings::Singleton()->new_setting('js_cache', false, false, true);

Settings::Singleton()->new_setting('css_minify', true, false, true);

//Setting search
//Allow site searching
//true / false
Settings::Singleton()->new_setting('search', true);

//Setting related_pages
//Allow site related_pages searching
//true / false
Settings::Singleton()->new_setting('related_pages', true, false, true);

//Setting search_with_security
//Allow searching when a user is logged in to include pages with security
//Slower but gives more results
//true / false
Settings::Singleton()->new_setting('search_with_security', true, false, true);

//Setting cache_use_ETags
//Might only work on unix style file systems
Settings::Singleton()->new_setting('cache_use_ETags', true, false, true);

//Setting site_option_showpagetime
//true / false
Settings::Singleton()->new_setting('site_option_showpagetime', true);

//Setting tidy
//Askes the render method to call tidy on the html output, the default skin should not need this but its here anyway.
//true / false
Settings::Singleton()->new_setting('tidy', true, false, true);

//Setting phpids
//Use PHPIDS (Intrusion Detection System)
Settings::singleton()->new_setting('phpids', false, false, true);

//Site file options

//Static files hosts
//If not set then the main document_root var / ajax is used
//Ajax
//Settings::Singleton()->new_setting('static_host_ajax', ('ajax.'.$GLOBALS['document_root']));
//Skin and images
//Settings::Singleton()->new_setting('static_host_skin', ('skin.'.$GLOBALS['document_root']));
//JS host
//Settings::Singleton()->new_setting('static_host_js', ('js.'.$GLOBALS['document_root']));

//Paths
Settings::Singleton()->new_setting('site_option_css', ($GLOBALS['document_root'] . '/css'));
Settings::Singleton()->new_setting('site_folder_graphics', ($GLOBALS['document_root'] . '/graphics/'));

/* This has been changed from javascript to js-combine due to debians prototype package url rewriting */
Settings::Singleton()->new_setting('site_folder_js', ($GLOBALS['document_root'] . '/js-combine/'));
Settings::Singleton()->new_setting('site_folder_js_direct', ($GLOBALS['document_root'] . '/js/'));

//Uploads alowed, other options may disable this
//true / false
Settings::Singleton()->new_setting('uploads_enabled', true);

//Temporay uploads folder
Settings::Singleton()->new_setting('cms_folder_uploads', $GLOBALS['cms_folder_uploads'], false, true);

//Permanant uploads folder
Settings::Singleton()->new_setting('cms_folder_downloads', $GLOBALS['cms_folder_downloads'], false, true);

//Unscaled images
Settings::Singleton()->new_setting('cms_folder_orig_images', $GLOBALS['cms_folder_orig_images'], false, true);

//Log files
Settings::Singleton()->new_setting('cms_folder_logs', $GLOBALS['cms_folder_logs'], false, true);

//download_rate
//Maximum file download rate in kb/s
Settings::Singleton()->new_setting('download_rate', NULL, false, true);

//stream
//Set the path for the streaming server,
//use /download/ for simple downloads or '/video/' for php based streaming
Settings::Singleton()->new_setting('stream', '/video/');

/* Session security */
//Setting session_security_enhanced
//Requires that the headers for the user_agent is constant with when the session was created
//true / false
Settings::Singleton()->new_setting('session_security_enhanced', true, false, true);

//Setting session_security_ip
//Requires the origonating IP to be constant with when the session was created
//true / false
Settings::Singleton()->new_setting('session_security_ip', false, false, true);

//Setting login_capthca
//Requires the user to enter a security code when login in
//true / false
Settings::Singleton()->new_setting('login_capthca', true);

//Setting login_javascript_secure
//If the users webbrowser suports JS then the users login is encrptyed using the capthca as part of the key
//true / false
Settings::Singleton()->new_setting('login_secure', true);

//Setting login_capthca_length
//Sets the length of the image
//int max 8
Settings::Singleton()->new_setting('login_capthca_length', 5, false, true);

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
Settings::Singleton()->new_setting('debug', 0, false, true);

//Setting debug_log_folder
//string path to the debug log folder
//Default = '/var/log/cms'
Settings::Singleton()->new_setting('debug_log_folder', '/var/log/cms', false, true);

//Encryption hash type used for passwords standard is md5 or sha1 (min recomended) other types may require PECL hash installing
//Note changing this once users have entered there passwords will require reseting ALL there accounts.
Settings::Singleton()->new_setting('site_option_encryption', 'sha256', false, true);

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
Settings::Singleton()->new_setting('random_key', $GLOBALS['random_key'], true, true);

//Setting update_version_check
//To enable auto version checking set this value to the update server
//If the log dir is not writeable auto update checking will be disabled
Settings::Singleton()->new_setting('update_version_check', 'spensierato.fryer.org.uk', false, true);

//Setting update_version_check_schedule
//Set the timeperiod between version update checking in days
//If the log dir is not writeable auto update checking will be disabled
//default: 7
Settings::Singleton()->new_setting('update_version_check_schedule', 7, false, true);

//Setting user_incorect_logins
//To enable brute force login protection set the max number of failed logins per username
//On reaching the value a new password is generated
Settings::Singleton()->new_setting('user_incorect_logins', 10, false, true);

//Setting user_auto_email_password
//To enable the system to automatacily email new passwords if re-set by the system
//The email includes a note about account security
Settings::Singleton()->new_setting('user_auto_email_password', true, false, true);

Settings::singleton()->new_setting('memcached_sql', false, false, true);
Settings::singleton()->new_setting('memcached_session', false, false, true);

Settings::singleton()->new_setting('virus_scanner', 'clamav', false, true);
//Settings::singleton()->new_setting('virus_scanner_path', '/path/to/custom/scanner/install/', false, true);

//Setting mod_rewrite
//Use mod_rewrite to clean the URL's
Settings::singleton()->new_setting('mod_rewrite', $GLOBALS['allow_rewrite'], false, true);

//setting date_format
//this determines the date format for the website.
//use date()-compliant format directives
Settings::singleton()->new_setting('date_format', 'd/m/Y', false, true);

//setting server_os
//this setting contains the server's os
//used to toggle os specific finctionality (e.g. ffmpeg)
Settings::singleton()->new_setting('server_os', $GLOBALS['server_os'], false, true);

//output_compression_level
//Sets the GZip compression level set to 0 to disable
Settings::singleton()->new_setting('output_compression_level', 9, false, true);
?>