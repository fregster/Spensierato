<?
abstract class Element
{
	public static function type($type)
	{
		if(is_numeric($type))
		{
			$type = Element::get_element_type($type);
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
	
	public static function get_element_type($id)
	{
		$type = Database::Singleton()->sql_select_first(array('type'), 'tbl_elements', array('id'), array($id));
		return $type['type'];
	}
}
?>