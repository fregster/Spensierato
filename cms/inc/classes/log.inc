<?php
abstract class log {

	/*
	 	'id' => $int.' UNSIGNED NOT NULL '.$increment,
		'ts' => $int.' UNSIGNED NOT NULL',
		'security' => $int.' UNSIGNED NOT NULL',
		'user' => $int.' UNSIGNED NOT NULL',
		'action' => varchar(128).' NOT NULL',
		'detail' => varchar(255).' NOT NULL',
		'ip' => varchar(64). ' NOT NULL',
		'request_type' => varchar(12).' NOT NULL',
		'request_data' => $long_text,
		'request_uri' => varchar(255).' NOT NULL',
		'user_agent' => varchar(128).' NOT NULL',
		'ids' => $long_text,
		'trigger_file' => varchar(64). ' NOT NULL',
		'trigger_line' => $int.' UNSIGNED NOT NULL',
		
		sql_insert($table, $fields, $values)
	 */
	
	public static function record($action, $detail, $security = false){
		
		global $ids_result;
		$backtrace = debug_backtrace();
		
		//Drop fields that we do not want to log!
		if(isset($_REQUEST['password'])){
			unset($_REQUEST['password']);
		}
		
		$vaules = array(
			CMS::$time, 										//ts
			$security, 											//security
			user::Singleton()->id(), 							//user
			$action, 											//action
			$detail, 											//detail
			$_SERVER['REMOTE_ADDR'].':'.$_SERVER['REMOTE_PORT'], //IP:Port
			$_SERVER['REQUEST_METHOD'], 						//Request type
			serialize($_REQUEST),								//Full request object
			$_SERVER['REQUEST_URI'],							//Request URI
			$_SERVER['HTTP_USER_AGENT'], 						//User Agent
			serialize($ids_result), 												//IDS data
			basename($backtrace[0]['file']), 					//Trigger file
			$backtrace[0]['line'], 								//Trigger line
		);
		
		database::Singleton()->force_write_user();
		$result = database::Singleton()->sql_insert('tbl_log', array('ts', 'security', 'user', 'action', 'detail', 'ip', 'request_type', 'request_data', 'request_uri', 'user_agent', 'ids', 'trigger_file', 'trigger_line'), $vaules);
		database::Singleton()->destroy_write_user();
		
		return $result;
	}
	
	/**
	 * 
	 * Have any security alerts been triggered since the stated timestamp
	 * @param int $timestamp
	 */
	public static function audit($timestamp){
		
	}
}