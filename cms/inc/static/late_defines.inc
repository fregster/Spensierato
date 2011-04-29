<?php
if(!defined('CACHE_PATH')){
	DEFINE('CACHE_PREFIX', $GLOBALS['random_key']);
}
if(!defined('CACHE_PATH')){
	DEFINE('CACHE_PATH', $GLOBALS['cms_folder_cache'].DS);	
}
//	if(!defined('CACHE_TYPE')){
//		DEFINE('CACHE_TYPE', 'file');
//	}