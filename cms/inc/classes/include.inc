<?
$path = substr(__FILE__, 0, -11);  

//die($path);
foreach (glob($path.'*.inc') as $filename) 
{
   require_once($filename);
}

require_once($path.'elements/default.inc');

foreach (glob($path.'elements/*.inc') as $filename) 
{
   require_once($filename);
}
?>