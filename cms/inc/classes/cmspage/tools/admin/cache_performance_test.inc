<?php
$loops = 1000;

define('TEST_NAME', 'test string');
define('TEST_STRING', 'The quick brom fox jumped over the lazy dog');
define('TEST_NAMESPACE', 'test');
define('TEST_TIME', 120);

/**
 * Test cache
 */
echo 'Cache Type: '; var_dump(persistantcache::Singleton()->type());
echo 'Testing Save Compressed and Encrypted: '; var_dump(persistantcache::Singleton()->save(TEST_NAME, TEST_STRING, TEST_TIME, TEST_NAMESPACE, true, true));
echo 'Testing Get: '; var_dump(persistantcache::Singleton()->get(TEST_NAME, TEST_NAMESPACE));

function benchmark_get(){
	global $loops;
	$i = 0; 
	while($i < $loops){
		persistantcache::Singleton()->get(TEST_NAME, 'test');
		++$i;
	} 
}

function benchmark_save($encrypt = false, $compress = false){
	global $loops;
	$i = 0; 
	while($i < $loops){
		persistantcache::Singleton()->save(TEST_NAME, TEST_STRING, TEST_TIME, TEST_NAMESPACE, $encrypt, $compress);
		++$i;
	} 
}

function benchmark_both($encrypt = false, $compress = false){
	global $loops;
	$i = 0; 
	while($i < $loops){
		persistantcache::Singleton()->save(TEST_NAME, TEST_STRING, TEST_TIME, TEST_NAMESPACE, $encrypt, $compress);
		persistantcache::Singleton()->get(TEST_NAME, TEST_NAMESPACE);
		++$i;
	} 
}
/**
 * Test with both Encryption and Compression enabled
 */
echo('<p>Save Compressed and Encryped</p>');
echo('Timing save requrests<br/>');
$t = new Timer();
$t->start();
benchmark_save(true, true);
$t->stop(true);

echo('<br/>Timing get requrests<br/>');
$t = new Timer();
$t->start();
benchmark_get();
$t->stop(true);

echo('<br/>Timing mixed requrests<br/>');
$t = new Timer();
$t->start();
benchmark_both(true, true);
$t->stop(true);

/**
 * Test with just Encryption enabled
 */
echo('<p>Save Encryped</p>');
echo('Timing save requrests<br/>');
$t = new Timer();
$t->start();
benchmark_save(true, false);
$t->stop(true);

echo('<br/>Timing get requrests<br/>');
$t = new Timer();
$t->start();
benchmark_get();
$t->stop(true);

echo('<br/>Timing mixed requrests<br/>');
$t = new Timer();
$t->start();
benchmark_both(true, false);
$t->stop(true);

/**
 * Test with just Compression enabled
 */
echo('<p>Save Compressed</p>');
echo('Timing save requrests<br/>');
$t = new Timer();
$t->start();
benchmark_save(false, true);
$t->stop(true);

echo('<br/>Timing get requrests<br/>');
$t = new Timer();
$t->start();
benchmark_get();
$t->stop(true);

echo('<br/>Timing mixed requrests<br/>');
$t = new Timer();
$t->start();
benchmark_both(false, true);
$t->stop(true);

/**
 * Simple save, no compression or encryption
 */
echo('<p>Save plain</p>');
echo('Timing save requrests<br/>');
$t = new Timer();
$t->start();
benchmark_save();
$t->stop(true);

echo('<br/>Timing get requrests<br/>');
$t = new Timer();
$t->start();
benchmark_get();
$t->stop(true);

echo('<br/>Timing mixed requrests<br/>');
$t = new Timer();
$t->start();
benchmark_both();
$t->stop(true);

exit;