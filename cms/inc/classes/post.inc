<?php
/*
 * Use this method to access POST vars
 */
require_once('variables.inc');
class post extends variables
{
	public static function variable($input, $type = NULL)
	{
		if (isset($_POST[$input])) {
			return parent::variable($_POST[$input], $type);
		}
		return FALSE;
	}
}