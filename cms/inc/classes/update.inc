<?php
/**
 * The update class
 * 
 * This is for the Spensierato auto update functionality
 * Major increments are for versions that can NOT be auto upgraded to
 * Minor increments are for major new features or ANY database schema change
 * Incremental versions are for any code only features, code improvments and upgrades, bug fixes and security updates.
 * 
 * @author Paul Fryer
 *
 */

class update
{
	private $latest = true;
	private $major;
	private $minor;
	private $incremental;
	
	private function __construct()
	{
		$versions = explode('.', cms::$cms_version);
		update::$major = $versions[0];
		update::$minor = $versions[1];
	}
	
	public function version()
	{
		return update::major.'.'.update::minor.'.'.update::incremental;
	}
	
	/**
	 * TODO finish this, it should check a TXT record at spensierato.net for version information.
	 * It should raise a warning if an updated release has been made.
	 * @return bool
	 */
	
	public function check($force = false)
	{
		$hostname = Settings::singleton()->get_setting('update_version_check');
		if($hostname != NULL)
		{
			return false;
		}


		$update_log = DIR_LOG.'update.log';
		if(file_exists($update_log))
		{
			$pervious_update_check = file_get_contents($update_log);
		}
		$update_interval = Settings::Singleton()->get_setting('update_version_check_schedule')*24*60*60;

		if((isset($pervious_update_check) && $pervious_update_check+$update_interval > time()) || $force)
		{

			$update_log = fopen($update_log, 'w');

			fwrite($update_log, time());
			fclose($update_log);


			//This function does not work in windows fo fake it
			if(!function_exists('checkdnsrr'))
			{
			    function checkdnsrr($host, $type='')
			    {
			        if(!empty($host))
			        {
			            $type = (empty($type)) ? 'MX' :  $type;
			            exec('nslookup -type='.$type.' '.escapeshellcmd($host), $result);
			            $it = new ArrayIterator($result);
			            foreach(new RegexIterator($it, '~^'.$host.'~', RegexIterator::GET_MATCH) as $result)
			            {
			                if($result)
			                {
			                    return true;
			                }
			            }
			        }
			        return false;
			   	}
			}

			//If a DNS record is found
			if(checkdnsrr($hostname, 'A'))
			{
				$dns = dns_get_record($hostname, DNS_TXT);
				
				$live_version = explode('.', substr($dns['A'], 4));
				
				if($live_version[0] > update::$major) //TODO rewrite this to switch on major minor and inc updates with diffrent text. Auto update not available on Major upgrades and as of yet Minor upgrades as there is no auto DB Schema update
				{
					warnings::Singleton()->register('New version of Spensierato released', 'There is a new Major Relese version available ('.$live_version[0].'), please visit the spensierato website for details');
					return true;
				}
				else if($live_version[1] > update::$minor)
				{
					warnings::Singleton()->register('New version of Spensierato released', 'There is a new version available ('.$live_version[0].'.'.$live_version[1].') to download which might contain new features, bug fixes and security updates, please visit the spensierato website for details');
					return true;
				}
				else if($live_version[2] > update::$incremental)
				{
					warnings::Singleton()->register('New version of Spensierato released', 'There is a new version available ('.$live_version[0].'.'.$live_version[1].'.'.$live_version[2].') The release might contain bug fixes and security updates or new features. An autoupdate might be available depending on your configuration');
					return true;
				}
				else
				{
					warnings::Singleton()->register('Auto update check', 'Update check shows you are running the latest version of Spensierato '.$live_version[0].'.'.$live_version[1].'.'.$live_version[2], 3);
					return false;
				}
			}
		}
		return false;
	}
	
	public function download($force = false)
	{
		if(update::check() || $force)
		{
			$host = 'http';
			if(Settings::singleton()->get_setting('update_version_https_only'))
			{
				$host .= 's';
			}
			
			$host .= '://'.Settings::singleton()->get_setting('update_version_check').'/spensierato_latest.zip';
		}
	}
	
	public function update()
	{
		
	}
}