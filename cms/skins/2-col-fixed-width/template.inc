<?php
menu::singleton()->set_options(array('top_menu_depth' => 2));

Settings::Singleton()->new_setting('template_header_graphic_left', 'company-logo.png');
Settings::Singleton()->new_setting('template_header_graphic_right', 'prity-picutre.png');
Settings::Singleton()->new_setting('template_header_graphic_background', 'prity-background.png');

Settings::Singleton()->overwrite_setting('template_extends', 'default', false, true);

$inherirted_css = Settings::Singleton()->get_setting('template_inherits_css');
$include = array('common.css', 'sprite-minimal.css', '2_col_layout.css');
if($inherirted_css)
{
	if(is_array($inherirted_css))
	{
		array_merge($include, $inherirted_css);
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