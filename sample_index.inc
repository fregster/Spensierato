<?php
/**
 * An example of how to setup a custom Spensierato install
 */

//$force_install = true;

/**
 * Put a spensierato_options.inc file parall to this file to force custom options note this will be included AFTER the CMS options file so it's for overwriting the settings
 * 
 * Put a spensierato_setup.inc file paralle to this file for all the DB settings
 */

//Link to index.php to force error reporting when using the SVN version
//require_once('../../spensierato/trunk/public_html/index.php');
define('SITE_DIR', dirname(__FILE__).DIRECTORY_SEPARATOR);
require_once('../../spensierato/spensierato_cms.inc');