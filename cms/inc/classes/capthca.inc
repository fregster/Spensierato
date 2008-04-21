<?
class Capthca
{ 
/*
* File: CaptchaSecurityImages.php
* Author: Simon Jarvis
* Copyright: 2006 Simon Jarvis
* Date: 03/08/06
* Updated: 07/02/07
* Requirements: PHP 4/5 with GD and FreeType libraries
* Link: http://www.white-hat-web-design.co.uk/articles/php-captcha.php
* 
* This program is free software; you can redistribute it and/or 
* modify it under the terms of the GNU General Public License 
* as published by the Free Software Foundation; either version 2 
* of the License, or (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful, 
* but WITHOUT ANY WARRANTY; without even the implied warranty of 
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
* GNU General Public License for more details: 
* http://www.gnu.org/licenses/gpl.html
*
*/
 
   static function create_Images($width='120',$height='40') 
   {
   		$characters = Settings::Singleton()->get_setting('login_capthca_length');
		if(($characters >= 8) or (!is_numeric($characters)) )
		{
			$characters = 8;
		}
		$code = Common::generate_random_string($characters);
		$_SESSION['capthca_security_code'] = $code;
		
		/* font size will be 75% of the image height */
		$font = Settings::Singleton()->get_setting('cms_root').'/inc/fonts/monofont.ttf';
		$font_size = $height * 0.75;
		$image = imagecreate($width, $height) or die('Cannot initialize new GD image stream');
		
		/* set the colours */
		$background_color = imagecolorallocate($image, 255, 255, 255);
		$text_color = imagecolorallocate($image, 20, 40, 100);
		$noise_color = imagecolorallocate($image, 100, 120, 180);
		
		/* generate random dots in background */
		for( $i=0; $i<($width*$height)/3; $i++ ) 
		{
		 imagefilledellipse($image, mt_rand(0,$width), mt_rand(0,$height), 1, 1, $noise_color);
		}
		
		/* generate random lines in background */
		for( $i=0; $i<($width*$height)/150; $i++ ) 
		{
		 imageline($image, mt_rand(0,$width), mt_rand(0,$height), mt_rand(0,$width), mt_rand(0,$height), $noise_color);
		}
		/* create textbox and add text */
		$textbox = imagettfbbox($font_size, 0, $font, $code) or die('Error in imagettfbbox function');
		$x = ($width - $textbox[4])/2;
		$y = ($height - $textbox[5])/2;
		
		imagettftext($image, $font_size, 0, $x, $y, $text_color, $font , $code) or die('Error in imagettftext function');
		/* output captcha image to browser */
		header('Content-Type: image/jpeg');
		Cache::headers_no_cache();
		imagejpeg($image);
		imagedestroy($image);
   }
   
   static function Code_check($security_code)
   {
   		if(Settings::Singleton()->get_setting('login_capthca'))
		{
			if((Settings::Singleton()->get_setting('login_capthca_remember')) and (cookie::Singleton()->get_cookie_value('username') != NULL) )
			{
				return true;
			}
		
			if(($_SESSION['capthca_security_code'] == $security_code) && (!empty($_SESSION['capthca_security_code'])) ) 
			{
				// Insert you code for processing the form here, e.g emailing the submission, entering it into a database. 
				unset($_SESSION['capthca_security_code']);
				return true;
			} 
			else
			{
				// Insert your code for showing an error message here
				unset($_SESSION['capthca_security_code']);
				return false;
			}
		}
		else
		{
			//Image not used return true
			return true;
		}
	}

}
?>