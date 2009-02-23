<?php
/*
If in admin mode then load all elements as required for admin bar even if not currently in use
Load all classes in admin includes as these are functions that require to be loaded before the XSLT etc
*/
global $class_block_list;
$path = dirname(__FILE__).'/';
$admin_paths = array('path_elements' => $path .'inc/classes/elements/', 
					'path_admin' => $path.'inc/classes/adminincludes/',
);

require_once($admin_paths['path_elements'].'elementdefault.inc');

foreach($admin_paths as $paths)
{
	if(is_dir($paths))
	{
		foreach (glob($paths.'*.inc') as $filename) 
		{
			$class = splitFilename($filename);
			if(!class_exists($class[0]) and !isset($class_block_list[$class[0]]))
			{
		  	 	require_once($filename);
			}
		}
	}
}

function splitFilename($filename)
{
	$filename = basename($filename);
    $pos = strrpos($filename, '.');
    if ($pos === false)
    { // dot is not found in the filename
        return array($filename, ''); // no extension
    }
    else
    {
        $basename = substr($filename, 0, $pos);
        $extension = substr($filename, $pos+1);
        return array($basename, $extension);
    }
} 
?>