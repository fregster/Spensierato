<?php
/*
 * Use this method to access POST vars
 */

class post extends variables
{
	public function variable($input, $type = NULL)
	{
		return parent::check($_POST[$input], $type);
	}
}
?>