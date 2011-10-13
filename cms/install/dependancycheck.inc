<?php
require_once(INCLUDE_PATH.DIRECTORY_SEPARATOR.'cms'.DIRECTORY_SEPARATOR.'inc'.DIRECTORY_SEPARATOR.'static'.DIRECTORY_SEPARATOR.'functions.inc');

$required = array('session', 'SimpleXML', 'mysqli', 'hash', 'xml', 'xmlreader', 'xmlwriter', 'xsl', 'mcrypt', 'iconv');
$recomended = array('ffmpeg', 'gd', 'pdo', 'pdo_mysql', 'pdo_sqlite');
$optional = array('tidy', 'pspell');
$pear = array('mdb2' => 'mdb2', 'sqlite' => 'MDB2_Driver_sqlite');

$req_missing = array();
$rec_missing = array();
$opt_missing = array();

$pdo_drivers = array();
if(class_exists('PDO'))
{
	$pdo_drivers = PDO::getAvailableDrivers();
	$pdo_drivers = array_flip($pdo_drivers);
}

if(!version_compare(phpversion(), '5.2', '>=')) 
{
	if(!extension_loaded($req)) { array_push($req_missing, '<strong>PHP5.2 or greater is required</strong>'); }
}

foreach($required as $req)
{
	if(!extension_loaded($req)) { array_push($req_missing, $req); }
}

foreach($recomended as $rec)
{
	if(!extension_loaded($rec)) { array_push($rec_missing, $rec); }
}

foreach($optional as $opt)
{
	if(!extension_loaded($opt)) { array_push($opt_missing, $opt); }
}

check_includes('MDB2.php');

function check_includes($include)
{
	global $opt_missing;
	if(@!include_once($include)) { array_push($opt_missing, $include); }
}

function check_rewrite()
{
	global $background_image;
	$handle = load($background_image);
	
	if($handle['info']['http_code'] == '200') //If the page has retured content then error
	{
		return true;
	}

	return false;
}