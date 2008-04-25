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
	
	public static function clean_path($path)
	{
		return(str_ireplace('..', '',  $path));
	}
	
	public static function time_with_microseconds()
	{
		return time()+microtime();
	}
	
	public static function micro_time()
	{
		return microtime();
	}
	
	public static function microtime_to_seconds($time, $round = 4)
	{
		return round($time / 10000000000, $round);
	}

	public function get_file_contents_as_string($file)
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
	
	public static function xml_to_array($xml)
	{
		$xml = simplexml_load_string($xml);
		
		foreach ($xml->children() as $child)
		{
			$php_array[$child->getName()] = strip_tags($child->saveXML());
		}

		return $php_array;
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
	
	public static function generate_random_string($length)
	{
		/* list all possible characters, similar looking characters and vowels have been removed */
		$possible = '23456789bcdfghjkmnpqrstvwxyz';
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
		$xml = simplexml_load_string($xml);
		$result= $xml->xpath($node);

		foreach ((array)$result as $result) 
		{
	    	return($result);
		}
	}
}
?>