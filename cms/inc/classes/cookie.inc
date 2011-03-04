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


		if(isset($this->cookie_information['salt']))
		{
			$this->salt = $this->cookie_information['salt'];
		}
		$this->encryption = false;
		$this->load();
	}

	public function __destruct()
	{
		$this->save();
	}

	public function debug()
	{
		echo(' final data ');
		print_r($this->cookie_array);echo('<br/>');
		print_r($this->cookie_information);echo('<br/>');
		print_r($this->decrypt($this->cookie_information['data']));echo('<br/>');
		print_r(unserialize($this->decrypt($this->cookie_information['data'])));echo('<br/>');
	}

	public function load()
	{
		if (isset($this->cookie_information['data'])) {
			$this->cookie_array = unserialize(@$this->decrypt($this->cookie_information['data']));
			if($this->cookie_array === false)
			{
				//The cookie was corupt so delete it
				$this->cookie_update = true;
				$this->save();
			}
		}
		else $this->cookie_array = array();
//		var_dump($this->cookie_information);
//		var_dump($this->cookie_array);
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
		$this->cookie_update = true;
		unset($this->cookie_array[$name]);
	}
	
	public function destroy()
	{
		setcookie($this->cookie_name, '', CMS::$time-42000, Settings::singleton()->get_setting('document_root'));
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
			$this->salt = md5($_SESSION['userid'].microtime().Settings::Singleton()->get_setting('random_key'));
		}
		return $this->salt;
	}

	private function save()
	{
		//Only save the cookie if updated or not set
		if($this->cookie_update)
		{
			// Build the array
			$cookie_information_new = serialize(array('salt' => $this->salt(), 'data' => $this->encrypt(serialize($this->cookie_array))));

			// save the cookie array
			setcookie($this->cookie_name, $cookie_information_new, CMS::$time+$this->cookie_life , Settings::Singleton()->get_setting('document_root'), FALSE, FALSE, TRUE);
			$this->cookie_update = false;
		}
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