<?php
DEFINE('DS', DIRECTORY_SEPARATOR);
DEFINE('DIR_writable', INCLUDE_PATH.'cms'.DS.'writable'.DS);
DEFINE('DIR_LOG', DIR_writable.'logs');
DEFINE('DIR_STATIC', INCLUDE_PATH.'cms'.DS.'inc'.DS.'static'.DS);
DEFINE('DIR_CLASSES', INCLUDE_PATH.'cms'.DS.'inc'.DS.'classes'.DS);
DEFINE('DIR_EXTERNAL', INCLUDE_PATH.'cms'.DS.'external'.DS);
DEFINE('DIR_TEMPLATE', INCLUDE_PATH.'cms'.DS.'template'.DS);
DEFINE('DIR_SKINS', DIR_TEMPLATE.'skins'.DS);
DEFINE('DIR_INDEX', dirname($_SERVER['SCRIPT_FILENAME']).DS);
DEFINE('DIR_ROOT', realpath(DIR_INDEX.'..'.DS).DS);
DEFINE('DIR_CONFIG', DIR_ROOT.'config'.DS);
DEFINE('DIR_JS', INCLUDE_PATH.'cms'.DS.'js'.DS);
if(!defined('ERROR_LOG_DIR')){ define('ERROR_LOG_DIR', DIR_ROOT.'writable'.DS.'logs'.DS.'errors'); }