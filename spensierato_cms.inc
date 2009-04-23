<?php
function exception_handler($exception)
{
  echo 'Uncaught exception: ' , $exception->getMessage(), "\n";
}

/**
 * Auto class loading function
 * $class_block_list are for elements or classes that are not ready for production
 * Uncoment them for testing
 *
 * @param string $class_name
 */
$class_block_list = array(
//'elementvideo' => 1,
//'elementgallery' => 1,
//'elementlink' => 1,
);
$include_path = dirname(__FILE__);
$class_profile_use_true_memory = false;
$class_profile = array(array('class' => 'inital code', 'mem_diff' => (memory_get_usage($class_profile_use_true_memory))));
function __autoload($class_name)
{
	global $class_block_list;
	global $include_path;
	global $class_profile;
	global $class_profile_use_true_memory;
	$trace = false;

	$class_name = strtolower($class_name);

	$paths = array(		$include_path.'/cms/inc/classes/',
						$include_path.'/cms/inc/classes/elements/',
						$include_path.'/cms/inc/classes/optional/',
					);

	if($class_name != '' and $class_name != null and !class_exists($class_name) and !isset($class_block_list[$class_name])) //Ensure we should be trying to load the class
	{
		foreach($paths as $path) //Cycle through the includes dir in order of preference
		{
			if(file_exists($path.$class_name.'.inc')) //Check the file exists
			{
				if($trace) { $start_time = explode(" ",microtime()); $start_time = $start_time[1] + $start_time[0]; $start_mem = memory_get_usage($class_profile_use_true_memory); }
					require_once $path.$class_name.'.inc';
				if($trace) { $mtime = explode(" ",microtime()); $mtime = $mtime[1] + $mtime[0]; $class_profile[] = array('class' => $class_name, 'mem_diff' => (memory_get_usage($class_profile_use_true_memory) - $start_mem) / 1024, 'load_time' => $mtime - $start_time); }
					return true;
			}
		}

		die('<p>Critical error: Class "'.$class_name.'" not defined</p>'); //A class has been called the can not be loaded
	}
	return false; //Should never really get here
}

function load_extended_class($base_class, $new_class)
{
	global $include_path;
	$base_len = strlen($base_class)+1;
	$class_filename = $new_class;
	if(substr($new_class, 0, $base_len) == ($base_class.'_'))
	{
		$class_filename = substr($new_class, $base_len);
	}

	include_once($include_path.'/cms/inc/classes/'.$base_class.'/'.$class_filename.'.inc');
	return class_exists($new_class);
}

set_exception_handler('exception_handler');
try
{
	//Include all the options for the CMS
	//Functions requires by globals
	function get_site_document_root()
	{
    	$document_root = $GLOBALS['document_root_folder_only'];

    	if(!Common::rewrite_enabled() && $GLOBALS['allow_rewrite']) //We use the globals here as the settings class might not be loaded
    	{
    		$document_root .= '/index.php';
    	}

		return $document_root;
	}

	function get_site_document_root_folder_only() //This does NOT work if we have a front end proxy, needs thought on a possible solution.
    {
    	$a = substr($_SERVER['PHP_SELF'], 0 , strripos($_SERVER['PHP_SELF'], '.php'));
    	$document_root = substr($a, 0, strripos($a, '/'));
    	if($document_root == '')
    	{
    		$proto = 'http://';
    		if(isset($_SERVER['HTTPS']))
    		{
	    		if( ($_SERVER['HTTPS'] != '') && ($_SERVER['HTTPS'] != 'off') )
	    		{
	    			$proto = 'https://';
	    		}
    		}
    		$document_root = $proto . $_SERVER['SERVER_NAME'];

    	}
		return $document_root;
    }

	function get_file_root()
	{
		$ext = substr($_SERVER['SCRIPT_FILENAME'], -4 , 4);
		$a = substr($_SERVER['SCRIPT_FILENAME'], 0 , strripos($_SERVER['SCRIPT_FILENAME'], $ext));
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
//	$install = true;

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