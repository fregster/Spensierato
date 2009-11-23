<?php
class clamav extends virusscan
{
	private $use_deamon_version = true;
	private $command_minimal;
	protected $command = 'clamscan'; //Default command overwriten in the construct if using the deamon version
	protected $possible_av_locations = array('/usr/local/bin/', '/usr/bin/'); //FreeBSD and Debian default paths

	protected function __construct()
	{
		if($this->use_deamon_version)
		{
			$this->command = 'clamdscan';
		}

		if(Settings::singleton()->get_setting('virus_scanner_path') != false)
		{
			$this->possible_av_locations[] = Settings::singleton()->get_setting('virus_scanner_path');
		}

		array_reverse($this->possible_av_locations);

		parent::__construct();

		//Fall back if file is not found by using just the command (Works on Debian and Ubuntu)
		$this->command_minimal = $this->command;
		$this->command .= ' --no-summary '; //print_r($this);die();
	}

	public function scan_file($file)
	{
		$av = parent::scan_file($file);

		/*
		 * clamav return codes
		 * 0 : No virus found.
		 * 1 : Virus(es) found.
		 * 2 : An error occured.
		 *
		 * Note 127 will be returned from shell if clamdscan was not found
		 */
		switch ($av)
		{
			case 0:
				return false;
			break;

			case 1:
				return 'Virus Scan ClamAV'.$this->version().' found a virus';
			break;
			
			case (settings::singleton()->get_setting('virus_scanner_pass_on_error') == true);
				return false;
			break;
			
			case 2:
				return 'Virus Scan Failed';
			break;

			case 127:
				return 'Virus Scanner not found';
			break;

			default:
				return 'Virus Scan Failed for an unknow reason error code: '.$av.' was retuned';
			break;
		}
	}

	public function version()
	{
		$version = null;
		passthru($this->command_minimal.' --version', $version);

		return $version;
	}
}