<?php
//uncomment to force an install
//	$force_install = true;
//default is to install the cms
$install = true;
date_default_timezone_set('UTC');
function exception_handler($exception)
{
  echo 'Uncaught exception: ' , $exception->getMessage(), "\n";
}

$class_block_list = array(
//'elementvideo' => 1,
//'elementgallery' => 1,
//'elementlink' => 1,
);
$include_path = dirname(__FILE__);
$default_include_path = get_include_path();
/**
 * Check this is not an attack using IDS
 */
try {
	$external_path = $include_path.DIRECTORY_SEPARATOR.'cms'. DIRECTORY_SEPARATOR .'external'. DIRECTORY_SEPARATOR;
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
set_include_path($default_include_path);
unset($default_include_path);
require_once($include_path.DIRECTORY_SEPARATOR.'cms'.DIRECTORY_SEPARATOR.'inc'.DIRECTORY_SEPARATOR.'static'.DIRECTORY_SEPARATOR.'defines.inc');

ini_set('error_log', DIR_LOG.'php_error.log');
//ini_set('display_errors', 'Off');

require_once(DIR_STATIC.'autoloader.inc');
require_once(DIR_STATIC.'functions.inc');

set_exception_handler('exception_handler');
try
{
	//Include all the options for the CMS
	//Functions requires by globals
	

	//document root is the URL root ie spensierato.net
	$GLOBALS['document_root_folder_only'] = get_site_document_root_folder_only();
	$GLOBALS['document_root'] = get_site_document_root();

	//site root is the filesystem root ie /var/www/spensierato/
	$GLOBALS['site_root'] = get_file_root();

	//Include the Site setup code
	if(is_file(file_root().DS.'spensierato_setup.inc'))
	{
		require_once($index_root.DS.'spensierato_setup.inc');
		$install = false; //cancel the install directive
	}
	
	//Has the site extended the spensierato options?
	if(!$install){
		require_once($include_path.DS.'spensierato_options.inc');
		if(is_file($index_root.DS.'spensierato_options.inc')) { require_once($index_root.DS.'spensierato_options.inc'); }
	}
	//run the installation
	if($install || isset($force_install))
	{
		$install = true;
		include($include_path.DS.'cms'.DS.'install'.DS.'install.inc');
	}
	
	//Start site
	CMS::Singleton()->start(); 

	//Make any custom changes here after the CMS code has loaded but before the site is ran
	
	if(is_file(file_root().DS.'custom.inc')) { include(file_root().DS.'custom.inc'); }

	//Run the CMS
	CMS::Singleton()->run_cms();
}
catch (Exception $e)
{
    echo 'Caught exception: ',  $e->getMessage(), "\n";
}