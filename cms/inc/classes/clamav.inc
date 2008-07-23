<?php
require virusScan.inc;
class clamav extends virusScan
{
	private function __constructor()
	{
		parent::__constructor();
		if(Settings::singleton()->get_setting('virus_scanner_path') == false)
		{
			$this->command = '/usr/local/bin/clamdscan --no-summary ';
		}	
	}

	public function scan_file($file)
	{
		$av = true;
		$output = array();
		$command = $this->command.' '.$file;
		
		//Execute the command
		exec($command, $output, $av);
		
		if($av == false)
		{
			return false;
		}
		
		return $output[sizeof($output) - 1];
	}
}

?>