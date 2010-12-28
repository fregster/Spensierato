<?php
//Site Settings
//Settings::Singleton()->new_setting($name, $value, $protected = false, $hidden = false, $editible = null || array || false, $edit_group = null)
//Editable should be an array containing the following
//array('desc' => 'Setting Description', 'cat' => 'Catagory Name', 'sec' => 'Section Name'); setting to false will disable any previous settings
//Protected stops the value been overwriten in the future by DB settings
//Hidden stops the settings been viewed in the XML, useful for hiding directory structure

//Setting $GLOBALS['protect_db_settings']
//Alows overwriting of the db settings with settings from this file
//Must be GLOBAL as this var is called in the construct function
//true / false
Settings::Singleton()->new_setting('protect_db_settings', true, true, true);

//Setting Intro text
//String
Settings::Singleton()->new_setting('intoduction_text', 'Welcome To ', false, false, true, 1);

//Setting site_name_long
//String
Settings::Singleton()->new_setting('site_name_long', 'Another Spensierato Site', false, false, true, 1);

//Setting site_name_short
//Strring default = site_name_long
Settings::Singleton()->new_setting('site_name_short', 'Spensierato', false, false, true, 1);

//Setting site_lang
//For language template (Not implemented)
Settings::Singleton()->new_setting('site_lang', 'en_gb', false, false, array('en_gb'), 10);

//Setting meta_description
//
Settings::singleton()->new_setting('meta_description', 'A free and easy to use, open source content managment system', false, false, true, 2);

//Setting meta_keywords
//String of Comma seperated values
Settings::Singleton()->new_setting('meta_keywords', 'cms, open, free, gpl, spensierato, content, managment, system', false, false, true, 2);

//Setting meta_verify-v1
//Google verification string
Settings::Singleton()->new_setting('meta_verify-v1', NULL, false, false, true, 3);

//Setting char_set
//Default encodeing set
Settings::Singleton()->new_setting('char_set', 'UTF-8');

//Setting homepage
//int default sql select lowest value from db set to reduce sql hits or force to defaut to another page
//Settings::Singleton()->new_setting('homepage', 1);

//Settings content_require_approval
//Allow contributors to make changes but they must wait for an editor to approve the content before it gets published
Settings::Singleton()->new_setting('content_require_approval', true, false, true, true);

//Settings content_auto_publish
//If you can aprove your own content on saving it will be automaticaly published
Settings::Singleton()->new_setting('content_auto_publish', true, false, true, true);

//Setting show_footer
//false / true
Settings::Singleton()->new_setting('show_footer', true, false, false, true, 5);

//Setting menu_level
//int default 0
Settings::Singleton()->new_setting('menu_level', 0, false, false, true, 4);

//Setting menu_max_levels
// int default 2
Settings::Singleton()->new_setting('menu_max_levels', 2, false, false, true, 4);

//Setting show_copyright
//true / false
Settings::Singleton()->new_setting('show_copyright', true, false, false, true, 5);

//Setting allow_registration
//true / false
Settings::Singleton()->new_setting('allow_registration', true, false, false, true, 6);

//Setting show_login
//true / false
Settings::Singleton()->new_setting('show_login', true, false, false, true, 6);

//Setting show_help
//true / false
Settings::Singleton()->new_setting('show_help', true, false, false, true, 5);

//Setting show_access
//true / false
Settings::Singleton()->new_setting('show_access', true, false, false, true, 5);

//Setting shop
//Global option to enable the shop
Settings::Singleton()->new_setting('shop', true, false, false, true, 7);

//Setting show_shop
//If a product page has been created automaticly add the shop option to the menu
Settings::Singleton()->new_setting('show_shop', true, false, true, true, 7);

//Setting shop_menu_name
//Menu name for the shop
Settings::Singleton()->new_setting('shop_menu_name', 'Shop', false, true, true, 7);

//Setting shop_tax_rate
//Local Tax rate as a decimal ie 15% is 0.15
Settings::Singleton()->new_setting('shop_tax_rate', 0, false, false, true, 10);

//Setting currency
//currency code GBP is Great British Pound, use the paypal codes
Settings::Singleton()->new_setting('currency', 'GBP', false, false, true, 10);

