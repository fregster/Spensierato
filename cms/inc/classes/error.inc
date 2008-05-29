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
	
	public function exception($exception)
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
		die('403 Forbiden');
	}
	
	public static function do404()
	{
		die('404 File not found');
	}
}
//Over write the exception handler to use the now loaded error class
set_exception_handler(array('error','exception'));
?>