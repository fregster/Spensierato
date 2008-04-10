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
require_once('inc/classes/include.inc');

//Calculated vars
//None

//Custom includes
//None
?>