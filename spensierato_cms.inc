<?php
//uncomment to force an install
	$force_install = true;

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

require_once($include_path.'/cms/inc/static/autoloader.inc');
require_once($include_path.'/cms/inc/static/functions.inc');

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

	//default is to install the cms
	$install = true;

	//Include the Site setup code
	if(is_file(file_root().'/spensierato_setup.inc'))
	{
		require_once(file_root().'/spensierato_setup.inc');
		$install = false; //cancel the install directive
	}
	
	//Has the site extended the spensierato options?
	require_once($include_path.'/spensierato_options.inc');
	if(is_file(file_root().'/spensierato_options.inc')) { require_once(file_root().'/spensierato_options.inc'); }

	//run the installation
	if($install || isset($force_install))
	{
		include($include_path.'/cms/install/install.inc');
		die();
	}
	
	//Start site
	CMS::Singleton()->start(); 

	//Make any custom changes here after the CMS code has loaded but before the site is ran
	
	if(is_file(file_root().'/custom.inc')) { include(file_root().'/custom.inc'); }

	//Run the CMS
	CMS::Singleton()->run_cms();
}
catch (Exception $e)
{
    echo 'Caught exception: ',  $e->getMessage(), "\n";
}
?>