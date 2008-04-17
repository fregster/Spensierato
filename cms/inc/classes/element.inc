<?
abstract class Element
{
	public static function type($type)
	{
		if(is_int($type))
		{
			$type = Database::Singleton()->sql_select_first(array('	type'), 'tbl_page_elements', array('id'), array($type));
		}
		
		if(is_string($type))
		{
			if(class_exists($type))
			{
				$c = $type;
			}
			else
			{
				$c = elementDefault;
			}
		}
		
		return new $c;
	}
}
?>