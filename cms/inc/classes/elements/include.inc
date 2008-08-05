<?php
$path = substr(__FILE__, 0, -11);  

require_once($path.'default.inc');

foreach (glob($path.'*.inc') as $filename) 
{
   require_once($filename);
}
?>