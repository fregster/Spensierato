<?
	// initialize ob_gzhandler function to send and compress data
  //ob_start ("ob_gzhandler");
  ob_start();
  session_start();
  
  $path = Settings::Singleton()->get_setting('cms_root').'/skins/'.Settings::Singleton()->get_setting('skin') .'/css.css';
	
	if(is_file($path))
	{
		Cache::headers_allow_cache($path);
		
		//include CSS file
		include($path);
	}
	else
	{
		//Send 404
		//TODO write 404
		die($path);
	}
?>