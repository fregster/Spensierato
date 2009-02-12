<?php
/*
 * This class is for providing security and cleaning of variables grabbed from the URL (GET) and from POST
 */
abstract class variables
{
	public function variable($input, $type = NULL)
	{	
		if($input == NULL or $input == '')
		{
			return false;
		}
		$type .= '';
		if(function_exists($type))
		{
			return variables::$type($input);
		}

		return variables::default_string($input);
	}
	
	protected function default_string($input)
	{
		$remove = array('<', '>', '"', "'");

		return(str_replace($remove, '', $input));
	}
	
	protected function html($input)
	{
		return Common::html($html);
	}
	
	protected function string($input)
	{
		if(is_string($input))
		{
			return $input;
		}
		
		return false;
	}
	
	protected function sql($input)
	{
		return Common::sql_clean($input);
	}
	
	protected function numeric($input)
	{
		if(is_numeric($input))
		{
			return $input;
		}
		
		return false;
	}
	
	protected function int($input)
	{
		if(is_int($input))
		{
			return $input;
		}
		
		return false;
	}
	
	protected function is_array($input)
	{
		if(isset($input[0]))
		{
			return $input;
		}
		
		return false;
	}
	
	protected function is_set($input)
	{
		print_r($input);
		die();
		if($input != NULL and isset($input))
		{
			return true;
		}
		
		return false;
	}
	
}
?>