//Setting currency_symbol
Settings::Singleton()->new_setting('currency_symbol', '£', false, false, true, 10);

//Setting business_id
//The unique id used with your payment provider
Settings::Singleton()->new_setting('business_id', null, false, false, true, 7);

//Settings shop_email_orders
//An email address that all orders will be BCC to
Settings::Singleton()->new_setting('shop_email_orders', null, false, false, true, 7);


//Setting skin
//Set this to the path of the skin you wish to use
//Folder name from cms/skins/
Settings::Singleton()->new_setting('skin', 'default', false, false, true, 4);

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

//Use YUI compressed version of MooTools
Settings::Singleton()->new_setting('js_use_yui', 0, true, true);

//Setting js_cache
//This setting allows the javascript once built to be cached on the file system
//The reduces the number of external scripts called, improves compression and reduces load on your server
//The folder with in the CMS 'writable' or '/tmp/cms' needs to be writeable by the web server
Settings::Singleton()->new_setting('js_cache', false, false, true);

//Setting js_use_growler
//This setting is to include the growler style notifications
//Aditional JS will be loaded to enable this
//Fall back is to use Mootools Fade bar which also support's colours :-)
Settings::singleton()->new_setting('js_use_growler', true, false, false, true);

//Setting js_use_imgage_lazyload
//This setting is to use Javascript to stop loading of images that are in the HTML but not in the current view 
//Images are dynamically loaded with a nice holding image when they become viewable
//Aditional JS will be loaded to enable this
Settings::singleton()->new_setting('js_use_imgage_lazyload', false, false, false, true);


Settings::Singleton()->new_setting('css_minify', true, false, true);

//Setting search
//Allow site searching
//true / false
Settings::Singleton()->new_setting('search', true, false, false, true);

//Setting use_lucence_search
//Allow the use of the zend version of lucence which is a filesystem based index, oftern faster and more reliable than MySQL 
//Does not require any specific database code so is more cross platform complient
//true / false
Settings::Singleton()->new_setting('use_lucence_search', true, false, true, true);

//Setting related_pages
//Allow site related_pages searching
//true / false
Settings::Singleton()->new_setting('related_pages', true, false, true, true);

//Setting search_with_security
//Allow searching when a user is logged in to include pages with security
//Slower but gives more results
//true / false
Settings::Singleton()->new_setting('search_with_security', true, false, true, true);

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
Settings::singleton()->new_setting('phpids', false, false, true, true);

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
Settings::Singleton()->new_setting('uploads_enabled', true, false, false, true);

//download_rate
//Maximum file download rate in kb/s
Settings::Singleton()->new_setting('download_rate', NULL, false, true, true);

//stream
//Set the path for the streaming server,
//use /download/ for simple downloads or '/video/' for php based streaming
Settings::Singleton()->new_setting('stream', '/video/');

/* Session security */
//Setting session_security_enhanced
//Requires that the headers for the user_agent is constant with when the session was created
//true / false
Settings::Singleton()->new_setting('session_security_enhanced', true, false, true, true);

//Setting session_security_ip
//Requires the origonating IP to be constant with when the session was created
//true / false
Settings::Singleton()->new_setting('session_security_ip', false, false, true, true);

//Setting cookie_encryption
//Use cookie encryption, set to false to supress the warning if your server does not support encryption.
//true / false
Settings::Singleton()->new_setting('cookie_encryption', true, false, true, true);

//Setting login_capthca
//Requires the user to enter a security code when login in
//true / false
Settings::Singleton()->new_setting('login_capthca', true, false, false, true);

//Setting login_javascript_secure
//If the users webbrowser suports JS then the users login is encrptyed using the capthca as part of the key
//true / false
Settings::Singleton()->new_setting('login_secure', true, false, false, true);

//Setting login_capthca_length
//Sets the length of the image
//int max 8
Settings::Singleton()->new_setting('login_capthca_length', 5, false, true, true);

