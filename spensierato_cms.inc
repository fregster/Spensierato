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
			return true;
		}

		if(file_exists($path.'/cms/inc/classes/'.$class_name.'.inc'))
		{
			require_once $path.'/cms/inc/classes/'.$class_name.'.inc';
			return true;
		}
		else if (file_exists($path.'/cms/inc/classes/elements/'.$class_name.'.inc'))
		{
			require_once $path.'/cms/inc/classes/elements/'.$class_name.'.inc';
			return true;
		}
		else if (file_exists($path.'/cms/inc/classes/optional/'.$class_name.'.inc'))
		{
			require_once $path.'/cms/inc/classes/optional/'.$class_name.'.inc';
			return true;
		}
		else
		{
			 die('<p>Critical error: Class "'.$class_name.'" not defined</p>');
		}
	}
}

set_exception_handler('exception_handler');
try
{
	//Include all the options for the CMS
	//Functions requires by globals
	function get_site_document_root()
	{
    	$document_root = $GLOBALS['document_root_folder_only'];

    	if(!Common::rewrite_enabled() && $GLOBALS['allow_rewrite'])
    	{
    		$document_root .= '/index.php';
    	}

		return $document_root;
	}
	
	function get_site_document_root_folder_only()
    {
    	$a = substr($_SERVER['PHP_SELF'], 0 , strripos($_SERVER['PHP_SELF'], '.php'));
    	$document_root = substr($a, 0, strripos($a, '/'));
    	if($document_root == '')
    	{
    		if( ($_SERVER['HTTPS'] == '') || ($_SERVER['HTTPS'] == 'off') )
    		{
    			$proto = 'http://';
    		}
    		else
    		{
    			$proto = 'https://';
    		}
    		$document_root = $proto . $_SERVER['SERVER_NAME'];
    		
    	}
		return $document_root;
    }

	function get_file_root()
	{
		$ext = substr($_SERVER[SCRIPT_FILENAME], -4 , 4);
		$a = substr($_SERVER[SCRIPT_FILENAME], 0 , strripos($_SERVER[SCRIPT_FILENAME], $ext));
		return substr($a, 0, strripos($a, '/'));
	}

	//document root is the URL root ie spensierato.net
	$GLOBALS['document_root_folder_only'] = get_site_document_root_folder_only();
	$GLOBALS['document_root'] = get_site_document_root();
	
	//site root is the filesystem root ie /var/www/spensierato/
	$GLOBALS['site_root'] = get_file_root();

	//default is to install the cms
	$install = true;

	//Include the Site setup code
	if(is_file('./spensierato_setup.inc'))
	{
		require_once('./spensierato_setup.inc');
		$install = false; //cancel the install directive
	}

	//Include the CMS
	require_once('./cms/cms_includes.inc');

	//uncomment to force an install
	//$install = true;

	//run the installation
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