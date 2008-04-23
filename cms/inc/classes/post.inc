<?php
/*
 * Use this method to access POST vars
 */
require_once('variables.inc');
class post extends variables
{
	public function variable($input, $type = NULL)
	{
		return parent::$_POST[$input]($type);
	}
}
?>