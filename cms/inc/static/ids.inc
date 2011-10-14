<?php
if(file_exists($external_path.'IDS'.DIRECTORY_SEPARATOR.'Init.php'))
{
	try {
		set_include_path($external_path);
		require_once 'IDS'.DIRECTORY_SEPARATOR.'Init.php';
		$request = array(
		        //'REQUEST' => $_REQUEST,
		        'SERVER' => $_SERVER,
		        'GET' => $_GET,
		        'POST' => $_POST,
		        //'COOKIE' => $_COOKIE
		    );
		    
		//Server vars that are considered to be safe
		unset($request['SERVER']['PATH']);
		unset($request['SERVER']['HTTP_ACCEPT_CHARSET']);
		
		//CMS Vars that are likely to trigger false positives. These must be amazingly filtered, sanatized blah blah blah
		unset($request['POST']['meta_keywords']);
		unset($request['POST']['html']);
		unset($request['SERVER']['HTTP_COOKIE']);
		
		$config_file = $external_path.'IDS'.DIRECTORY_SEPARATOR.'Config'.DIRECTORY_SEPARATOR.'Config.ini.php';
		if(file_exists(DIR_CONFIG.'Config.ini.php')){
			$config_file = DIR_CONFIG.'Config.ini.php';
		}
		    
		$ids_init = IDS_Init::init($config_file);
		$ids_init->config['General']['use_base_path'] = false;
		$ids_init->config['General']['base_path'] = $external_path;
		$ids_init->config['General']['filter_path'] = $external_path.'IDS'.DIRECTORY_SEPARATOR.'default_filter.xml';
		$ids_init->config['General']['tmp_path'] = sys_get_temp_dir();	
		$ids_init->config['Caching']['tmp'] = sys_get_temp_dir();	
		$ids_init->config['Caching']['path'] = sys_get_temp_dir().'/default_filter.cache';	
		
	    if(file_exists($ids_init->config['General']['filter_path']))
	    {
			//Initiate the PHPIDS and fetch the results
			$ids = new IDS_Monitor($request, $ids_init);
			$result = $ids->run();

			if($result->getImpact() > 10)
			{
				echo 'IDS Triggered';
				if(defined('DEVELOPMENT_INSTALL')){
					echo '<p>'.$result.'</p>';
				}
				exit;
			}
	    }
	    //Clean up now redundent vars
	    unset($request);
		unset($ids_init);
		unset($ids);
		unset($result);
		unset($config_file);
	}
	catch(Exception $e)
	{
		printf('An IDS error occured: %s',$e->getMessage());
	    exit;
	}
}