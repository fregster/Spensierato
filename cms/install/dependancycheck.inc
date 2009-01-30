<?php
$required = array('session', 'SimpleXML', 'mysqli', 'hash', 'xml', 'xmlreader', 'xmlwriter', 'xsl');
$optional = array('ffmpeg', 'gd', 'mcrypt', 'tidy');

$req_missing = array();
$opt_missing = array();

if(!version_compare(phpversion(), '5.2', '>=')) 
{
	if(!extension_loaded($req)) { array_push($req_missing, '<strong>PHP5.2 or greater is required</strong>'); }
}

foreach($required as $req)
{
	if(!extension_loaded($req)) { array_push($req_missing, $req); }
}

foreach($optional as $opt)
{
	if(!extension_loaded($opt)) { array_push($opt_missing, $opt); }
}
?>