<?php
/**
 * Healthcheck for site timeout
 */

if(User::Singleton()->is_logged_in()){
	die('1');
}

die('0');