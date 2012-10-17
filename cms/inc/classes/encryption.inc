<?php
/**
 * 
 * Encryption is a container class for encryption methods
 * All methods here should really just be static methods for performance
 * @author Paul Fryer
 *
 */

class encryption{

	/**
	* 
	* Simple method to test if encryption is available
	*/
	
	static public function supported(){
		if(function_exists('mcrypt_encrypt') && function_exists('mcrypt_decrypt'))
		{
			return true;
		}
		return false;
	}
	
	/**
	* 
	* Default Encryption method
	* All data is double encryped first with Twofish and then with AES using RIJNDAEL 256
	* 
	* Returns encrypted data
	* 
	* @param $input
	*/
        static public function encrypt($input, $keysize = null){
                return encryption::encrypt_aes(encryption::encrypt_twofish($input), $keysize);
        }
		
	static public function encrypt_default($input, $keysize = null){
		return encryption::encrypt($input, $keysize);
	}
        
        /**
         * 
         * Default Decryption method
         * All data is double decrypted first with AES using RIJNDAEL 128 then Twofish
         * 
         * Returns unencrypted data
         * 
         * @param $input
         */
        static public function decrypt($input, $keysize = null){
                return encryption::decrypt_twofish(encryption::decrypt_aes($input, $keysize));
        }
		
	static public function decrypt_default($input, $keysize = null){
                return encryption::decrypt($input, $keysize);
        }
 
	/**
	* 
	* Secure Encryption method
	* All data is triple encryped first with Serpent then Twofish and then with AES using RIJNDAEL 128
	* 
	* Returns encrypted data
	* 
	* @param $input
	*/
        static public function encrypt_secure($input, $keysize = 256){
                return encryption::encrypt_aes(encryption::encrypt_twofish(encryption::encrypt_serpent($input, $keysize), $keysize), $keysize);
        }
        
        /**
         * 
         * Secure Decryption method
         * All data is tripple decrypted first with AES using RIJNDAEL 128 then Twofish then Serpent
         * 
         * Returns unencrypted data
         * 
         * @param $input
         */
        static public function decrypt_secure($input, $keysize = 256){
                return encryption::decrypt_serpent(encryption::decrypt_twofish(encryption::decrypt_aes($input, $keysize), $keysize),$keysize);
        }
		
       /**
	* 
	* Internal method for encrypting with AES
	* 128 blocksize as default with a 256 key is default to be AES complient
        * 
        * Returns AES encrypted data
        * 
	* @param $input
	* @param $keysize array(128, 256)
	* @param $blocksize array(128, 192, 256)
	*/
        static public function encrypt_aes($input, $keysize = null, $blocksize = null){
		$iv = encryption::generate_iv($blocksize);
                return $iv.trim(base64_encode(mcrypt_encrypt(encryption::get_aes_encryption_blocktype($blocksize), encryption::key($keysize), $input, encryption::get_encryption_method(), $iv))); 
        }
        
        /**
         * 
         * Internal method for encrypting with Blowfish
         * 
         * Returns Blowfish encrypted data
         * 
         * @param $input
         */
		 
        static public function encrypt_blowfish($input){
		$iv = encryption::generate_iv(128);
                return trim(base64_encode(mcrypt_encrypt(MCRYPT_BLOWFISH, encryption::key_secondary(), $iv.$input, encryption::get_encryption_method(), substr(md5(encryption::key(),true), 0, 8 )))); 
        }
		
       /**
	* 
	* Internal method for encrypting with Twofish
	* 128 blocksize as default with a 256 key is default
        * 
        * Returns Twofish encrypted data
        * 
        * @param $input
	* @param $keysize array(128, 256)
        */
        static public function encrypt_twofish($input, $keysize = null){
		$iv = encryption::generate_iv(mcrypt_get_iv_size(MCRYPT_TWOFISH, encryption::get_encryption_method()));
               return $iv.trim(base64_encode(mcrypt_encrypt(MCRYPT_TWOFISH, encryption::key($keysize), $input, encryption::get_encryption_method(), $iv))); 
        } 
		
	/**
	 * 
	 * Internal method for encrypting with Serpent
	 * 128 blocksize as default with a 256 key is default
         * 
         * Returns Serpent encrypted data
         * 
         * @param $input
	 * @param $keysize array(128, 256)
         */
        static public function encrypt_serpent($input, $keysize = null){
		$iv = encryption::generate_iv(mcrypt_get_iv_size(MCRYPT_SERPENT, encryption::get_encryption_method()));
                return $iv.trim(base64_encode(mcrypt_encrypt(MCRYPT_SERPENT, encryption::key($keysize), $input, encryption::get_encryption_method(), $iv))); 
        } 
 
