<?php

Settings::Singleton()->overwrite_setting('template_extends', 'default');
Settings::singleton()->new_setting('template_footer_text', '<p>Sun Rise Template</p>');

//Settings::Singleton()->new_setting('template_header_graphic_left', null);
//Settings::Singleton()->new_setting('template_header_graphic_right', null);
Settings::Singleton()->new_setting('template_header_graphic_background', 'headerbg.png');
Settings::Singleton()->new_setting('template_br_after_name', true);

$inherirted_css = Settings::Singleton()->get_setting('template_inherits_css');
$include = array('search.css', 'css.css', 'common.css', 'standard_sprite.css');
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
Settings::Singleton()->overwrite_setting('template_inherits_css', $include);

$js = "<script type='text/javascript'>
	window.addEvent('domready', function() {
		  var status = {
		    'true': 'Close',
		    'false': 'Open'
		  };
		var menuVSlide = new Fx.Slide('insidetop');
		menuVSlide.hide();
		$('v_toggle').addEvent('click', function(event){
		    event.stop();
		    menuVSlide.toggle();
		  });
		  menuVSlide.addEvent('complete', function() {
		    $('vertical_status').set('text', status[menuVSlide.open]);
		  });
	  });
	  </script>";
	  
content::singleton()->add_custom_head('script', array('type' => 'text/javascript'), $js);