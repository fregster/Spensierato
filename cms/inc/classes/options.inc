<?php
/*
Where all the session options are stored
*/
class options
{
	private static $instance;
	/*
		Valid options is an array of options with an array of valid selections for that option the first value in the array is the default
	*/
	private $valid_options = array(
					'admin_mode' => array(false, true),
					'test' => array(true),
					'force_reload' => array(false, true),
					'display_warnings' =>array(true, false)
							);

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
	
	function __construct()
	{
		//The options has not been started this session
		if(!isset($_SESSION['cms_options']))
		{
			$_SESSION['cms_options'] = array();
			$this->set_default_options();
			
			//Load saved options
			$this->load_saved_options();
		}
	}
	
	public function admin_check()
	{
		//Admin mode hack
		if($this->get_option('admin_mode'))
		{
			if(!Security::Singleton()->can_edit())
			{
				$this->set_option('admin_mode', false);
			}
		}
	}
	
	public function load_saved_options()
	{
		foreach($this->valid_options as $option)
		{
			if(cookie::Singleton()->get_cookie_value($option))
			{
				$_SESSION['cms_options'][$option] = cookie::Singleton()->get_cookie_value($option);
			}
		}
	}
	
	public function options_as_xml()
	{
//		$serializer_options = array ( 
//			   'addDecl' => FALSE, 
//			   'encoding' => Settings::Singleton()->get_setting('char_set'), 
//			   'indent' => '  ',
//			   'rootName'  => 'options',
//			   'defaultTagName'  => 'option'
//			);
		return Common::array_to_xml_simple($_SESSION['cms_options'], 'options', 'option');
	}
	
	public function set_option($option, $value)
	{
		if(isset($this->valid_options[$option][$value]))
		{
			$_SESSION['cms_options'][$option] = $value;
		}
	}
	
	public function get_option($option)
	{
		if(isset($_SESSION['cms_options'][$option]))
		{
			return $_SESSION['cms_options'][$option];
		}
		
		return NULL;
	}
	
	public function toggle_option($option)
	{
		//Find the position of the current option
		$key = array_search($this->get_option($option), $this->valid_options[$option]);
		
		// -1 as arrays start at 0 and count starts at 1
		if($key == count($this->valid_options[$option])-1)
		{
			//this is the last value in the array so use the default one
			$this->set_option($option, $this->valid_options[$option][0]);
		}
		else
		{
			//There are further values in the array so move up the array
			$this->set_option($option, $this->valid_options[$option][$key +1]);#
		}
	}
	
	private function set_default_options()
	{	
		foreach($this->valid_options as $name => $value)
		{
			$this->set_option($name, $value[0]);
		}
	}
}
?>