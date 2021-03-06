<?php
class cookie
{
	private static $instance;
    private $decypted;
	private $cookie_array = array();
	private $cookie_update;
	private $salt;
	private $encryption = false;

	private $cookie_life = 2678400; //31 days
	private $cookie_name = 'CMS';

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

	protected function __construct()
	{
		$this->cookie_name = sha1(($this->cookie_name.Settings::singleton()->get_setting('random_string')));
		
		if(settings::singleton()->get_setting('cookie_encryption') && encryption::supported())
		{
			$this->encryption = true;
		}
		else if(settings::singleton()->get_setting('cookie_encryption'))
		{
			warnings::Singleton()->register('Encryption Failed', 'Your server does not support encryption. Your users cookies are not been encrypted');
		}

		if (isset($_COOKIE[$this->cookie_name])) {
			$this->cookie_information = @unserialize(stripslashes($_COOKIE[$this->cookie_name]));
		}
		else $this->cookie_information = FALSE;

//		$this->encryption = false;
		$this->load();
		if(isset($this->cookie_array['data']['salt']))
		{
			$this->salt = $this->cookie_array['data']['salt'];
		}
	}

	public function __destruct()
	{
		$this->save();
	}

	public function debug()
	{
		echo('<h2>'. $this->cookie_name .'</h2> final data <br/>');
		echo('Array '); print_r($this->cookie_array);echo('<br/>');
		echo('Information '); print_r($this->cookie_information);echo('<br/>');
		print_r('Data ' . $this->decrypt($this->cookie_information['data']));echo('<br/>');
		echo('Data '); var_dump(unserialize($this->decrypt($this->cookie_information['data'])));echo('<br/>');
	}

	public function load()
	{
		if (isset($this->cookie_information['data'])) {
			$this->cookie_array = @unserialize($this->decrypt($this->cookie_information['data']));
			
			if($this->cookie_array == false)
			{
				//The cookie was corupt so delete it
//				$this->cookie_update = true;
//				$this->save();
			}
		}
		else {
			$this->cookie_array = array();
		}
	}

	public function set_cookie($name, $value)
	{
		// add the value to the array
		$this->cookie_update = true;
		$this->cookie_array[$name]= $value;
	}

	public function remove_cookie($name)
	{
		//remove a value from the array
		if(isset($this->cookie_array[$name])){
			$this->cookie_update = true;
			unset($this->cookie_array[$name]);
		}
	}
	
	public function destroy()
	{
		return $this->save(true);
	}
	
	public function is_cookie_value_set($name)
	{
		return isset($this->cookie_array[$name]);
	}

	public function get_cookie_value($name)
	{
		$value = null;
		if( (is_array($this->cookie_array)) and (is_string($name)) )
		{
			if(isset($this->cookie_array[$name]))
			{
				$value = $this->cookie_array[$name];
			}
		}
		return $value;
	}

	private function key_value()
	{
		$key_value = md5(Settings::singleton()->get_setting('random_key').$this->salt());

		return $key_value;
	}

	private function salt()
	{
		if(!isset($this->salt))
		{
			$uid = 0;
			if(isset($_SESSION['userid'])){ $uid = $_SESSION['userid']; }
			$this->salt = sha1(CMS::$time.$uid.microtime().Settings::Singleton()->get_setting('random_key'));
		}
		return $this->salt;
	}

	private function save($force = false)
	{
		//Only save the cookie if updated or not set
		if($this->cookie_update || $force)
		{
			// save the cookie array
			$this->cookie_update = false;
			$path = DOCUMENT_PATH;
			if(substr($path, -1) != '/'){
				$path .= '/';
			}
			$this->cookie_array['time'] = CMS::$time;
			$this->cookie_array['salt'] = $this->salt();
			$this->cookie_array['path'] = $path;
			return setcookie($this->cookie_name, serialize(array('data' => $this->encrypt(serialize($this->cookie_array)))), CMS::$time+$this->cookie_life , $path, FALSE, FALSE, TRUE);
		}
		return false;
	}

	private function encrypt($decrypted_text, $key = NULL)
	{
		if($this->encryption)
		{
			return encryption::encrypt($decrypted_text);
		}

		return $decrypted_text;
	}

	private function decrypt($encrypted_text, $key = NULL)
	{
		if($this->encryption)
		{
			return encryption::decrypt($encrypted_text);
		}

		return $encrypted_text;
	}
}