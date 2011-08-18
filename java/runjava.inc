<?php

	function runCommand($command)
	{
		exec(escapeshellcmd($command), $output, $result);
		return array($output, $result);
	}

	$javapath = Settings::Singleton()->get_setting('cms_root').'java/';
	$path = Settings::singleton()->get_setting('writable');
	$command = 'java -version';
	
	//Execute the command
	$exec = runCommand($command);
	
	if($exec['result'])
	{
		foreach (glob($path.'*.javascript', GLOB_NOSORT) as $filename) 
		{
			if(filemtime($filename.'.cleaned') < filemtime($filename) or !file_exists($filename) )
			{
				$filenameCleaned = $filename.'.cleaned';
				$filenamePacked = $filename.'.packed';
				unset($filenameCleaned); unset($filenamePacked);
				
				$command1 = 'java -jar '.$javapath.'custom_rhino.jar -c '.$filename.' > '.$filename.'.cleaned 2>&1';	
				$command2 = 'java -jar '.$javapath.'yuicompressor-2.3.6.jar'.'--type js -o '.$filename.' '.$filename.'.packed';
			
				runCommand($command1); runCommand($command2);
			
			}			
		}	
	}
		

?>