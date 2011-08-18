<?php
class csrf {

	public static function build_token(){
		if(!Settings::singleton()->get_setting('csrf-name')){
			//Settings::singleton()->overwrite_setting();
			Settings::Singleton()->new_setting('csrf-name', substr(Common::return_hash(controller::$time), -6), true, false, false);
			Settings::Singleton()->new_setting('csrf-value', Common::return_hash(Settings::singleton()->get_setting('csrf-name')), true, false, false);
		}
	}
	
	public static function check_token(){
		if((!post::variable('csrf-name', 'isset') || !post::variable('csrf-value', 'isset')) || Common::return_hash(post::variable('csrf-name')) != post::variable('csrf-value')){
			error::do403();
		}
		return true;
	}
}