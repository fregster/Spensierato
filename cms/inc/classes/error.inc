<?
// Standard error functions
abstract class error
{
	public function error($text, $e = NULL)
	{	
		if(isset($e))
		{
			echo $e->getMessage();
		}
		die($text);
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
?>