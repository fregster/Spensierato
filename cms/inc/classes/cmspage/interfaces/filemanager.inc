<?php
ini_set('display_errors', 0);
error_reporting(E_ERROR);
require_once DIR_EXTERNAL.'filemanager'.DS.'FileManager.php';

function UploadIsAuthenticated($get){
	session::Singleton()->force();
	user::singleton()->set_user_details(true);
	return user::singleton()->is_admin();
}


$filepath = '';

$filepath = realpath(Settings::Singleton()->get_setting('cms_folder_downloads').DS.'mediamanager').DS;
if(!file_exists($filepath)){
	common::create_folders_from_path($filepath);
}if(!is_writable($filepath)){
	die(
	json_encode(array(
	'status' => 0,
	'error' => 'Root directory not writable',
	))
	);
}

$browser = new FileManager(array(
	'directory' => $filepath,
	'assetBasePath' => Settings::Singleton()->get_setting('document_root').Settings::Singleton()->get_setting('site_folder_js_direct').'/fileman/',
	'thumbnailPath' => $filepath.DS.'Thumbnails',
	'maxUploadSize' => Settings::Singleton()->get_setting('max_file_size'),
));

$browser->fireEvent(!empty($_GET['event']) ? $_GET['event'] : null);