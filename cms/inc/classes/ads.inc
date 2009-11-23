<?php
abstract class ad
{
	static function google()
	{
		echo('
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>Google Adverts</title>
  </head>
  <body>
<script type="text/javascript">
<!--
google_ad_client = "'.Settings::singleton()->get_setting('google_ad_client').'"
google_ad_slot = "'.Settings::singleton()->get_setting('google_ad_slot').'";
google_ad_width = '.Settings::singleton()->get_setting('google_ad_width').';
google_ad_height = '.Settings::singleton()->get_setting('google_ad_height').';
-->
</script>
<script type="text/javascript" src="http://pagead2.googlesyndication.com/pagead/show_ads.js"></script>
  </body>
</html>');
	}
}