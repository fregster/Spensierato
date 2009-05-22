<?php
function cms_root()
{
	global $include_path;
	return($include_path.'/cms/');
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

function get_site_document_root()
{
    $document_root = $GLOBALS['document_root_folder_only'];

    if(!Common::rewrite_enabled() && $GLOBALS['allow_rewrite']) //We use the globals here as the settings class might not be loaded
    {
    	$document_root .= '/index.php';
    }

	return $document_root;
}

function get_site_document_root_folder_only() //This does NOT work if we have a front end proxy, needs thought on a possible solution.
    {
    $a = substr($_SERVER['PHP_SELF'], 0 , strripos($_SERVER['PHP_SELF'], '.php'));
    $document_root = substr($a, 0, strripos($a, '/'));
    if($document_root == '')
    {
    	$proto = 'http://';
    	if(isset($_SERVER['HTTPS']))
    	{
    		if( ($_SERVER['HTTPS'] != '') && ($_SERVER['HTTPS'] != 'off') )
    		{
    			$proto = 'https://';
    		}
    	}
    	$document_root = $proto . $_SERVER['SERVER_NAME'];

    }
	return $document_root;
    }

function get_file_root()
{
	$ext = substr($_SERVER['SCRIPT_FILENAME'], -4 , 4);
	$a = substr($_SERVER['SCRIPT_FILENAME'], 0 , strripos($_SERVER['SCRIPT_FILENAME'], $ext));
	return substr($a, 0, strripos($a, '/'));
}
?>