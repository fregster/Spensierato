<?
//include this file to call all includes in the correct order
require_once 'MDB2.php';

//Database files
require_once('database/database.inc');

//Any functions required before classes are included
function cms_root()
{
	return(str_replace('/home/paul/ftp/', '/var/www/fryer.org.uk/test/', dirname(__FILE__)));
}

//Classes files
/* Test to see if __autoloader works */
//require_once('inc/classes/include.inc');

//Calculated vars
//None

//Include the Options file
require_once('spensierato_options.inc');

if(Options::Singleton()->get_option('admin_mode'))
{
	require_once('cms_admin_includes.inc');
}

//Custom includes
//None
?>