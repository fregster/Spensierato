<?php
set_include_path(
   get_include_path()
   . PATH_SEPARATOR
   . dirname(__FILE__) .'/lib/'
  );

  require_once 'IDS/Init.php';
  $request = array(
      'REQUEST' => $_REQUEST,
      'GET' => $_GET,
      'POST' => $_POST,
      'COOKIE' => $_COOKIE
  );
  
  try
  {
	  $init = IDS_Init::init(dirname(__FILE__) .'/config.ini');
	
	  $ids = new IDS_Monitor($request, $init);
	  $result = $ids->run();
	
	  if (!$result->isEmpty()) {
	
	  	//Log the information
	//   require_once 'IDS/Log/File.php';
	//   require_once 'IDS/Log/Composite.php';
	//   
	//   $compositeLog = new IDS_Log_Composite();
	//   $compositeLog->addLogger(IDS_Log_File::getInstance($init));
	//   $compositeLog->execute($result);
	
	   // Take a look at the result object
	   //TODO email $result to support
	//   echo $result;
		//TODO forward to better error page
		die('IDS error');
		   
	  }
  }
  
  catch(Exception $e)
  {
  	Warnings::register('PHPIDS', 'An exception was thrown ('.$e->getMessage().'). PHPIDS has been disabled', 3);
  }
  
set_include_path(NULL);
?>