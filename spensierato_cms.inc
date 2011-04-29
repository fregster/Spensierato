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

function exception_handler($exception = NULL)
{
	header('HTTP/1.1 500 Internal Server Error');
	echo('<html><head></head><body>');
	echo('<h1>Opps</h1><p>Unfortunatly an uncaught exception happened.<br/>Exception: ' . $exception->getMessage().'</p>');
	$backtrace = debug_backtrace();
//			var_dump($backtrace);
	echo($backtrace[0]['args'][1].'<br/>');
	echo(basename($backtrace[0]['args'][2]).' : '. $backtrace[0]['args'][3].'<br/>');
	echo('</body></html>');
	exit;
  
}

function uncaught_error_handler($errno, $errstr, $errfile, $errline)
{
	if (!(error_reporting() & $errno)) {
        // This error code is not included in error_reporting probably intentially supressed
        return;
    }
    
	switch ($errno) {
	
	    case E_USER_WARNING:
	    case E_WARNING:
	        echo "<b>WARNING</b> [$errno] $errstr ".basename($errfile) . ' ' . $errline."<br />\n";
	        break;
	
	    case E_USER_NOTICE:
	    case E_NOTICE:
	    	 echo "<b>NOTICE</b> [$errno] $errstr ".basename($errfile) . ' ' . $errline."<br />\n";
	        break;
	
	    default:
	        header('HTTP/1.1 500 Internal Server Error');
	        echo('<html><head></head><body>');
			echo('<h1>Opps</h1><p>Unfortunatly an uncaught error happened.<br/>Error Code: '.$errno.'</p>');
			$backtrace = debug_backtrace();
//			var_dump($backtrace);
			echo($backtrace[0]['args'][1].'<br/>');
			echo(basename($backtrace[0]['args'][2]).' : '. $backtrace[0]['args'][3].'<br/>');
			echo('</body></html>');
			exit;
	        break;
	   }
}

set_error_handler('uncaught_error_handler');


$class_block_list = array(
//'elementvideo' => 1,
//'elementgallery' => 1,
//'elementlink' => 1,
);

define('INCLUDE_PATH', dirname(__FILE__).DIRECTORY_SEPARATOR);
$default_include_path = get_include_path();

/**
 * Check this is not an attack using IDS
 */
if(file_exists('IDS'.DIRECTORY_SEPARATOR.'Init.php'))
{
	try {
		$external_path = INCLUDE_PATH.'cms'. DIRECTORY_SEPARATOR .'external'. DIRECTORY_SEPARATOR;
		set_include_path($external_path);
		require_once 'IDS'.DIRECTORY_SEPARATOR.'Init.php';
		$request = array(
		        'REQUEST' => $_REQUEST,
		        'GET' => $_GET,
		        'POST' => $_POST,
		        'COOKIE' => $_COOKIE
		    );
		
		$ids_init = IDS_Init::init($external_path.'IDS'.DIRECTORY_SEPARATOR.'Config'.DIRECTORY_SEPARATOR.'Config.ini.php');
		$ids_init->config['General']['use_base_path'] = false;
		$ids_init->config['General']['base_path'] = $external_path;
		$ids_init->config['General']['filter_path'] = 'default_filter.xml';
	    if(file_exists($ids_init->config['General']['filter_path']))
	    {
			//Initiate the PHPIDS and fetch the results
			$ids = new IDS_Monitor($request, $ids_init);
			$result = $ids->run();
			
			if(!$result->isEmpty())
			{
				echo $result;
				exit;
			}
	    }
	    //Clean up now redundent vars
	    unset($request);
		unset($ids_init);
		unset($ids);
		unset($result);	
	}
	catch(Exception $e)
	{
		printf('An IDS error occured: %s',$e->getMessage());
	    exit;
	}
}
set_include_path($default_include_path.PATH_SEPARATOR.INCLUDE_PATH.'cms'. DIRECTORY_SEPARATOR .'external'. DIRECTORY_SEPARATOR);
unset($default_include_path);

//Do static requires
require_once(INCLUDE_PATH.'cms'.DIRECTORY_SEPARATOR.'inc'.DIRECTORY_SEPARATOR.'static'.DIRECTORY_SEPARATOR.'defines.inc');
require_once(DIR_STATIC.'autoloader.inc');
require_once(DIR_STATIC.'functions.inc');

//Force any PHP settings
ini_set('error_log', DIR_LOG.DS.'php_error.log');
//ini_set('display_errors', 'Off');

set_exception_handler('exception_handler');
try
{
	//Make any custom changes here that are required pre the CMS code has loaded
	if(is_file(DIR_INDEX.'early_custom.inc')) { include(DIR_INDEX.'early_custom.inc'); }
	
	//Include all the options for the CMS
	//Functions requires by globals

	//document root is the URL root ie spensierato.net
	define('DOCUMENT_ROOT_FOLDER_ONLY', get_site_document_root_folder_only());
	define('DOCUMENT_ROOT', get_site_document_root());
	define('DOCUMENT_PATH', get_site_path());

	//Has an early custom defines file been specified
	if(!$install){
		require_once(INCLUDE_PATH.'spensierato_custom.inc');
		if(is_file(DIR_INDEX.'spensierato_custom.inc')) { require_once(DIR_INDEX.'spensierato_custom.inc'); }
	}
	
	//Include the Site setup code
	if(is_file(DIR_INDEX.'spensierato_setup.inc'))
	{
		require_once(DIR_INDEX.'spensierato_setup.inc');
		$install = false; //cancel the install directive
	}
	
	require_once(DIR_STATIC.'late_defines.inc');
	
	//Set any late or custom defines
	if(file_exists(DIR_INDEX.'spensierato_custom_defines.inc')){
		include_once(DIR_INDEX.'spensierato_custom_defines.inc');
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
		if(is_file(DIR_INDEX.'spensierato_options.inc')) { require_once(DIR_INDEX.'spensierato_options.inc'); }
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
	if(is_file(DIR_INDEX.'custom.inc')) { include(DIR_INDEX.'custom.inc'); }

	//Run the CMS
	CMS::Singleton()->run_cms();
}
catch (Exception $e)
{
    echo 'Caught exception: ',  $e->getMessage(), "\n";
}