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
		 * Default Encryption method
		 * All data is double encryped first with blowfish and then with AES using RIJNDAEL 256
		 * 
		 * Returns encrypted data
		 * 
		 * @param $input
		 */
        static public function encrypt($input){
                return encryption::encrypt_aes(encryption::encypt_blowfish($input));
        }
        
        /**
         * 
         * Default Decryption method
         * All data is double decrypted first with AES using RIJNDAEL 256 then Blowfish
         * 
         * Returns unencrypted data
         * 
         * @param $input
         */
        static public function decrypt($input){
                return encryption::decrypt_blowfish(encryption::decrypt_aes($input));
        }
        
        /**
         * 
         * Internal method for encrypting with AES 256
         * 
         * Returns AES encrypted data
         * 
         * @param $input
         */
        static private function encrypt_aes($input){
                return trim(base64_encode(mcrypt_encrypt(MCRYPT_RIJNDAEL_256, encryption::key(), $input, MCRYPT_MODE_ECB, mcrypt_create_iv(mcrypt_get_iv_size(MCRYPT_RIJNDAEL_256, MCRYPT_MODE_ECB), MCRYPT_RAND)))); 
        }
        
        /**
         * 
         * Internal method for encrypting with Blowfish
         * 
         * Returns Blowfish encrypted data
         * 
         * @param $input
         */
        static private function encypt_blowfish($input){
                return trim(base64_encode(mcrypt_encrypt(MCRYPT_BLOWFISH, encryption::key_secondary(), $input, MCRYPT_MODE_CBC, substr(md5(encryption::key(),true), 0, 8 )))); 
        }
        
        /**
         * 
         * Internal method for decypting AES encypted data
         * 
         * Returns unencrypted data
         * 
         * @param $input
         */
        static private function decrypt_aes($input){
                return trim(mcrypt_decrypt(MCRYPT_RIJNDAEL_256, encryption::key(), base64_decode($input), MCRYPT_MODE_ECB, mcrypt_create_iv(mcrypt_get_iv_size(MCRYPT_RIJNDAEL_256, MCRYPT_MODE_ECB), MCRYPT_RAND))); 
        }
        
        /**
         * 
         * Internal method for decrypting Blowfish encrypted data
         * 
         * Returns unencrypted data
         * 
         * @param $input
         */
        static private function decrypt_blowfish($input){
                return trim(mcrypt_decrypt(MCRYPT_BLOWFISH, encryption::key_secondary(), base64_decode($input), MCRYPT_MODE_CBC, substr(md5(encryption::key(),true), 0, 8 ))); 
        }
        
        /**
         * 
         * Internal method for getting the main encryption key for AES and Blowfish
         * 
         * Returns a string
         * 
         */
        static private function key(){
                return md5(masterpassword::Singleton()->get_password());
        }
        
        /**
         * 
         * Internal method for getting the main encryption key for Blowfish
         * 
         * Returns a string
         * 
         */
        static private function key_secondary(){
                return md5(masterpassword::Singleton()->get_password_secondary());
        }
}