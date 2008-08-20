<?php
/*
This file should now be obsolete as __autoloader is defined
If in admin mode then load all elements as required for admin bar even if not currently in use 
*/

if(Options::Singleton()->get_option('admin_mode'))
{
	$path = substr(__FILE__, 0, -11);  
	
	require_once($path.'default.inc');
	
	foreach (glob($path.'*.inc') as $filename) 
	{
	   require_once($path.$filename);
	}
}
?>