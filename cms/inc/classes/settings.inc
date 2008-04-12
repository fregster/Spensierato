<?
class Settings
{
	private $settings = array();
	
	protected static $instance;

	public static function singleton()
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
	
	public function get_database_settings()
	{
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

	public function new_setting($name, $value, $protected = false)
	{
			if(!isset($this->settings[$name]))
			{
				$this->settings[$name] = array();
			}
			$setting_details = array('protected' => $protected, 'value' => $value);
			array_push($this->settings[$name], $setting_details);
	}
	
	public function get_setting($name)
	{
		if(isset($this->settings[$name]))
		{
			return $this->settings[$name][0]['value'];
		}
		
		return NULL;
	}
	
	public function settings_as_xml()
	{
		$serializer_options = array ( 
			   'addDecl' => FALSE, 
			   'encoding' => Settings::Singleton()->get_setting('char_set'), 
			   'indent' => '  ',
			   'rootName'  => 'settings',
			   'defaultTagName'  => 'setting'
			);
		
		$i = 0;
		$settings = array();
		foreach($this->settings as $setting)
		{
			$key = array_keys($this->settings);
			$settings[$key[$i]] = $setting[0]['value'];
			$i++;
		}
		return Common::array_to_xml($settings, $serializer_options);
	}
	
	public function overwrite_setting($name, $value, $protected = false)
	{
		//Check the old setting is not protected
		if($this->settings[$name][0]['protected'] != true)
		{
			//Remove old settings
			unset($this->settings[$name]);
			
			//Push the new setting
			die($name);
			$this->new_setting($name, $value, $protected);
			
			return true;
		}

		return false;
	}
}
?>