<?php
/**
 * Define the setting groups
 * This is just for easy editing
 */

class Settings
{
	private $settings = array();
	private $db_loaded = false;
	private $define_value_map = array();

	private static $instance;

	public static function singleton()
	{
	   if (!isset(self::$instance))
	   {
		   $c = __CLASS__;
		   self::$instance = new $c;
	   }

	   return self::$instance;
	}

	protected function __construct($max_sha_hash = 'sha256')
	{
		//Backwards compatability
		if(!isset($GLOBALS['max_sha_hash']))
		{
			$GLOBALS['max_sha_hash'] = 'sha256';
		}
		
		//Force some default settings
		$document_root = Common::document_root();
		$this->new_setting('static_host_ajax', $document_root.'/ajax');

		$this->new_setting('static_host_skin', $document_root.'/');

		$this->new_setting('static_host_js', $document_root.'/');
		
		$this->new_setting('allow_db_settings', true);
	}
	
	public function build_defines($defines){
		if(is_array($defines))
		{
			foreach($defines as $key => $value)
			{
				define($key, $value);
				$this->define_value_map[$value] = $key;
			}
		}
	}

	public function __clone()
	{
	   trigger_error('Clone is not allowed.', E_USER_ERROR);
	}

	//Update the settings with those from the DB
	public function get_database_settings($force_reload = false)
	{
		if(!$this->db_loaded || $force_reload)
		{
			$this->db_loaded = true;
			if(isset($this->settings['allow_db_settings'][0]) && $this->settings['allow_db_settings'][0]){
				$settings = Database::Singleton()->sql_select(array('setting', 'value'), 'tbl_settings');
	
				if(	($this->get_setting('protect_db_settings') != true) and ($this->get_setting('protect_db_settings') != false) )
				{
					die('must be true or false');
				}
	
				if(is_array($settings))
				{
					foreach($settings as $setting)
					{
						$setting['value'] = @unserialize($setting['value']);
						if ($setting['value'] === 'false') { $setting['value'] = false; }
						if ($setting['value'] === 'true') { $setting['value'] = true; }
	
						$this->overwrite_setting($setting['setting'], $setting['value'], $this->get_setting('protect_db_settings'), null, true);
					}
				}
			}
		}
	}

	public function new_setting($name, $value, $protected = false, $hidden = false, $editable = false, $edit_group = null, $loaded_from_db = false)
	{
			if(!isset($this->settings[$name]))
			{
				$this->settings[$name] = array();
			}
			$setting_details = array('protected' => $protected, 'hidden' => $hidden, 'value' => $value, 'editable' => $editable, 'edit_group' => $edit_group, 'db_setting' => $loaded_from_db);
			array_push($this->settings[$name], $setting_details);
	}

	public function get_setting($name)
	{
		if(!$this->db_loaded)
		{
			$this->get_database_settings();
		}
		if(isset($this->settings[$name][0]['value']))
		{
			return $this->settings[$name][0]['value'];
		}

		return false;
	}

	public function settings_as_xml()
	{
		return xml::array_to_xml_simple($this->get_all_settings(), 'settings', 'setting');
	}
	
	public function get_all_settings($editable = false, $map_names = false)
	{
		$i = 0;
		$settings = array();
		
		/**
		 * Split for performance, increased code size but reduces complex if's majority of the time
		 */
		if(!$editable)
		{
			foreach($this->settings as $setting)
			{
				$key = array_keys($this->settings);
				if($setting[0]['hidden'] != true)
				{
					$settings[$key[$i]] = $setting[0]['value'];	
				}
				$i++;
			}
			return $settings;
		}
		else
		{
			foreach($this->settings as $setting)
			{
				$key = array_keys($this->settings);
				if($setting[0]['editable'] == true)
				{
					if(!isset($setting[0]['edit_group'])){ $setting[0]['edit_group'] = 99999; }
					if(!isset($settings[$setting[0]['edit_group']]))
					{
						$settings[$setting[0]['edit_group']] = array();
					}		
					
					$settings[$setting[0]['edit_group']][$key[$i]] = array('name' => $key[$i], 'value' => $setting[0]['value'], 'options' => $setting[0]['editable']);
					if($setting[0]['protected'] && !$setting[0]['db_setting'])
					{
						$settings[$setting[0]['edit_group']][$key[$i]]['protected'] = true;
					}
				}
				$i++;
			}
			ksort($settings);
			
			if($map_names){
				foreach($settings as $k => $v)
				{
					if(isset($this->define_value_map[$k]))
					{
						$settings[$this->define_value_map[$k]] = $settings[$k];
				  		unset($settings[$k]);
					}
					else
					{
						$settings['Untited_Group_'.$k] = $settings[$k];
				  		unset($settings[$k]);
					}
				}
			}
			
			return $settings;
		}
	}
	
	public function update_database_setting($name, $value)
	{
		if ($value == 'false') { $value = false; }
		if ($value == 'true') { $value = true; }
		
		if($this->get_setting($name) != $value)
		{
//			Database::Singleton()->sql_update_or_insert('tbl_settings', array('setting'), array($key), array($key, $new_value), true, 'setting');
			Database::Singleton()->sql_delete_where('tbl_settings', 'setting', $name);	
			Database::singleton()->sql_insert('tbl_settings', array('setting', 'value'), array($name, serialize(utf8_encode($value))));
			return true;
		}
	}

	public function overwrite_setting($name, $value, $protected = false, $editable = null, $loaded_from_db = false)
	{
		//Check if the setting exists. if not, create a new one.
		if (!isset($this->settings[$name])) {
			$this->new_setting($name, $value, $protected, $editable, null, null, $loaded_from_db);
			return TRUE;
		}
		//Check the old setting is not protected
		else if($this->settings[$name][0]['protected'] != true)
		{
			//Remove old settings
			$old_details = $this->settings[$name][0];
			if(is_null($editable)){ $editable = $old_details['editable']; }
			unset($this->settings[$name]);

			//Push the new setting
			$this->new_setting($name, $value, $protected, $old_details['hidden'], $editable, $old_details['edit_group'], $loaded_from_db);

			return true;
		}

		return false;
	}
}