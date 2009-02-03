<?php
/**
 * Information and options for the skin
 */
Settings::Singleton()->new_setting('template_dtd', 'xslt');
Settings::Singleton()->new_setting('template_header_graphic', array('headerfill.png', 'header-green.png'));

/**
 * Any skin specific images that you want to use javascript to preload
 * All files in the pre-load-images folder are always included with the code below
 */

foreach (glob(Settings::Singleton()->get_setting('cms_root').'/skins/'.Settings::Singleton()->get_setting('skin') .'/pre-load-images/*') as $filename) 
{
	array_push($this->preload, basename($filename));
}

?>