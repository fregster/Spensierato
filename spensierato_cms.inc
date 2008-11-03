<?php
function exception_handler($exception) 
{
  echo 'Uncaught exception: ' , $exception->getMessage(), "\n";
}

/**
 * Auto class loading function
 *
 * @param string $class_name
 */
function __autoload($class_name)
{
	if($class_name != '' and $class_name != null and !class_exists($class_name))
	{
		$class_name = strtolower($class_name);
		$path = dirname(__FILE__);
		
		if($class_name == 'database')
		{
			//Database files
			require_once($path.'/cms/database/database.inc');
		}
		
		if(file_exists($path.'/cms/inc/classes/'.$class_name.'.inc'))
		{
			require_once $path.'/cms/inc/classes/'.$class_name.'.inc';
		}
		else if (file_exists($path.'/cms/inc/classes/elements/'.$class_name.'.inc'))
		{
			require_once $path.'/cms/inc/classes/elements/'.$class_name.'.inc';
		}
		else if (file_exists($path.'/cms/inc/classes/optional/'.$class_name.'.inc'))
		{
			require_once $path.'/cms/inc/classes/optional/'.$class_name.'.inc';
		}
		else
		{
			 die('Critical error: Class "'.$class_name.'" not defined');
		}
	}
}

set_exception_handler('exception_handler');
try 
{
	//Include all the options for the CMS
	//Functions requires by globals
	function get_site_host()
	{
		$a = substr($_SERVER['PHP_SELF'], 0 , strripos($_SERVER['PHP_SELF'], '.php'));
		return substr($a, 0, strripos($a, '/'));
	}
	$GLOBALS['site_root'] = get_site_host();
	$install = true;
	
	//Include the Site setup code
	if(is_file('./spensierato_setup.inc'))
	{
		require_once('./spensierato_setup.inc');
		$install = false;
	}
	
	//Include the CMS
	require_once('./cms/cms_includes.inc');
	
	if($install)
	{
		include('./cms/install/install.inc');
		die();
	}
	
	//Start site
	CMS::Singleton()->start();
	
	//Make any custom changes here after the CMS code has loaded but before the site is ran
	if(is_file('custom.inc')) { include('custom.inc'); }
	
	//Run the CMS
	CMS::Singleton()->run_cms();
} 
catch (Exception $e) 
{
    echo 'Caught exception: ',  $e->getMessage(), "\n";
}
?>