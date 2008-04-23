<?
abstract class Element
{
	public static function type($type)
	{
		if(is_numeric($type))
		{
			$type = Database::Singleton()->sql_select_first(array('type'), 'tbl_elements', array('id'), array($type));
		}
		
		if(class_exists($type))
		{
			$c = $type;
		}
		else
		{
			$c = elementDefault;
		}
		
		return new $c;
	}
}
?>