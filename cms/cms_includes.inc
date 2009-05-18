<?php
//Any functions required before classes are included
function cms_root()
{
	return(dirname(__FILE__));
}

//This gets the file system root to the index.php
$index_root = '';
function file_root()
{
	global $index_root;
	
	if(isset($index_root[1]))
	{
		return $index_root;
	}
	$list = get_included_files();
	$index_root = dirname($list[0]);
	return $index_root;
}

//Classes files
/* Test to see if __autoloader works */
//require_once('inc/classes/include.inc');

//Calculated vars
//None

//Include the Options file
require_once($include_path.'/spensierato_options.inc');

//Custom includes
//None
?>