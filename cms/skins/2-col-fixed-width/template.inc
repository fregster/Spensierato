<?php
menu::singleton()->set_options(array('top_menu_depth' => 2));

Settings::Singleton()->new_setting('template_header_graphic_left', 'company-logo.png');
Settings::Singleton()->new_setting('template_header_graphic_right', 'prity-picutre.png');
Settings::Singleton()->new_setting('template_header_graphic_background', 'prity-background.png');

Settings::Singleton()->overwrite_setting('template_extends', 'default', false, true);
?>