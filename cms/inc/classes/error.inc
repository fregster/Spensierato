<?php
// Standard error functions
abstract class error
{
	public function error($text, $exception = NULL)
	{	
		if(isset($exception))
		{
			echo $exception->getMessage();
		}
		die($text);
	}
	
	public static function exception($exception)
	{
		error::error('An exception was thrown', $exception);
	}
	
	// Load error page
	public static function error_page($code, $text)
	{
		include('../../../inc/pages/error.inc');
	}

	public static function do403()
	{
		header('HTTP/1.1 403 Forbidden');
		die('403 Forbiden');
	}
	
	public static function do404()
	{
		header('HTTP/1.1 404 Forbidden');
		die('404 File not found');
	}
	
	/**
	* Default error handling
	*/
	private static function log($msg)
	{
		error_log($msg, 3, "/var/tmp/my-errors.log");
	}
}
//Over write the exception handler to use the now loaded error class
set_exception_handler(array('error','exception'));