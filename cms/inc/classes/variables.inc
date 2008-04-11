<?php
/*
 * This class is for providing security and cleaning of variables grabbed from the URL (GET) and from POST
 */
abstract class variables
{
	public function check($input, $type = NULL)
	{
		if($type == NULL)
		{
			return $input;
		}
		
		variables::$type($input);
	}
	
	
	protected function string($input)
	{
		if(is_string($input))
		{
			return $input;
		}
		
		return false;
	}
	
	protected function numeric($input)
	{
		if(is_numeric($input))
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
	
}
?>