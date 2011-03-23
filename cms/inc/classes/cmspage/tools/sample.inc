<?php
/**
 * This file is a simple tool for generating data for performance testing
 */

//Disable any output compression
ini_set('zlib.output_compression', 'Off');

/**
 * Force headers of no cache
 */
$expires = gmdate('D, d M Y H:i:s', (time() - 36000)) . ' GMT';
header('Cache-Control: private, no-cache, must-revalidate'); // HTTP/1.1
header('Expires: ' . $expires); // Date in the past
header('pragma: private'); 


//Size of data file 128*1024 = 128Kb
$length=128*1024;

//Max time in seconds
$max_time = 2;

//Force ob_off to allow for fast streaming
ob_end_flush();

$start = time();
$i = 0;
while ($i < $length)
{
//	echo(substr('abcdfghjkmnpqrstvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890', mt_rand(0, strlen('abcdfghjkmnpqrstvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890')-1), 1));
	echo 'a';
	$i++;
	//Check if we've been sending data for to long, ie a slow connection
	if(($start+$max_time)< time()){ exit; }
}

exit;