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
		if(isset($exploded_class[1])) 
		{ 
			$paths[] = $include_path.'/cms/inc/classes/'.$exploded_class[0].'/';
			$class_filename = $exploded_class[1];
		 }
		
		$paths[] =	$include_path.'/cms/inc/classes/';
		$paths[] =	$include_path.'/cms/inc/classes/elements/';
		$paths[] =	$include_path.'/cms/inc/classes/optional/';
					
		foreach($paths as $path) //Cycle through the includes dir in order of preference
		{
			if(file_exists($path.$class_filename.'.inc')) //Check the file exists
			{
				if($trace) { $start_time = explode(" ",microtime()); $start_time = $start_time[1] + $start_time[0]; $start_mem = memory_get_usage($class_profile_use_true_memory); }
					require_once $path.$class_filename.'.inc';
				if($trace) { $mtime = explode(" ",microtime()); $mtime = $mtime[1] + $mtime[0]; $class_profile[] = array('class' => $class_name, 'mem_diff' => (memory_get_usage($class_profile_use_true_memory) - $start_mem) / 1024, 'load_time' => $mtime - $start_time); }
					return true;
			}
		}
		
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
?>