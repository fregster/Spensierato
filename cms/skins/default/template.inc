<?php
/**
 * Information and options for the skin
 */
Settings::Singleton()->new_setting('template_dtd', 'xslt');
Settings::Singleton()->new_setting('template_header_graphic', array('headerfill.png', 'header-green.png'));
Settings::Singleton()->overwrite_setting('template_verstion', cms::$cms_version, true);

/**
 * Any skin specific images that you want to use javascript to preload
 * All files in the pre-load-images folder are always included with the code below
 */

foreach (glob(Settings::Singleton()->get_setting('cms_root').DS.'skins'.DS.Settings::Singleton()->get_setting('skin').DS.'pre-load-images'.DS.'*') as $filename) 
{
	array_push($this->preload, basename($filename));
}

/**
 * Does this template extend another template?
 */
//Setting template_extends
//Settings::Singleton()->new_setting('template_extends', 'default', false, true);

/**
 * Should the template also inherits any CSS files?
 * array of css file names ie array('common.css', 'standard_sprite.css');
 */
$inherirted_css = Settings::Singleton()->get_setting('template_inherits_css');
$css = 'css.css';
if(Settings::Singleton()->get_setting('skin_alternative_css') != false)
{
	$css = Settings::Singleton()->get_setting('skin_alternative_css');
}
$include = array('common.css', 'standard_sprite.css', 'search.css', $css);
if($inherirted_css)
{
	if(is_array($inherirted_css))
	{
		$include = array_merge($include, $inherirted_css);
	}
	else
	{
		$include[] = $inherirted_css;
	}
}
Settings::Singleton()->overwrite_setting('template_inherits_css', $include, false, true);
unset($include);
unset($inherirted_css);
?>