//Setting login_capthca_remember
//If the user logs in with remember me do not use capthca
//Defaults to true so capthca is not used if the cookie is authenticated
//true / false
Settings::Singleton()->new_setting('login_capthca_remember', true, false, false, true);

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
Settings::Singleton()->new_setting('google_ads', false, false, false, true);
Settings::Singleton()->new_setting('google_ad_slot', '6057605872', false, false, true);
Settings::Singleton()->new_setting('google_ad_client', 'pub-6675893347627358', false, false, true);
Settings::Singleton()->new_setting('google_ad_width', 180, false, false, true);
Settings::Singleton()->new_setting('google_ad_height', 150, false, false, true);

//Setting css_mobile
//Enable the mobile CSS, this overwrites css styles with non positional css
Settings::Singleton()->new_setting('css_mobile', true, false, false, true);

//Setting update_version_check
//To enable auto version checking set this value to the update server
//If the log dir is not writeable auto update checking will be disabled
Settings::Singleton()->new_setting('update_version_check', 'version.spensierato.net', false, true, false, false, true);

//Setting update_version_https_only
//Enable to only allow secure updates
//This will be the default in the future once auto updates work safely
Settings::Singleton()->new_setting('update_version_https_only', false, false, true, false, false, true);

//Setting update_version_check_schedule
//Set the timeperiod between version update checking in days
//If the log dir is not writeable auto update checking will be disabled
//default: 7
Settings::Singleton()->new_setting('update_version_check_schedule', 7, false, true, false, false, true);

//Setting user_incorect_logins
//To enable brute force login protection set the max number of failed logins per username
//On reaching the value a new password is generated
Settings::Singleton()->new_setting('user_incorect_logins', 10, false, true, false, false, true);

//Setting user_auto_email_password
//To enable the system to automatacily email new passwords if re-set by the system
//The email includes a note about account security
Settings::Singleton()->new_setting('user_auto_email_password', true, false, true, true);

Settings::singleton()->new_setting('memcached_sql', false, false, true, true);
Settings::singleton()->new_setting('memcached_session', false, false, true, true);

Settings::singleton()->new_setting('virus_scanner', 'clamav', false, true, true);
//Settings::singleton()->new_setting('virus_scanner_path', '/path/to/custom/scanner/install/', false, true);

//setting date_format
//this determines the date format for the website.
//use date()-compliant format directives
Settings::singleton()->new_setting('date_format', 'd/m/Y', false, true, true);

//setting show_username_in_approvals
//this determines whether to show the username in the pending approvals list in element editing
Settings::singleton()->new_setting('approvals_show_revisor', true, false, false, true);

//output_compression_level
//Sets the GZip compression level set to 0 to disable
Settings::singleton()->new_setting('output_compression_level', 9, false, true);

//2 settings to control how long both saved and pending approvals
//are saved in the elements_data_approve table
Settings::singleton()->new_setting('approvals_saved_lifetime', 0, false, true);
Settings::singleton()->new_setting('approvals_pending_lifetime', 0, false, true);

//determine whether old approvals are visible (value = 1) or hidden (value = 0) by default
Settings::singleton()->new_setting('approvals_show_old', true, false, false, true);

if(!$install) //Settings that require the install to have been completed
{
	//Temporay uploads folder
	Settings::Singleton()->new_setting('cms_folder_uploads', $GLOBALS['cms_folder_uploads'], false, true);

	//Permanant uploads folder
	Settings::Singleton()->new_setting('cms_folder_downloads', $GLOBALS['cms_folder_downloads'], false, true);

	//Unscaled images
	Settings::Singleton()->new_setting('cms_folder_orig_images', $GLOBALS['cms_folder_orig_images'], false, true);

	//Log files
	Settings::Singleton()->new_setting('cms_folder_logs', $GLOBALS['cms_folder_logs'], false, true);
	
	//Search Index files
	Settings::Singleton()->new_setting('cms_folder_search', $GLOBALS['cms_folder_search'], false, true);

	//Load the random key into the settings singleton
	Settings::Singleton()->new_setting('random_key', $GLOBALS['random_key'], true, true);

	//Setting mod_rewrite
	//Use mod_rewrite to clean the URL's
	Settings::singleton()->new_setting('mod_rewrite', $GLOBALS['allow_rewrite'], false, true);

	//setting server_os
	//this setting contains the server's os
	//used to toggle os specific finctionality (e.g. ffmpeg)
	Settings::singleton()->new_setting('server_os', $GLOBALS['server_os'], false, true);
}