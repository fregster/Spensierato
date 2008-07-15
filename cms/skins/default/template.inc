<?php
/**
 * Information and options for the skin
 */

Settings::Singleton()->new_setting('template_header_graphic', array('headerfill.png', 'header-green.png'));

$preload = array();

foreach (glob(Settings::Singleton()->get_setting('cms_root').'/skins/'.Settings::Singleton()->get_setting('skin') .'/pre-load-images/*') as $filename) 
{
	array_push($preload, basename($filename));
}
if(isset($preload[0]))
{
	Settings::Singleton()->new_setting('pre_load_images', $preload);
}
?>