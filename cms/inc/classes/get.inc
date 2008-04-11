<?php
/*
 * Use this method to access GET vars
 */

class get extends variables
{
	public function check($input, $type = NULL)
	{
		return parent::check($_GET[$input], $type);
	}
}
?>