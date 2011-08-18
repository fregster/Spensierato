<?php
class cmspage_dashboard extends cmspage
{
	public function initalise()
	{
		//Dashboards are likly to change content quickly and are lightly viewed and so caching can increase load rather than help
		Settings::singleton()->overwrite_setting('cache_xslt_transformation', false);
		return true;
	}
	
	public function type()
	{
		if(isset(CMS::Singleton()->path_vars[1])){
			return (strtolower(CMS::Singleton()->path_vars[1]));
		}
		return false;
	}
	
	public function run()
	{
		if(user::Singleton()->is_admin() && options::Singleton()->get_option('admin_mode'))
		{
			if(cmspage_dashboard::type() && file_exists(DIR_CLASSES.'dashboard'.DS.cmspage_dashboard::type().'.inc')){
				require_once DIR_CLASSES.'dashboard'.DS.cmspage_dashboard::type().'.inc';
			}
			else {
				foreach(glob(DIR_CLASSES.'dashboard'.DS.'*.inc', GLOB_NOSORT) as $filename) 
				{
					include_once($filename);
				}	
			}
			
			//var_dump(Settings::singleton()->get_setting('dashboards')); die();
			
			Render::Singleton()->render_dashboard(Content::Singleton()->build());
			return true;
		}
		error::do403();
	}
	
	public static function register($long_name, $short_name, $class, $admin = false, $icon = null, $edit_icon = null, $html = null, $selected = false)
	{
		$types = Settings::singleton()->get_setting('dashboards');
		if(!is_array($types))
		{
			$types = array();
		}

		if(!$admin or options::Singleton()->get_option('admin_mode'))
		{
			if(is_null($icon)) { $icon = 'page_white'; }
			if(is_null($edit_icon)) { $edit_icon = $icon; }
			array_push($types, array('name' => $long_name, 'link' => $short_name, 'class' => $class, 'icon' => $icon, 'edit_icon' => $edit_icon, 'html' => $html, 'selected' => $selected));
		}
		Settings::singleton()->overwrite_setting('dashboards', $types, false);
	}
}