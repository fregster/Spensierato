<?php
/* This file should now be obsolete as __autoloader is defined */


require_once('settings.inc');
require_once('cms.inc');
$path = substr(__FILE__, 0, -11);  

foreach (glob($path.'*.inc') as $filename) 
{
   require_once($filename);
}
?>