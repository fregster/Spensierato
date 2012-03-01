<?php
/**
 * Index file created by Spensierato CLI Installer
 * Put a spensierato_setup.inc file in the dir above to this file for all the DB settings and basic site settings
 *
 * Put a spensierato_options.inc file in the dir above this file to force custom options
 * Note: This will be included AFTER the CMS options file so it's for overwriting the site options
 * 
 */

//Link to index.php to force error reporting when using the repository version
define('SITE_DIR', dirname(__FILE__).DIRECTORY_SEPARATOR);
