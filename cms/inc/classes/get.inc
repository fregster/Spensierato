<?php
/*
 * Use this method to access GET vars
 */
require_once('variables.inc');
class get extends variables
{
	public static function variable($input, $type = NULL)
	{
		if (isset($_GET[$input])) {
			return parent::variable($_GET[$input], $type);
		}
		return FALSE;
	}
}