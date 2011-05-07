<?php
if(!defined('CACHE_PREFIX') && isset($GLOBALS['random_key'])){
	DEFINE('CACHE_PREFIX', $GLOBALS['random_key']);
}
if(!defined('CACHE_PATH') && isset($GLOBALS['cms_folder_cache'])){
	DEFINE('CACHE_PATH', $GLOBALS['cms_folder_cache'].DS);	
}
//	if(!defined('CACHE_TYPE')){
//		DEFINE('CACHE_TYPE', 'file');
//	}