<?
abstract class Common
{
	public static function get_path_vars()
	{
		//Expand URL to get vars
		return (Common::array_compress(Common::getPathVariables($_SERVER[PHP_SELF], __FILE__)));
	}
	
	public static function return_hash($input, $use_random = true)
	{
		if($use_random) { $input = $input.$GLOBALS['random_key']; }
		return hash(Settings::Singleton()->get_setting('site_option_encryption'), $input);
	}
	
	public static function bounce_to_referer()
	{
		if($_SERVER['HTTP_REFERER'] != NULL)
		{
			$url = $_SERVER['HTTP_REFERER'];
		}
		else
		{
			$url = Settings::Singleton()->get_setting('homepage');
		}
		Common::redirect($url);
	}
	
	public static function redirect($url)
	{
		if(substr($url, 0, 4) != 'http')
		{
			$url = Settings::Singleton()->get_setting('document_root').'/'.$url;
		}
		header('Location: '.$url);
		die();
	}
	
	public static function redirect_inc_vars($url, $vars)
	{
		if(substr($url, 0, 4) != 'http')
		{
			$url = Settings::Singleton()->get_setting('document_root').'/'.$url;
		}
		
		$var_string ='?';
		foreach($vars as $key => $var)
		{
			$var_string .= $key.'='.$var.'&';
		}
		header('Location: '.$url.$var_string);
		die();
	}
	public static function clean_path($path)
	{
		return(str_ireplace('..', '',  $path));
	}
	
	public static function time_with_microseconds()
	{
		return CMS::Singleton()->time()+Common::micro_time();
	}
	/**
	 * Replaced with time and microtime in CMS
	 */
	public static function micro_time()
	{
		return CMS::Singleton()->microtime();
	}
	
	public static function microtime_to_seconds($time, $round = 4)
	{
		return round($time / 10000000000, $round);
	}

	public static function get_file_contents_as_string($file)
	{
		if(is_file($file))
		{
			$search = array('**CMS**DocumentRoot',
							'**CMS**CMSRoot');
			$replace = array(	Settings::Singleton()->get_setting('document_root'),
								Settings::Singleton()->get_setting('cms_root'));
			return str_replace($search, $replace, file_get_contents(Common::clean_path($file)));
		}
		return '';
	}

	public static function document_root()
    {
    	$a = substr($_SERVER['PHP_SELF'], 0 , strripos($_SERVER['PHP_SELF'], '.php'));
    	$document_root = substr($a, 0, strripos($a, '/'));
    	if($document_root == '')
    	{
    		if( ($_SERVER['HTTPS'] == '') || ($_SERVER['HTTPS'] == 'off') )
    		{
    			$proto = 'http://';
    		}
    		else
    		{
    			$proto = 'https://';
    		}
    		$document_root = $proto . $_SERVER['SERVER_NAME'];
    		
    	}
    	
    	if(!Common::rewrite_enabled())
    	{
    		$document_root .= '/index.php';
    	}
    	
		return $document_root;
    }
	
	public static function site_root()
	{
		
		$ext = substr($_SERVER[SCRIPT_FILENAME], -4 , 4);
		$a = substr($_SERVER[SCRIPT_FILENAME], 0 , strripos($_SERVER[SCRIPT_FILENAME], $ext));
		return substr($a, 0, strripos($a, '/'));
	}
	
	public static function post_sql_safe($var)
	{
		return(Common::sql_clean($_POST[$var]));
	}
	
	public static function sql_clean($input)
	{
		$sql_replace = array("'", '"');
		$clean = trim(str_replace($sql_replace, '', (stripslashes($input))));
		return($clean);
	}
	
	public static function post_is_numeric($var)
	{
		if(is_numeric($_POST[$var]))
		{
			return $_POST[$var];
		}
		else
		{
			return false;
		}
	}
	
	private static function array_compress($array) 
	{
	    $aReturn = array();
	    foreach ($array as $value)
	    if (strlen($value) > 0) { $aReturn[] = $value; }
	    return $aReturn;
	}
	
	private static function getPathVariables()
    {
        return explode("/", $_SERVER['PATH_INFO']);
    }
	
	public static function clean_and_combine_url($url, $int)
	{
		return Common::clean_url(implode('/', array_slice($url, $int)));
	}
	
	public static function clean_url($url)
	{
		return str_ireplace('..', '',  $url);
	}
	
	public static function xml_merge($xml1, $xml2)
	{	
		return array_to_xml(common::xml_merge_to_array($xml1, $xml2));
	}
	
	public static function xml_merge_to_array($xml1, $xml2)
	{
		return array_merge_recursive(xml_to_array($xml1), xml_to_array($xml2));
	}
	
