<?php
/**
 * 
 * This is the container class for the master encryption password
 * The class is maintaned seperatly for object size resons
 * @author Paul Fryer
 *
 */

class masterpassword {
        private static $instance;
        
        private $options = array('path' => null, 'key1' => 'spensierato_key1.pwd', 'key2' => 'spensierato_key2.pwd');
        
        private $password_primary = false;
        private $password_seconday = false;

        /**
         * 
         * Master password singleton method with optional options array to pass through to the constructor
         * Returns the standard object
         * 
         * @param array $options
         */
        public static function Singleton($options = array()){
           if (!isset(self::$instance)) {
                   $c = __CLASS__;
                   self::$instance = new $c($options);
           }

           return self::$instance;
        }

        public function __clone(){
           trigger_error('Clone is not allowed.', E_USER_ERROR);
        }

        /**
         * 
         * Basic construct method
         * Options paramater sets the contaniner path and / or key file names
         * @param $options
         */
		protected function __construct($options = array()){
				$this->options = array_merge($this->options, $options);
        	if(is_null($this->options['path'])) { echo 'hello'; $this->options['path'] = DIR_CONFIG; }
        }
        
        /**
         * 
         * Returns the master encryption password
         */
        
        public function get_password(){
        	if(!$this->password_primary){
        		$this->password_primary = hash('sha256', @file_get_contents($this->options['path'].$this->options['key1']));
        	}
        	return $this->password_primary;
        }
        
        /**
         * 
         * Set's the master password
         */
        public function set_password($overwrite = false){
        		$filename = $this->options['path'].$this->options['key1'];
      			if($overwrite && file_exists($filename))
        		{
        			unlink($filename);
        		}
        		
        		if(isset($filename) && file_exists($filename))
        		{
        			return false;
        		}
        		
                return @file_put_contents($filename, Common::generate_random_string(2048, true, true));
        }
        
        /**
         * 
         * Returns the secondary password
         */
        public function get_password_secondary(){
        	if(!$this->password_seconday){
        		$this->password_seconday = hash('sha256', @file_get_contents($this->options['path'].$this->options['key2']));
        	}
        	return $this->password_seconday;
        }
        
        /**
         * 
         * Set the secondary password
         * @param unknown_type $overwrite
         */
        public function set_password_secondary($overwrite = false, $password_similarity_check = true){
        		$filename = $this->options['path'].$this->options['key2'];
        		if($overwrite && file_exists($filename))
        		{
        			unlink($filename);
        		}
        		
        		if(isset($filename) && file_exists($filename))
        		{
        			return false;
        		}
        		
      			if($password_similarity_check && class_exists('StringMatch'))
      			{
	                $password = Common::generate_random_string(2048, true, true);
	                $comparison = new StringMatch;
	                $result = round(($comparison->fstrcmp ($password, strlen($password), $this->get_password(), strlen($this->get_password()), 0.4)),2);
	                
	                if($result > 75){
//	                    die('Passwords are to similar strings match around: '.$result.'%');
						return masterpassword::set_password_secondary();
	                }
	       		}
                return @file_put_contents($filename, $password);
        }
}