<?php

if(!defined('CACHE_PREFIX') && setup::singleton()->load('random_key')){
	DEFINE('CACHE_PREFIX', setup::singleton()->load('random_key'));
}

if(!defined('CACHE_PATH') && setup::singleton()->load('cms_folder_cache')){
	DEFINE('CACHE_PATH', setup::singleton()->load('cms_folder_cache').DS);	
}
//	if(!defined('CACHE_TYPE')){
//		DEFINE('CACHE_TYPE', 'file');
//	}