        /**
         * 
         * Internal method for decypting AES encypted data
	 * 128 blocksize as default with a 256 key is default to be AES complient
         * 
         * Returns unencrypted data
         * 
         * @param $input
	 * @param $keysize array(128, 256)
	 * @param $blocksize array(128, 192, 256)
         */
        static public function decrypt_aes($input, $keysize = null, $blocksize = null){
		$iv_blocksize = mcrypt_get_iv_size(encryption::get_aes_encryption_blocktype($blocksize), encryption::get_encryption_method());
		$iv = substr($input, 0, $iv_blocksize);
		return trim(mcrypt_decrypt(encryption::get_aes_encryption_blocktype($blocksize), encryption::key($keysize), base64_decode(substr($input,($iv_blocksize))), encryption::get_encryption_method(), $iv)); 
        }
		
	/**
         * 
         * Internal method for decypting Twofish encypted data
	 * 128 blocksize as default with a 256 key is default
         * 
         * Returns unencrypted data
         * 
         * @param $input
	 * @param $keysize array(128, 256)
         */
        static public function decrypt_twofish($input, $keysize = null){
		$iv_blocksize = mcrypt_get_iv_size(MCRYPT_TWOFISH, encryption::get_encryption_method());
		$iv = substr($input, 0, $iv_blocksize);
		return trim(mcrypt_decrypt(MCRYPT_TWOFISH, encryption::key($keysize), base64_decode(substr($input,($iv_blocksize))), encryption::get_encryption_method(), $iv)); 
        }
		
	/**
         * 
         * Internal method for decypting Serpent encypted data
	 * 128 blocksize as default with a 256 key is default
         * 
         * Returns unencrypted data
         * 
         * @param $input
	 * @param $keysize array(128, 256)
         */
        static public function decrypt_serpent($input, $keysize = null){
		$iv_blocksize = mcrypt_get_iv_size(MCRYPT_SERPENT, encryption::get_encryption_method());
		$iv = substr($input, 0, $iv_blocksize);
		return trim(mcrypt_decrypt(MCRYPT_SERPENT, encryption::key($keysize), base64_decode(substr($input,($iv_blocksize))), encryption::get_encryption_method(), $iv)); 
        }
		
	static private function get_aes_encryption_blocktype($blocksize){
		switch ($blocksize) {
			case 256:
				return MCRYPT_RIJNDAEL_256;
				break;
					
			case 192:
				return MCRYPT_RIJNDAEL_192;
				break;
				
			default:
				return MCRYPT_RIJNDAEL_128;
				break;
			}
			return MCRYPT_RIJNDAEL_128;
		}
		
	static private function get_encryption_method(){
		return MCRYPT_MODE_CBC;
	}
        
        /**
         * 
         * Internal method for decrypting Blowfish encrypted data
         * 
         * Returns unencrypted data
         * 
         * @param $input
         */
        static public function decrypt_blowfish($input){
                return trim(substr(mcrypt_decrypt(MCRYPT_BLOWFISH, encryption::key_secondary(),base64_decode($input), encryption::get_encryption_method(), substr(md5(encryption::key(),true), 0, 8 )),16)); 
        }
        
	static private function key_length($length = 256){
		return round($length / 8);
	}
		
        /**
         * 
         * Internal method for getting the main encryption key for AES and Blowfish
         * 
         * Returns a string
         * 
         */
        static private function key($length = 256){
                return substr(masterpassword::Singleton()->get_password(), 0, encryption::key_length($length));
        }
        
        /**
         * 
         * Internal method for getting the main encryption key for Blowfish
         * 
         * Returns a string
         * 
         */
        static private function key_secondary($length = 256){
                return substr(masterpassword::Singleton()->get_password_secondary(), 0, encryption::key_length($length));
        }

	static private function generate_iv($blocksize){
		$length = mcrypt_get_iv_size(encryption::get_aes_encryption_blocktype($blocksize), encryption::get_encryption_method());
		$chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";	

		$size = strlen( $chars );
		$str = '';
		for( $i = 0; $i < $length; $i++ ) {
			$str .= $chars[ rand( 0, $size - 1 ) ];
		}

		return $str;
	}
}
