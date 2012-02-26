<?php
/**
 * Spensierato boot loader script, bit messy but boot loaders are.
 */

//Start Output buffering
ob_start();

//uncomment to force an install
//	$force_install = true;
//default is to install the cms
$install = true;

//Force a timezone to stop PHP complaning
date_default_timezone_set('UTC');


$class_block_list = array(
//'elementvideo' => 1,
//'elementgallery' => 1,
//'elementlink' => 1,
);

define('INCLUDE_PATH', dirname(__FILE__).DIRECTORY_SEPARATOR);
$default_include_path = get_include_path();

//Do static requires
require_once(INCLUDE_PATH.'cms'.DIRECTORY_SEPARATOR.'inc'.DIRECTORY_SEPARATOR.'static'.DIRECTORY_SEPARATOR.'defines.inc');
require_once(DIR_STATIC.'error_handels.inc');
$external_path = INCLUDE_PATH.'cms'. DIRECTORY_SEPARATOR .'external'. DIRECTORY_SEPARATOR;

/**
 * Check this is not an attack using IDS
 */
require_once(DIR_STATIC.'ids.inc');

/**
 * Force the use of these error handlers
 */
set_error_handler('uncaught_error_handler');
set_exception_handler('exception_handler');
register_shutdown_function('shutdown_handler');

set_include_path($default_include_path.PATH_SEPARATOR.INCLUDE_PATH.'cms'. DIRECTORY_SEPARATOR .'external'. DIRECTORY_SEPARATOR);
unset($default_include_path);

require_once(DIR_STATIC.'autoloader.inc');
require_once(DIR_STATIC.'functions.inc');

//Force any PHP settings
ini_set('error_log', DIR_LOG.DS.'php_error.log');
//ini_set('display_errors', 'Off');


try
{
	//Make any custom changes here that are required pre the CMS code has loaded
	if(is_file(DIR_CONFIG.'early_custom.inc')) { include(DIR_CONFIG.'early_custom.inc'); }
	
	//Include all the options for the CMS
	//Functions requires by globals

	//document root is the URL root ie spensierato.net
	define('DOCUMENT_ROOT_FOLDER_ONLY', get_site_document_root_folder_only());
	define('DOCUMENT_ROOT', get_site_document_root());
	define('DOCUMENT_PATH', get_site_path());

	//Has an early custom defines file been specified
	if(!$install){
		require_once(INCLUDE_PATH.'spensierato_custom.inc');
		if(is_file(DIR_CONFIG.'spensierato_custom.inc')) { require_once(DIR_CONFIG.'spensierato_custom.inc'); }
	}
	
	//Include the Site setup code
	if(is_file(DIR_CONFIG.'spensierato_setup.inc'))
	{
		require_once(DIR_CONFIG.'spensierato_setup.inc');
		$install = false; //cancel the install directive
	}
		
	require_once(DIR_STATIC.'late_defines.inc');
	
	//Set any late or custom defines
	if(file_exists(DIR_CONFIG.'spensierato_custom_defines.inc')){
		include_once(DIR_CONFIG.'spensierato_custom_defines.inc');
	}
	
	Settings::Singleton()->build_defines(
		array(
			'SETTING_GROUP_SITE' => 1,
			'SETTING_GROUP_LOCALE' => 10,
			'SETTING_GROUP_METADATA' => 20,
			'SETTING_GROUP_GOOGLE' => 30,
			'SETTING_GROUP_SKIN' => 40,
			'SETTING_GROUP_USER' => 50,
			'SETTING_GROUP_RENDER_OPTIONS' => 60,
			'SETTING_GROUP_SHOP' => 70,
			'SETTING_GROUP_CONTENT_CONTROL' => 80,
			'SETTING_GROUP_LOGIN' => 90,
			'SETTING_GROUP_SEARCH' => 100,
			'SETTING_EXTERNAL_API_KEYS' => 150,
			'SETTING_GROUP_PERFORMANCE' => 500,
			'SETTING_GROUP_SECURITY' => 750,
			'SETTING_GROUP_SYSTEM' => 1000,
			'SETTING_GROUP_UPDATE_CONTROL' => 1100,
		)
	);
	
	//Has the site extended the spensierato options?
	if(!$install){
		require_once(INCLUDE_PATH.'spensierato_options.inc');
		if(is_file(DIR_CONFIG.'spensierato_options.inc')) { require_once(DIR_CONFIG.'spensierato_options.inc'); }
	}
	
	//run the installation
	if($install || isset($force_install))
	{
		$install = true;
		include(INCLUDE_PATH.DS.'cms'.DS.'install'.DS.'install.inc');
		exit;
	}
	
	//Start site
	CMS::Singleton()->start(); 

	//Make any custom changes here after the CMS code has loaded but before the site is ran
	if(is_file(DIR_CONFIG.'custom.inc')) { include(DIR_CONFIG.'custom.inc'); }

	//Run the CMS
	CMS::Singleton()->run_cms();
}
catch (Exception $e)
{
    echo 'Caught exception: ',  $e->getMessage(), "\n";
}