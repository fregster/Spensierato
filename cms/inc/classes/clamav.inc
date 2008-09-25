<?php
class clamav extends virusScan
{
	private $use_deamon_version = true;
	
	function __construct()
	{
		parent::construct();
		
		if($this->use_deamon_version)
		{
			$command = 'clamdscan';
		}
		else
		{
			$command = 'clamscan';
		}
		
		if(Settings::singleton()->get_setting('virus_scanner_path') == false)
		{
			if(file_exists('/usr/local/bin/'.$command))
			{
				$this->command = '/usr/local/bin/'.$command;
			}
			else if(file_exists('/usr/bin/'.$command))
			{
				$this->command = '/usr/bin/'.$command;
			}
			else
			{
				//Socket not found try using basic command (Works on Debian and Ubuntu)
				$this->command = $command;
			}
		}
		 $this->command .= ' --no-summary '; 
	}

	public function scan_file($file)
	{
		$av = true;
		$output = array();
		$command = $this->command.' '.$file;
		
		//Execute the command
		exec($command, $output, $av);
		
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
				return $output[sizeof($output) - 1];
			break;
			
			case 2:
				return 'Virus Scan Failed';
			break;
			
			case 127:
				return 'Virus Scanner command not found';
			break;
			
			default:
				return 'Virus Scan Failed for an unknow reason error code: '.$av.' was retuned';
			break;
		}	
	}
}

?>