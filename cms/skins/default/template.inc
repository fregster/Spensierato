<?php
/**
 * Information and options for the skin
 */

Settings::Singleton()->new_setting('template_header_graphic', array('headerfill.png', 'header-green.png'));

$preload = array();
foreach (glob('../pre-load-images/*') as $filename) 
{
   array_push($preload);
}
if(isset($preload[0]))
{
	Settings::Singleton()->new_setting('pre_load_images', $preload);
}
?>