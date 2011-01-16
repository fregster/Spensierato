<?php
class Register
{
	private $hash;
	private $id;
	private $username;
	private $email;
	private $salt;

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

	protected function __construct(){

	}

	public function __clone()
	{
	   trigger_error('Clone is not allowed.', E_USER_ERROR);
	}

	public function fill_in()
	{
		$element = Element::type('elementRegistration');
		$element->build('Registration');
		$this->get_html();
	}

	public static function username_check($input)
	{
		$username = database::Singleton()->sql_select_first('username', 'tbl_users', 'username', strtolower($input));
		if(isset($username['username']))
		{
			return true;
		}

		return false;
	}

	/**
	 * this is the initial registraion regiester
	 *
	 * The method builds a registration string to be emailed to the user
	 * The string contatins a link back to the site with an id and a hash in the URL
	 * The id is the temp data id
	 * The hash is the md5+sha1 hash of the server_host + random_key + timestamp + username + email address + password
	 */

	public function new_registration()
	{
		if(capthca::Code_check(post::variable('security_code','is_set')))
		{
			//build post data
			$this->username = post::variable('username','string');
			$this->email =  post::variable('email','string');
	
			//Generate a random password this makes the hash even more random
			$this->salt = common::generate_random_string(16);
	
			//TODO use POST class!
			$string = $_SERVER['SERVER_NAME'].Settings::singleton()->get_setting('random_key').CMS::time().$this->username.$this->email.$this->salt;
			$this->hash = md5($string).sha1($string);
			
			$this->save_registration();
			return $this->send_registration();
		}

		return false;
	}

	public function verify_register()
	{
		$id = get::variable('id','int');
		$temp_details = database::Singleton()->sql_select_first(array('data', 'ts'), 'tbl_temp', array('id'), array($id));

		if(!isset($temp_details['data'][1]))
		{
			return false;
		}

		//TODO write setting var
		if($temp_details['ts'] < (CMS::time()-3000000))
		{
			return false;
		}

		$data = xml::xml_to_array($temp_details['data']);

		//Compare hashes
		if($data['hash'] == get::variable('key','string'))
		{
			database::Singleton()->force_write_user();
			database::Singleton()->sql_delete_by_colum_equals('tbl_temp', 'id', $id);
			database::Singleton()->destroy_write_user();
			user::Singleton()->create($data['username'], $data['email'], true);
			die();
		}
		return false;
	}

	private function save_registration()
	{
		database::Singleton()->force_write_user();
		$data = xml::array_to_xml(array('hash' => $this->hash, 'username' => $this->username, 'email' => $this->email, 'salt' => $this->salt));
		$this->id = database::Singleton()->sql_insert('tbl_temp', array('data', 'ts'), array($data, time()));
		database::Singleton()->destroy_write_user();
	}

	private function send_registration()
	{
		$link = 'http://'.$_SERVER['SERVER_NAME'].Settings::singleton()->get_setting('document_root').'/register?id='.$this->id.'&key='.$this->hash;
		$to =  post::variable('username','string').'<'. post::variable('email','string').'>';
		$from = Settings::singleton()->get_setting('site_name_long').' <no-reply@'.str_replace('www.', '', $_SERVER['SERVER_NAME']).'>';

		$subject = 'Registration at '.settings::singleton()->get_setting('site_name_long');
		$message = '<p>Thank you for registering please follow the link to confirm your registration<br />If you did not register at then please ignore this email</p> <a href="'.$link.'">'.$link.'</a>';
		return common::email($to, $from, $subject, $message, true);
	}
}