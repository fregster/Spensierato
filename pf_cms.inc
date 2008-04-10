<?
//Include all the options for the CMS
//Functions requires by globals
function get_site_host()
{
	$a = substr($_SERVER['PHP_SELF'], 0 , strripos($_SERVER['PHP_SELF'], '.php'));
	return substr($a, 0, strripos($a, '/'));
}
$GLOBALS['site_root'] = get_site_host();

//Include the Site setup code
require_once('pf_cms_setup.inc');

//Include the CMS
require_once('cms/cms_includes.inc');

//Include the Options file
require_once('pf_cms_options.inc');

//Start site
CMS::Singleton()->start();

//Make any custom changes here after the CMS code has loaded but before the site is ran
if(is_file('custom.inc')) { include('custom.inc'); }

//Run the CMS
CMS::Singleton()->run_cms();
?>