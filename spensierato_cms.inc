<?
//Include all the options for the CMS
//Functions requires by globals
function get_site_host()
{
	$a = substr($_SERVER['PHP_SELF'], 0 , strripos($_SERVER['PHP_SELF'], '.php'));
	return substr($a, 0, strripos($a, '/'));
}
$GLOBALS['site_root'] = get_site_host();
$install = true;

//Include the Site setup code
if(is_file('spensierato_setup.inc'))
{
	require_once('spensierato_setup.inc');
	$install = false;
}

//Include the CMS
require_once('cms/cms_includes.inc');

if($install)
{
	include('cms/install/install.inc');
	die();
}

//Include the Options file
require_once('spensierato_options.inc');

//Start site
CMS::Singleton()->start();

//Make any custom changes here after the CMS code has loaded but before the site is ran
if(is_file('custom.inc')) { include('custom.inc'); }

//Run the CMS
CMS::Singleton()->run_cms();
?>