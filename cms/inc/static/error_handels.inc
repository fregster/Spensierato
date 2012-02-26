<?php 

/**
 * Set the required defines if not set prior to this include
 */
if(!defined('ERROR_LOG_DIR')){
	define('ERROR_LOG_DIR', dirname(__FILE__).DIRECTORY_SEPARATOR.'error_files');
}

if(!defined('ERROR_LOG_LEVEL')){
	define('ERROR_LOG_LEVEL', E_WARNING);
}

if(!defined('ERROR_LOG_DAYS')){
	define('ERROR_LOG_DAYS', 10);
}

/**
 * exception_handler
 * Uncaught exception handler
 * Proxies to the default error handler
 * @param object $exception
 */
function exception_handler($exception = NULL){
	$backtrace = debug_backtrace();
	uncaught_error_handler(E_ERROR, $exception->getMessage(), $backtrace[0]['args'][2], $backtrace[0]['args'][3]);
	exit;
  
}

/**
 * shutdown_handler
 * 
 * By default you should not be able to catch fatal errors
 * By having a shutdown function that looks at the error condition we can
 * Force the system to pass fatal error's through our error handler
 * 
 * True fatal error's are still logged in the default error logging files as defined by your PHP setup
 */
function shutdown_handler(){
	//Run the garbage collection routine
	error_garbage_collection();
	
	//Check if an error was thown that could not have been caught, process and pass to the error handler
	//This function MUST be perfect in suppressing and handeling errors
	if (@is_array($e = @error_get_last())) {
    	$code = isset($e['type']) ? $e['type'] : 0;
    	$msg = isset($e['message']) ? $e['message'] : '';
    	$file = isset($e['file']) ? $e['file'] : '';
    	$line = isset($e['line']) ? $e['line'] : '';
    	if ($code>0) uncaught_error_handler($code, $msg, $file, $line);
    }
}

/**
 * 
 * uncaught_error_handler
 * System error handeling logging and reporting function
 * @param int $errno
 * @param string $errstr
 * @param string $errfile
 * @param string $errline
 */
function uncaught_error_handler($errno, $errstr, $errfile, $errline){
	if (!(error_reporting() & $errno)) {
        // This error code is not included in error_reporting probably intentially supressed
        return null;
    }
    
    $backtrace = debug_backtrace();
    
    /*
     * Internal error handeling function to take the data and build a storable object
     */
    function create_error_object($errno, $errstr, $errfile, $errline, $backtrace){
    	
    	//Strip out var's we don't want to record
    	foreach(array('_SERVER', '_COOKIE', '_REQUEST') as $key){
	    	if(isset($backtrace[0]['args'][4][$key])){
	    		unset($backtrace[0]['args'][4][$key]);
	    	}
    	}
    	
    	//Build up the error store
    	$error_array = array(
    		'errorno'	=> $errno,
    		'filename' 	=> basename($errfile),
    		'path'		=> $errfile,
    		'line'		=>	$errline,
    		//'backtrace'	=> $backtrace,    	
    	);
    	
    	//The data array should now be a repeatable storable object
    	$serial = @serialize($error_array);
    	$hash = @sha1($serial);
    	if(!isset($hash) || $hash == false){
    		$hash = @sha1($error_array['path'].$error_array['line'].$error_array['errorno']);
    	}
    	log_to_file($hash, $serial);
    	return array('hash' => $hash, 'error' => $error_array);
    }
    
    /**
     * 
     * Takes the built errors and write it to the log dir if this error has not been recorded before
     * @param string $hash
     * @param string $input
     */
    function log_to_file($hash, $input){
    	$filename = ERROR_LOG_DIR.DIRECTORY_SEPARATOR.$hash;
    	
    	//Check that this error has not already been logged
    	if(!file_exists($filename)){
    		//This error has not been logged can we write to the dir
    		if(is_writable(ERROR_LOG_DIR)){
    			$handle = fopen($filename, 'a');
    			@fwrite($handle, $input);
    			fclose($handle);
    		}
    	} else {
    		//Update the logged file modified time to show that this error has happened again
    		@touch($filename);
    	}
    }
    
    function display_error_page($error){
    	/*
    	 * We should clear the output buffer, but it's not always started properly
    	 * Especially if people terminate a php file '?>' and leave a line break
    	 */
    	ob_end_clean();
    	@header('HTTP/1.1 500 Internal Server Error');
        echo('<html><head><title>Error Page</title></head><body class="fatal_error_body"><h1 class="fatal_error_header">Opps</h1>');
		echo('<p class="fatal_error_message">Unfortunatly an uncaught error happened.</p><pclass="fatal_error_code">Error code: '.$error['hash']).'</p>';
        if(defined('ERROR_DEBUG') && (ERROR_DEBUG == true || (@isset($_COOKIE['display_errors']) && $_COOKIE['display_errors'] == 'on'))){
    		if((@isset($_GET['html_errors']) && $_GET['html_errors'] == 'on') || (@isset($_COOKIE['html_errors']) && $_COOKIE['html_errors'] == 'on')){
    			@ini_set('html_errors', 'on');
				@ini_set('xdebug.var_display_max_data', 512);
				@ini_set('xdebug.var_display_max_depth', 6);
    		} 
    		
    		echo '<p><pre>';
    		htmlvardump($error['error']);
    		echo '</p></pre>';
    	}
		echo('</body></html>');
		exit;
    }
    
	switch ($errno) {
		//Fatal Errors
	    case E_CORE_ERROR:
	    case E_ERROR:
	         $sum = create_error_object($errno, $errstr, $errfile, $errline, $backtrace);
	         display_error_page($sum);
	        break;
	
	    default:
	        $sum = create_error_object($errno, $errstr, $errfile, $errline, $backtrace);
	    	display_error_page($sum);
	        break;
	   }

	   return null;
}

/**
 * 
 * Render the backtrace nicely
 * Use XDebug if installed else use the internal function
 * Function takes an array (Typically a debug backtrace) and renders it
 * @param array $var
 */
function htmlvardump($var){
	if(!ini_get('xdebug.overload_var_dump') || ini_get('xdebug.overload_var_dump') == false) {
		//Force the settings, mainly for testing
		ini_set('xdebug.overload_var_dump', false);
		ini_set('html_errors', false);
		ob_start(); 
		call_user_func_array('var_dump', $var); 
		echo '<pre>'.htmlentities(ob_get_clean()).'</pre>';
	} else {
		//XDebug is installed so we can use the improved and nicely formatted overloaded function
		ini_set('xdebug.var_display_max_data', 512);
		ini_set('xdebug.var_display_max_depth', 6); //Given the current backtrace format this should provide all the information required
		var_dump($var);
	}
}

/**
 * 
 * Garbage collection
 * Scan for and delete old log files
 * Random 1% chance of running every request
 */
function error_garbage_collection(){
	//1% chance of the garbage collection running, as we call the maintenance scripts every few min's combinded with traffic this should run enough times
	if(rand(0,100) == 100){
		$time = time();
		$error_files = glob(ERROR_LOG_DIR.DIRECTORY_SEPARATOR.'*');
		$max_age = 60*60*24*ERROR_LOG_DAYS;
		foreach ($error_files as $file){
			if ($time - filectime($file) > $max_age){
				@unlink($file);
			}
		}
	}
}