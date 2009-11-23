<?php
class Lists
{
	private static $instance;

	public static function Singleton()
	{
	   if (!isset(self::$instance))
	   {
		   $c = __CLASS__;
		   self::$instance = new $c;
	   }

	   return self::$instance;
	}

	public function __clone()
	{
	   trigger_error('Clone is not allowed.', E_USER_ERROR);
	}

	protected function __construct()
	{

	}

	public function get_lists_for_user($id)
	{
		return Database::Singleton()->sql_select(array('list_id'), 'tbl_list_membership', array('user_id'), array($id));
	}

	public function get_lists_details($lists)
	{
		if(!is_array($lists))
		{
			$lists = array($lists);
		}
		return Database::Singleton()->sql_select(array('*'), 'tbl_list_data', array('id'), array($lists));
	}
}