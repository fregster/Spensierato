<?
// Standard error function
function error($text, $e = NULL)
{	
	if(isset($e))
	{
		echo $e->getMessage();
	}
	die($text);
}

// Load error page
function error_page($code, $text)
{
	include('../../../inc/pages/error.inc');
}
?>