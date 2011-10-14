<?php 

function exception_handler($exception = NULL)
{
	header('HTTP/1.1 500 Internal Server Error');
	echo('<html><head></head><body>');
	echo('<h1>Opps</h1><p>Unfortunatly an uncaught exception happened.<br/>Exception: ' . $exception->getMessage().'</p>');
	$backtrace = debug_backtrace();
//			var_dump($backtrace);
	echo($backtrace[0]['args'][1].'<br/>');
	echo(basename($backtrace[0]['args'][2]).' : '. $backtrace[0]['args'][3].'<br/>');
	echo('</body></html>');
	exit;
  
}

function uncaught_error_handler($errno, $errstr, $errfile, $errline)
{
	if (!(error_reporting() & $errno)) {
        // This error code is not included in error_reporting probably intentially supressed
        return;
    }
    
	switch ($errno) {
	
	    case E_USER_WARNING:
	    case E_WARNING:
	        echo "<b>WARNING</b> [$errno] $errstr ".basename($errfile) . ' ' . $errline."<br />\n";
	        break;
	
	    case E_USER_NOTICE:
	    case E_NOTICE:
	    	 echo "<b>NOTICE</b> [$errno] $errstr ".basename($errfile) . ' ' . $errline."<br />\n";
	        break;
	
	    default:
	        header('HTTP/1.1 500 Internal Server Error');
	        echo('<html><head></head><body>');
			echo('<h1>Opps</h1><p>Unfortunatly an uncaught error happened.<br/>Error Code: '.$errno.'</p>');
			$backtrace = debug_backtrace();
//			var_dump($backtrace);
			echo($backtrace[0]['args'][1].'<br/>');
			echo(basename($backtrace[0]['args'][2]).' : '. $backtrace[0]['args'][3].'<br/>');
			echo('</body></html>');
			exit;
	        break;
	   }
}