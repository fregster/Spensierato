<?php
/**
 * 
 * This is the container class for the master encryption password
 * @author Paul
 *
 */
class masterpassword {
        private static $instance;
        
        private $password;
        private $site_root;

        public static function Singleton(){
           if (!isset(self::$instance)) {
                   $c = __CLASS__;
                   self::$instance = new $c;
           }

           return self::$instance;
        }

        public function __clone(){
           trigger_error('Clone is not allowed.', E_USER_ERROR);
        }

        protected function __construct(){
                $this->site_root = file_root();
        }
        
        /**
         * 
         * Returns the master encryption password
         */
        
        public function get_password(){
                return file_get_contents($this->site_root.DIRECTORY_SEPARATOR.'spensierato_key.pwd');
        }
        
        /**
         * 
         * Set's the master password
         */
        public function set_password($overwrite = false){
        		$filename = $this->site_root.DIRECTORY_SEPARATOR.'spensierato_key.pwd';
      			if($overwrite && file_exists($filename))
        		{
        			unset($filename);
        		}
        		
        		if(file_exists($filename))
        		{
        			return false;
        		}
        		
                return @file_put_contents($filename, Common::generate_random_string(2048, true, true));
        }
        
        public function get_password_secondary(){
                return file_get_contents($this->site_root.DIRECTORY_SEPARATOR.'spensierato_key2.pwd');
        }
        
        /**
         * 
         * Set the secondary password
         * @param unknown_type $overwrite
         */
        public function set_password_secondary($overwrite = false, $password_similarity_check = true){
        		$filename = $this->site_root.DIRECTORY_SEPARATOR.'spensierato_key2.pwd';
        		if($overwrite && file_exists($filename))
        		{
        			unset($filename);
        		}
        		
        		if(file_exists($filename))
        		{
        			return false;
        		}
        		
      			if($password_similarity_check)
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