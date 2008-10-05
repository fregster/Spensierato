<?php
$required = array('session', 'SimpleXML', 'mysqli', 'hash', 'xml', 'xmlreader', 'xmlwriter', 'xsl');
$optional = array('ffmpeg', 'gd', 'mcrypt', 'tidy');

$req_missing = array();
$opt_missing = array();

foreach($required as $req)
{
	if(!extension_loaded($req)) { array_push($req_missing, $req); }
}

foreach($optional as $opt)
{
	if(!extension_loaded($opt)) { array_push($opt_missing, $opt); }
}
?>