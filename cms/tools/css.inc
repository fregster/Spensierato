<?
	// initialize ob_gzhandler function to send and compress data
  ob_start ("ob_gzhandler");
  //ob_start();
  session_start();
  
  $path = Settings::Singleton()->get_setting('cms_root').'/skins/'.Settings::Singleton()->get_setting('skin') .'/css.css';
	
	if(is_file($path))
	{
		// send the requisite header information and character set
		header ('content-type: text/css; charset: '.Settings::Singleton()->get_setting('char_set'));

		// check cached credentials and reprocess accordingly
		header ('cache-control: must-revalidate');

		// set variable for duration of cached content
		$offset = 60 * 60;

		// set variable specifying format of expiration header
		$expire = 'expires: ' . gmdate ("D, d M Y H:i:s", time() + $offset) . ' GMT';

		// send cache expiration header to the client broswer
		header ($expire);

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