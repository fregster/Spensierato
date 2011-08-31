<?php
//This file contains the CMS settings required before we include the CMS code.

//Database vars
//The DB layer suports 2 users read and readwrite
//readwrite is used once logged in to provide another layer of prtection against sql injection.

setup::Singleton()->save_construct('database', 'Singleton', array(
		array(	'phptype' 	=> 'mysqli',
				'backend'	=> 'pdo',
				'username'	=> 'spensierato_read',
				'password'	=> 'fsaaxwgnzyarjkhf',
				'hostspec'	=> 'localhost',
				'database'	=> 'spensierato',
				'prefix'	=>	'cms_'),
		true,
		array(	'phptype' 	=> 'mysqli',
				'backend'	=> 'pdo',
				'username'	=> 'spensierato_writ',
				'password'	=> 'fsaaxwgnzyarjkhf',
				'hostspec'	=> 'localhost',
				'database'	=> 'spensierato',
				'prefix'	=>	'cms_'
		),
	)
);

setup::Singleton()->save_construct('session', 'Singleton', array());


setup::Singleton()->save('allow_rewrite', true);
setup::Singleton()->save('random_key', '2fe8b327381ba67d770d22fddff246dca71f0b4e');
setup::Singleton()->save('server_os', 'unix');
setup::Singleton()->save('max_sha_hash', 'sha512');
setup::Singleton()->save('session_handler', 'memcached');


setup::Singleton()->save('cms_folder_uploads', '/mnt/writable/uploads/tmp/');
setup::Singleton()->save('cms_folder_downloads', '/mnt/writable/uploads/');
setup::Singleton()->save('cms_folder_orig_images', '/mnt/writable/uploads/original_images/');
setup::Singleton()->save('cms_folder_logs', '/mnt/writable/logs/');
setup::Singleton()->save('cms_folder_cache', '/mnt/writable/cache/');
setup::Singleton()->save('cms_folder_search', '/mnt/writable/search/');

