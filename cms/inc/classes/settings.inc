<?php
class Settings
{
	private $settings = array();
	private $db_loaded = false;

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

	protected function __construct()
	{
		//Force some default settings
		$document_root = Common::document_root();
		$this->new_setting('static_host_ajax', $document_root.'/ajax');

		$this->new_setting('static_host_skin', $document_root);

		$this->new_setting('static_host_js', $document_root);
	}

	public function __clone()
	{
	   trigger_error('Clone is not allowed.', E_USER_ERROR);
	}

	//Update the settings with those from the DB
	public function get_database_settings()
	{
		if(!$this->db_loaded)
		{
			$this->db_loaded = true;
			//TODO write if load from db do this else skip
			$settings = Database::Singleton()->sql_select(array('setting', 'value'), 'tbl_settings');

			if(	($this->get_setting('protect_db_settings') != true) and ($this->get_setting('protect_db_settings') != false) )
			{
				die('must be true or false');
			}

			if(is_array($settings))
			{
				foreach($settings as $setting)
				{
					$this->overwrite_setting($setting['setting'], $setting['value'], $this->get_setting('protect_db_settings'));
				}
			}
		}
	}

	public function new_setting($name, $value, $protected = false, $hidden = false)
	{
			if(!isset($this->settings[$name]))
			{
				$this->settings[$name] = array();
			}
			$setting_details = array('protected' => $protected, 'hidden' => $hidden, 'value' => $value);
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
//		$serializer_options = array (
//			   'addDecl' => FALSE,
//			   'encoding' => Settings::Singleton()->get_setting('char_set'),
//			   'indent' => '  ',
//			   'rootName'  => 'settings',
//			   'defaultTagName'  => 'setting'
//			);

		$i = 0;
		$settings = array();
		foreach($this->settings as $setting)
		{
			$key = array_keys($this->settings);
			if($setting[0]['hidden'] != true)
			{
				$settings[$key[$i]] = $setting[0]['value'];
			}
			$i++;
		}
//		return xml::array_to_xml($settings, $serializer_options);
		return xml::array_to_xml_simple($settings, 'settings', 'setting');
	}

	public function overwrite_setting($name, $value, $protected = false)
	{
		//Check if the setting exists. if not, create a new one.
		if (!isset($this->settings[$name])) {
			$this->new_setting($name, $value, $protected);
			return TRUE;
		}
		//Check the old setting is not protected
		else if($this->settings[$name][0]['protected'] != true)
		{
			//Remove old settings
			$hidden = $this->settings[$name][0]['hidden'];
			unset($this->settings[$name]);

			//Push the new setting
			$this->new_setting($name, $value, $protected, $hidden);

			return true;
		}

		return false;
	}
}