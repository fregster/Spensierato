<?php
/*
If in admin mode then load all elements as required for admin bar even if not currently in use
Load all classes in admin includes as these are functions that require to be loaded before the XSLT etc
*/
global $class_block_list;
$admin_paths = array('path_elements' => DIR_CLASSES .'element'.DS, 
					'path_admin' => DIR_CLASSES.'adminincludes'.DS,
);
$path_len = strlen(DIR_CLASSES);

require_once($admin_paths['path_elements'].'default.inc');

foreach($admin_paths as $paths)
{
	if(is_dir($paths))
	{
		$path_to_class = implode('_', explode(DS, substr($paths, $path_len)));
		foreach (glob($paths.'*.inc', GLOB_NOSORT) as $filename) 
		{
			$class = splitFilename($filename);
			$pos_class_name = $path_to_class.$class[0];
			
			if(!class_exists($class[0]) and !isset($class_block_list[$class[0]]) and !class_exists($pos_class_name) and !isset($class_block_list[$pos_class_name]))
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