<?php
		//This file contains the CMS settings required before we include the CMS code.

		//Database vars
		//The DB layer suports 2 users read and readwrite
		//readwrite is used once logged in to provide another layer of prtection against sql injection.

		$GLOBALS['database_host'] = 'localhost';
		$GLOBALS['database_driver'] = 'mysqli';
		$GLOBALS['database_name'] = 'spensierato';
		$GLOBALS['database_user_read'] = 'spensierato_read';
		$GLOBALS['database_user_readwrite'] = 'spensierato_writ';
		$GLOBALS['database_password'] = 'pncdjjhmbanrvhyf';
		$GLOBALS['table_prefix'] = 'xbj_';
		$GLOBALS['allow_rewrite'] = true;
		$GLOBALS['cms_folder_uploads'] = 'writeable/uploads/tmp/';
		$GLOBALS['cms_folder_downloads'] = 'writeable/uploads/';
		$GLOBALS['cms_folder_orig_images'] = 'writeable/uploads/original_images/';
		$GLOBALS['cms_folder_logs'] = 'writeable/logs/';
		$GLOBALS['cms_folder_cache'] = 'writeable/cache/';
		?>