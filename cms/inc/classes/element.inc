<?
abstract class Element
{
	public static function type($type)
	{
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