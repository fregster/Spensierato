<?php
DEFINE('DS', DIRECTORY_SEPARATOR);
DEFINE('DIR_WRITEABLE', $include_path.DS.'cms'.DS.'writeable'.DS);
DEFINE('DIR_LOG', DIR_WRITEABLE.'logs');
DEFINE('DIR_STATIC', $include_path.DS.'cms'.DS.'inc'.DS.'static'.DS);
DEFINE('DIR_CLASSES', $include_path.DS.'cms'.DS.'inc'.DS.'classes'.DS);
DEFINE('DIR_EXTERNAL', $include_path.DS.'cms'.DS.'external'.DS);
DEFINE('CACHE_PREFIX', $_SERVER['SERVER_NAME']);