	public static function xml_to_array($xml)
	{
		$xml = simplexml_load_string($xml);
		
		if(!function_exists('process_children'))
		{
			function process_children($xml)
			{
				foreach ($xml->children() as $child)
				{				
					foreach($child->children() as $grandchild)
					{
						$php_array[$child->getName()] = process_children($child);
					}
					if($child->children())
					{
						$php_array[$child->getName()] = process_children($child);
					}
					else
					{
						$php_array[$child->getName()] = strip_tags($child->saveXML());
					}
				}
				return $php_array;
			}
		}
				
		return process_children($xml);
	}
	
	public static function array_to_xml($array, $serializer_options = NULL)
	{
		// include class file 
		require_once('XML/Serializer.php'); 
		
		// An array of serializer options
		if(is_null($serializer_options))
		{
			$serializer_options = array ( 
			   'addDecl' => FALSE, 
			   'encoding' => 'ISO-8859-1', 
			   'indent' => '  ',
			   'rootName'  => 'xml',
			   'defaultTagName'  => 'item'
			);
		}
		
		// create object 
		$serializer = new XML_Serializer($serializer_options);
		
		// perform serialization 
		$result = $serializer->serialize($array); 
		
		// check result code and display XML if success 
		if($result === true) 
		{ 
			return $serializer->getSerializedData(); 
		} 
	}
	
	public static function array_to_xml_simple($array, $root, $defaulttag = NULL)
	{
		return 	"<$root>".Common::array_to_xml_simple_recursion($array, $defaulttag)."</$root>";
	}
	
	public static function array_to_xml_simple_recursion($array, $defaulttag = NULL)
	{		
		if(is_array($array))
		{
			$xml = '';
			foreach($array as $tag => $value)
			{
				if(is_array($tag))
				{
					$xml .= common::array_to_xml_simple_recursion($tag);
				}
				else
				{
					if(is_numeric($tag) and $defaulttag != NULL)
					{
						$tag = $defaulttag;
					}
					$xml .= "<$tag>$value</$tag>";
				}
			}
			return $xml;
		}
	}

	public static function random_number($start = 0, $count = 10)
	{
		return rand($start, ($start+$count));
	}
	
	public static function get_random_from_array($input)
	{
			//If it is an array then randomly choose one
			if(isset($input[0]))
			{
				return 	$input[Common::random_number(0, count($input)-1)];			
			}

			return $input;	
	}

	public static function remove_extra_lines($input)
	{
		// remove comments with /* and */
	    // remove tabs, spaces, newlines, etc.
	    return str_replace(array("\r\n", "\r", "\n", "\t", '  ', '    ', '    '), '', $input);
	}
	
	public static function remove_css_comments($input)
	{
		$input = preg_replace('!/\*[^*]*\*+([^/][^*]*\*+)*/!', '', $input);
		return Common::remove_extra_lines($input);
	}
	
	public function remove_js_comments($code) 
	{  
     $mq = $sq = $mc = $sc = false;  
     $output = "";  
     for($i = 0; $i < strlen($code); $i++)  
     {  
         $l = $code{$i};  
         $n = $i+1;  
         $ll = $code{$i}.$code{$n};  
   
         switch($l)  
         {  
             case "\n":  
                 $sc = false;  
             break;  
             case "/":  
                 if($code{$n} == "/")  
                 {  
                     if(!$sc && !$mc && !$sq && !$mq)  
                         $sc = true;  
                     $i++;  
                 }  
                 else if($code{$n} == "*")  
                 {  
                     if(!$sc && !$mc && !$sq && !$mq)  
                         $mc = true;  
                     $i++;  
                 }  
                 continue 2;  
             break;  
             case "'":  
                 if(!$sc && !$mc && !$mq)  
                     $sq = !$sq;  
             break;  
             case "\"":  
                 if(!$sc && !$mc && !$sq)  
                     $mq = !$mq;  
             break;  
             case "*":  
                 if($code{$n} == "/")  
                 {  
                     if(!$sc && !$sq && !$mq)  
                         $mc = false;  
                     $i++;  
                 }  
                 continue 2;  
             break;  
         }  
         if(!$sc && !$mc)  
             $output .= $l;  
	}  
	return $output;  
}  
	
	public static function generate_random_string($length, $allow_numbers = false)
	{
		/* list all possible characters, similar looking characters and vowels have been removed */
		$possible = 'abcdfghjkmnpqrstvwxyz';
		if($allow_numbers)
		{
			$possible .= '23456789';
		}
		$code = '';
		$i = 0;
		while ($i < $length)
		{ 
			$code .= substr($possible, mt_rand(0, strlen($possible)-1), 1);
			$i++;
		}
		
		return $code;
	}

	public static function node_from_xml($xml, $node)
	{
		$xml_obj = simplexml_load_string($xml);
		if($xml_obj)
		{
			$result= $xml_obj->xpath($node);
	
			foreach ((array)$result as $result) 
			{
		    	return($result);
			}
		}
	}

	public static function html($input)
	{
		return htmlentities($input, ENT_QUOTES, Settings::singleton()->get_setting('char_set'));
	}

