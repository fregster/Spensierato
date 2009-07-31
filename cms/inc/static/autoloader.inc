<?php
$class_profile_use_true_memory = false;
$class_profile = array(array('class' => 'inital code', 'mem_diff' => (memory_get_usage($class_profile_use_true_memory))));

/**
 * Auto class loading function
 * $class_block_list are for elements or classes that are not ready for production
 * Uncoment them for testing
 *
 * @param string $class_name
 */

function __autoload($class_name)
{
	global $class_block_list;
	global $include_path;
	global $class_profile;
	global $class_profile_use_true_memory;
	$trace = false;
	$paths = array();

	if($class_name != '' and $class_name != null and !class_exists($class_name) and !isset($class_block_list[$class_name])) //Ensure we should be trying to load the class
	{
		$class_name = strtolower($class_name);
		$class_filename = $class_name;
		$exploded_class = explode('_', $class_name);
		if(isset($exploded_class[1])) //The class is in a folder 
		{ 
			$folder_count = count($exploded_class) -1;
			$temp_path = $include_path.'/cms/inc/classes/';
			$temp_path_ucfirst = $include_path.'/cms/inc/external/'; //For Zend plugins etc
			$class_filename = $exploded_class[($folder_count)];
			array_pop($exploded_class);
			foreach($exploded_class as $folder)
			{
				$temp_path .= $folder . '/'; //Spensieratos default filesystem in always lowercase
				$temp_path_ucfirst .= ucfirst($folder) . '/'; //Useful for Zend plugins
			}
			$paths[] = $temp_path;
			$paths[] = $temp_path_ucfirst;
			
			unset($temp_path);
			unset($temp_path_ucfirst);
			unset($folder_count);
		 }
		 unset($exploded_class);
		
		$paths[] =	$include_path.'/cms/inc/classes/';
		$paths[] =	$include_path.'/cms/inc/classes/optional/';
					
		foreach($paths as $path) //Cycle through the includes dir in order of preference
		{
			$file_ext = array('inc', 'php');  //Spensierato Default ext is inc as apache and iis can be configured not to serve them to help with security, Zend uses PHP
			foreach($file_ext as $ext)
			{
				if(file_exists($path.$class_filename.'.'.$ext)) //Check the file exists
				{
					if($trace) { $start_time = explode(" ",microtime()); $start_time = $start_time[1] + $start_time[0]; $start_mem = memory_get_usage($class_profile_use_true_memory); }
						require_once $path.$class_filename.'.'.$ext;
					if($trace) { $mtime = explode(" ",microtime()); $mtime = $mtime[1] + $mtime[0]; $class_profile[] = array('class' => $class_name, 'mem_diff' => (memory_get_usage($class_profile_use_true_memory) - $start_mem) / 1024, 'load_time' => $mtime - $start_time); }
						return true;
				}
				else if(file_exists($path.ucfirst($class_filename).'.'.$ext)) //Really for Zend stuff
				{
					if($trace) { $start_time = explode(" ",microtime()); $start_time = $start_time[1] + $start_time[0]; $start_mem = memory_get_usage($class_profile_use_true_memory); }
						require_once $path.ucfirst($class_filename).'.'.$ext;
					if($trace) { $mtime = explode(" ",microtime()); $mtime = $mtime[1] + $mtime[0]; $class_profile[] = array('class' => $class_name, 'mem_diff' => (memory_get_usage($class_profile_use_true_memory) - $start_mem) / 1024, 'load_time' => $mtime - $start_time); }
						return true;
				}
			}
		}
		return false; //Comment this out for debuging
		print_r($paths);
		echo($class_filename);
		die('<p>Critical error: Class "'.$class_name.'" not defined</p>'); //A class has been called the can not be loaded
	}
	return false; //Should never really get here
}

function get_extended_class_path($base_class, $new_class)
{
	global $include_path;
	
	$base_len = strlen($base_class)+1;
	$class_filename = $new_class;
	if(substr($new_class, 0, $base_len) == ($base_class.'_'))
	{
		$class_filename = substr($new_class, $base_len);
	}

	return($include_path.'/cms/inc/classes/'.$base_class.'/'.$class_filename.'.inc');
}


$extra_include_paths = array($include_path . '/cms/inc/external/');

foreach ($extra_include_paths AS $path)
{
	if (!file_exists($path) OR (file_exists($path) && filetype($path) !== 'dir'))
	{
		trigger_error("Include path '{$path}' not exists", E_USER_WARNING);
		continue;
	}
       
	$paths = explode(PATH_SEPARATOR, get_include_path());
		
	if (array_search($path, $paths) === false)
		array_push($paths, $path);
       
	set_include_path(implode(PATH_SEPARATOR, $paths));
}
unset($extra_include_paths);

?>