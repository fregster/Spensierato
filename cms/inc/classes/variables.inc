<?php
/*
 * This class is for providing security and cleaning of variables grabbed from the URL (GET) and from POST
 */
abstract class variables
{
	public static function variable($input, $type = NULL)
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

	protected static function default_string($input)
	{
		$remove = array('<', '>', '"', "'");

		return(str_replace($remove, '', $input));
	}

	protected static function html($input)
	{
		return Common::html($html);
	}

	protected static function string($input)
	{
		if(is_string($input))
		{
			return $input;
		}

		return false;
	}

	protected static function sql($input)
	{
		return Common::sql_clean($input);
	}

	protected static function numeric($input)
	{
		if(is_numeric($input))
		{
			return $input;
		}

		return false;
	}

	protected static function int($input)
	{
		if(is_int($input))
		{
			return $input;
		}

		return false;
	}

	protected static function is_array($input)
	{
		if(isset($input[0]))
		{
			return $input;
		}

		return false;
	}

	protected static function is_set($input)
	{
		if($input != NULL and isset($input))
		{
			return true;
		}

		return false;
	}
	
	protected static function path($input)
	{
		return $input;
	}

}