	/**
	 * This method currently only use PHP inbuilt mailer which is not very efficent if ussing on larger sites
	 * then please write a method that extends or replaces this with one using PEAR::Mail and give me the patch!
	 */
	public static function email($to = array(), $from, $subject, $content, $html = true, $bcc = NULL, $cc = NULL)
	{
		if(is_array($to))
		{
			foreach($to as $to)
			{
				if(!common::email_address_check())
				{
					return false;
				}
				else
				{
					$to_string = $to_string . $to .', ';
				}
			}
		}
		else
		{
			$to_string = $to;
		}
		
		if(!isset($to_string))
		{
			return false;
		}
		
		//Subjects should not have line breaks in them or html
		$subject = str_replace('\n', '', strip_tags($subject));
		
		//Email text should not be more than 70 chars
		$message = wordwrap($content, 70);
		
		$headers = '';
		if($html)
		{
			// To send HTML mail, the Content-type header must be set
			$headers .= 'MIME-Version: 1.0' . "\r\n";
			$headers .= 'Content-type: text/html; charset=iso-8859-1' . "\r\n";
			
			$message = '<html><head><title>'.$subject.'</title></head><body>'.$message.'</body></html>';
		}
		
		// Additional headers
		$headers .= 'From: ' .$from. "\r\n";
		$headers .= 'Return-path: ' .$from. "\r\n";
		if(isset($cc_string)) { $headers .= 'Cc: ' .$cc_string. "\r\n"; }
		if(isset($bcc_string)) { $headers .= 'Bcc: ' .$bcc_string. "\r\n"; }
		
		// Mail it
		return mail($to_string, $subject, $message, $headers);
	}
	
	public static function group_email($bcc = array(), $subject, $content, $from = NULL, $html = true)
	{
		if($from == NULL)
		{
			$from = user::Singleton()->email();
		}
		return common::email($from, $from, $subject, $content, $bcc, $html);
	}
	
	public static function email_address_check($email)
	{
		return true;
	}

	public static function cookie_check()
	{
		if(!isset($_SESSION['cookies_enabled']))
		{
			if(!isset($_GET['cookie_check']))
			{
				$_SESSION['cookies_enabled'] = true;
				common::redirect_inc_vars(Common::clean_and_combine_url(CMS::Singleton()->path_vars, 0), array('cookie_check' => 1));
			}
			else
			{
				warnings::singleton()->register('Cookies', 'Cookies must be enabled to be able to login');
				return false;
			}
		}
		return true;
	}
	
	public static  function returnMIMEType($filename)
    {
     	return mimetypegetter::get($filename);   
//    	preg_match("|\.([a-z0-9]{2,4})$|i", $filename, $fileSuffix);
//
//        switch(strtolower($fileSuffix[1]))
//        {
//            case "js" :
//                return "application/x-javascript";
//
//            case "json" :
//                return "application/json";
//
//            case "jpg" :
//            case "jpeg" :
//            case "jpe" :
//                return "image/jpg";
//
//            case "png" :
//            case "gif" :
//            case "bmp" :
//            case "tiff" :
//                return "image/".strtolower($fileSuffix[1]);
//
//            case "css" :
//                return "text/css";
//
//            case "xml" :
//                return "application/xml";
//
//            case "doc" :
//            case "docx" :
//                return "application/msword";
//
//            case "xls" :
//            case "xlt" :
//            case "xlm" :
//            case "xld" :
//            case "xla" :
//            case "xlc" :
//            case "xlw" :
//            case "xll" :
//                return "application/vnd.ms-excel";
//
//            case "ppt" :
//            case "pps" :
//                return "application/vnd.ms-powerpoint";
//
//            case "rtf" :
//                return "application/rtf";
//
//            case "pdf" :
//                return "application/pdf";
//
//            case "html" :
//            case "htm" :
//            case "php" :
//                return "text/html";
//
//            case "txt" :
//                return "text/plain";
//
//            case "mpeg" :
//            case "mpg" :
//            case "mpe" :
//                return "video/mpeg";
//
//            case "mp3" :
//                return "audio/mpeg3";
//
//            case "wav" :
//                return "audio/wav";
//
//            case "aiff" :
//            case "aif" :
//                return "audio/aiff";
//
//            case "avi" :
//                return "video/msvideo";
//
//            case "wmv" :
//                return "video/x-ms-wmv";
//
//            case "mov" :
//                return "video/quicktime";
//
//            case "zip" :
//                return "application/zip";
//
//            case "tar" :
//                return "application/x-tar";
//
//            case "swf" :
//                return "application/x-shockwave-flash";
//                
//             case "flv" :
//                return "video/x-flv";   
//
//            default :
//            if(function_exists("mime_content_type"))
//            {
//                $fileSuffix = mime_content_type($filename);
//            }
//
////            return "unknown/" . trim($fileSuffix[0], ".");
//			return false;
//        }
    }
    
    public function rewrite_enabled()
    {
    	if ( in_array("mod_rewrite", apache_get_modules()) ) 
    	{ 
    		return true;
    	} 

    	return false;
    }
}
?>