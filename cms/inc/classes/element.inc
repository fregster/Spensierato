<?php
abstract class Element
{
	public static function type($type)
	{
		if(is_numeric($type))
		{
			$type = Element::get_element_type($type);
		}
		$prefix = 'element_';
		if(substr($type, 0, 8) != $prefix)
		{
			if(substr($type, 0, 7) == 'element')
			{
				$type = $prefix.substr($type, 7);
			}
			else
			{
				$type = $prefix.$type;
			}
		}

		if(class_exists($type))
		{
			$c = $type;
		}
		else
		{
			$c = 'element_Default';
		}

		return new $c;
	}

	public static function get_element_type($id)
	{
		$type = Database::Singleton()->sql_select_first(array('type'), 'tbl_elements', array('id'), array($id));
		return $type['type'];
	}

	public static function register($name, $class, $admin = false, $icon = null, $edit_icon = null)
	{
		$types = Settings::singleton()->get_setting('elementTypes');
		if(!is_array($types))
		{
			$types = array();
		}

		if(!$admin or options::Singleton()->get_option('admin_mode'))
		{
			if(is_null($icon)) { $icon = 'page_white'; }
			if(is_null($edit_icon)) { $edit_icon = $icon; }
			array_push($types, array('name' => $name, 'class' => $class, 'icon' => $icon, 'edit_icon' => $edit_icon));
		}
		Settings::singleton()->overwrite_setting('elementTypes', $types);
	}
}