<?
class cookie
{
	private static $instance;	
	private $key_value;
    private $decypted;
	private $iv;
	private $id;
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
	
	function __construct()
	{
		if(function_exists('mcrypt_get_iv_size'))
		{
			$this->encryption = true;
			$iv_size = mcrypt_get_iv_size(MCRYPT_RIJNDAEL_256, MCRYPT_MODE_ECB);
	   		$this->iv = mcrypt_create_iv($iv_size, MCRYPT_RAND);
		}
		else
		{
			warnings::Singleton()->register('Encryption Failed', 'Your server does not support encryption. Your users cookies are not been encrypted');
		}
		$this->cookie_information = unserialize(stripslashes($_COOKIE[$this->cookie_name]));
		
		if(isset($this->cookie_information['salt']))
		{
			$this->salt = $this->cookie_information['salt'];
		}
	}
	
	function __destruct()
	{
		$this->save();
	}
	
	public function debug()
	{
		echo(' final data ');
		print_r($this->cookie_array);
	}
	
	public function load()
	{
		$this->cookie_array = unserialize($this->decrypt($this->cookie_information['data']));
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
	
	public function get_cookie_value($name)
	{
		if( (is_array($this->cookie_array)) and (is_string($name)) )
		{
			if(isset($this->cookie_array[$name]));
			{
				//return $this->cookie_array[$name];
				$value = $this->cookie_array[$name];
				return $value;
			}
		}
		return NULL;
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
			$this->salt = md5($_SESSION['user_id'].microtime().Settings::Singleton()->get_setting('random_key'));
		}
		return $this->salt;
	}
	
	private function save()
	{
		//Only save the cookie if updated or not set
		if($this->cookie_update)
		{	
			// Build the array
			$data = $this->encrypt(serialize($this->cookie_array));
			$cookie_information_new = serialize(array('salt' => $this->salt(), 'data' => $data));
		
			// save the cookie array
			setcookie($this->cookie_name, $cookie_information_new, time()+$this->cookie_life , Settings::Singleton()->get_setting('document_root'), $_SERVER['SERVER_NAME'], NULL, TRUE);
		}
	}
	
	private function encrypt($decrypted_text, $key = NULL)
	{	
		if($this->encryption)
		{
			if($key == NULL) { $key = $this->key_value(); }
			return mcrypt_encrypt(MCRYPT_RIJNDAEL_256, $key, $decrypted_text, MCRYPT_MODE_ECB, $this->iv);
		}
		
		return $decrypted_text;
	}
	
	private function decrypt($encrypted_text, $key = NULL)
	{	
		if($this->encryption)
		{
			if($key == NULL) { $key = $this->key_value(); }
			return mcrypt_decrypt(MCRYPT_RIJNDAEL_256, $key, $encrypted_text, MCRYPT_MODE_ECB, $this->iv);
		}
		
		return $encrypted_text;
	}
}
?>