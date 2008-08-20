<?php
/*
This file should now be obsolete as __autoloader is defined
If in admin mode then load all elements as required for admin bar even if not currently in use 
*/

if(Options::Singleton()->get_option('admin_mode'))
{
	$path = substr(__FILE__, 0, -11);  
	
	require_once($path.'elementdefault.inc');
	
	foreach (glob($path.'*.inc') as $filename) 
	{
		$class = splitFilename($filename);
		if(!class_exists($class[0]))
		{
	  	 	require_once($filename);
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