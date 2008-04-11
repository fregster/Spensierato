<?php
/*
 * Use this method to access GET vars
 */
require_once('variables.inc');
class get extends variables
{
	public function variable($input, $type = NULL)
	{
		return parent::check($_GET[$input], $type);
	}
}
?>