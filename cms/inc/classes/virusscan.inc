<?php
class virusscan
{
	protected static $instance;
	protected $command;
	protected $enabled = false;
	protected $possible_av_locations = array();

	public static function singleton()
	{
	   if (!isset(self::$instance)) {


	   		if(class_exists('Settings') && Settings::singleton()->get_setting('virus_scanner') && class_exists('virusscan_'.Settings::singleton()->get_setting('virus_scanner')))
	   		{
	   			$c = 'virusscan_'.Settings::singleton()->get_setting('virus_scanner');
	   		}
	   		else
	   		{
	   			$c = __CLASS__;
	   		}

		   self::$instance = new $c;
	   }

	   return self::$instance;
	}

	protected function __construct()
	{
		foreach($this->possible_av_locations as $path)
		{
			if(file_exists($path.$this->command))
			{
				$this->command = $path.$this->command; //Overwrite the command with the full path call
				$this->enabled = true;
				break;
			}
		}
	}

	/**
	 * Default file virus scan method
	 *
	 * @param path to $file
	 * @return false if no virus found or a string if found. returns false on fail
	 */
	public function scan_file($file)
	{
		if($this->enabled)
		{
			$av = true;
			$output = array();

			//Execute the command
			exec($this->build_exec_command($file), $output, $av);
//			print_r($av); die();
			return $av;
		}
		return false;
	}
	
	private function is_malware($file)
	{
		if(Settings::singleton()->get_setting('')){
			return malware::singleton()->is_malware($file);
		}
	}

	/**
	 * This is just a proxy method to scan_file to return true or false to make if statments easy
	 *
	 * @param string $file
	 */
	public function is_not_virus($file)
	{
		if(malware::singleton()->is_malware($file))
		{
			return false;
		}
		
		if(!$this->enabled)
		{
			return true;
		}
		
		if(!$this->scan_file($file))
		{
			return true;
		}
		return false;
	}

	/**
	 * This builds the cli exe command to scan a file
	 * @param $file
	 * @return string
	 */
	protected function build_exec_command($file)
	{
		return $this->command.' '.$file;
	}

	/**
	 * Return the virusscan version
	 * @return string of version
	 */
	public function version()
	{
		return '0';
	}
}