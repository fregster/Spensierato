<?php
abstract class Common
{
	public static function get_path_vars()
	{
		//Expand URL to get vars
		return (Common::array_compress(Common::getPathVariables($_SERVER['PHP_SELF'], __FILE__)));
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
		return(str_ireplace('../', '',  $path));
	}

	public static function time_with_microseconds()
	{
		return array_sum(explode(' ',microtime()));

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
		return $GLOBALS['document_root'];
    }

	public static function document_root_folder_only()
    {
		return $GLOBALS['document_root_folder_only'];
    }

	public static function site_root()
	{
		return $GLOBALS['site_root'];
	}

	public static function post_sql_safe($var)
	{
		return(Common::sql_clean(post::variable('string',$var)));
	}

	public static function sql_clean($input)
	{
		$sql_replace = array("'", '"');
		$clean = trim(str_replace($sql_replace, '', (stripslashes($input))));
		return($clean);
	}

	public static function post_is_numeric($var)
	{
		return post::variable('is_numeric',$var);
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
		if (isset($_SERVER['PATH_INFO'])) {
			return explode("/", $_SERVER['PATH_INFO']);
		}
        return array('');
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
		if(class_exists('persistantcache', false))
		{
			$result = persistantcache::Singleton()->get('xml_to_array_'.$xml);
			if($result != false) { return $result; }
		}

		$xml = simplexml_load_string($xml);

		if(!function_exists('process_children'))
		{
			function process_children($xml)
			{
				$php_array = array();

				foreach ($xml->children() as $child)
				{
					if($child->children())
					{
						$php_array[$child->getName()] = process_children($child);
					}
					else
					{
//						$php_array[$child->getName()] = strip_tags($child->saveXML());
						$php_array[$child->getName()] = strip_tags($child->saveXML());
					}
				}

				if(class_exists('persistantcache', false))
				{
					persistantcache::Singleton()->save('xml_to_array_'.$xml, $php_array);
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
				if(is_numeric($tag) and $defaulttag != NULL)
				{
					$tag = $defaulttag;
				}
				if(is_array($value))
				{
					$xml .= "<$tag>".common::array_to_xml_simple_recursion($value, $defaulttag)."</$tag>";
				}
				else
				{
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

	public static function remove_js_comments($code)
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

	public static function html($input, $decode = false)
	{
		if($decode)
		{
			return (html_entity_decode($input, ENT_QUOTES, Settings::singleton()->get_setting('char_set')));
		}
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
				if(!common::email_address_check($to))
				{
					return false;
				}
				else
				{
					$to_string = $to_string . $to .', ';
				}
			}
		}
		else if (is_string($to)) {
			$to_string = $to;
		}

		if(!isset($to_string) || empty($to_string))
		{
			return false;
		}
		
		if(is_array($bto))
		{
			foreach($bcc as $bcc)
			{
				if(!common::email_address_check($bcc))
				{
					return false;
				}
				else
				{
					$bcc_string = $bcc_string . $bcc .', ';
				}
			}
		}
		else if (is_string($bcc)) {
			$bcc_string = $bcc;
		}
		
		if(is_array($cc))
		{
			foreach($cc as $cc)
			{
				if(!common::email_address_check($cc))
				{
					return false;
				}
				else
				{
					$cc_string = $cc_string . $cc .', ';
				}
			}
		}
		else if (is_string($cc)) {
			$cc_string = $cc;
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
		return(mail($to_string, $subject, $message, $headers));
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
		$validator = new EmailAddressValidator();
		return $validator->check_email_address($email);
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
    }

    public static function rewrite_enabled()
    {
    	if ( in_array("mod_rewrite", apache_get_modules()) )
    	{
    		return true;
    	}

    	return false;
    }

	public static function strip_non_alpha_numeric($input)
	{
		return preg_replace('/[^a-zA-Z0-9\s\,\.\-\n\r\t@\/\_\(\)]/','',$input);
	}

	public static function mysql_stop_words_string()
	{
		return "a's, able, about, above, according, accordingly, across, actually, after, afterwards, again, against, ain't, all, allow, allows, almost, alone, along, already, also, although, always, am, among, amongst, an, and, another, any, anybody, anyhow, anyone, anything, anyway, anyways, anywhere, apart, appear, appreciate, appropriate, are, aren't, around, as, aside, ask, asking, associated, at, available, away, awfully, be, became, because, become, becomes, becoming, been, before, beforehand, behind, being, believe, below, beside, besides, best, better, between, beyond, both, brief, but, by, c'mon, c's, came, can, can't, cannot, cant, cause, causes, certain, certainly, changes, clearly, co, com, come, comes, concerning, consequently, consider, considering, contain, containing, contains, corresponding, could, couldn't, course, currently, definitely, described, despite, did, didn't, different, do, does, doesn't, doing, don't, done, down, downwards, during, each, edu, eg, eight, either, else, elsewhere, enough, entirely, especially, et, etc, even, ever, every, everybody, everyone, everything, everywhere, ex, exactly, example, except, far, few, fifth, first, five, followed, following, follows, for, former, formerly, forth, four, from, further, furthermore, get, gets, getting, given, gives, go, goes, going, gone, got, gotten, greetings, had, hadn't, happens, hardly, has, hasn't, have, haven't, having, he, he's, hello, help, hence, her, here, here's, hereafter, hereby, herein, hereupon, hers, herself, hi, him, himself, his, hither, hopefully, how, howbeit, however, i'd, i'll, i'm, i've, ie, if, ignored, immediate, in, inasmuch, inc, indeed, indicate, indicated, indicates, inner, insofar, instead, into, inward, is, isn't, it, it'd, it'll, it's, its, itself, just, keep, keeps, kept, know, knows, known, last, lately, later, latter, latterly, least, less, lest, let, let's, like, liked, likely, little, look, looking, looks, ltd, mainly, many, may, maybe, me, mean, meanwhile, merely, might, more, moreover, most, mostly, much, must, my, myself, name, namely, nd, near, nearly, necessary, need, needs, neither, never, nevertheless, new, next, nine, no, nobody, non, none, noone, nor, normally, not, nothing, novel, now, nowhere, obviously, of, off, often, oh, ok, okay, old, on, once, one, ones, only, onto, or, other, others, otherwise, ought, our, ours, ourselves, out, outside, over, overall, own, particular, particularly, per, perhaps, placed, please, plus, possible, presumably, probably, provides, que, quite, qv, rather, rd, re, really, reasonably, regarding, regardless, regards, relatively, respectively, right, said, same, saw, say, saying, says, second, secondly, see, seeing, seem, seemed, seeming, seems, seen, self, selves, sensible, sent, serious, seriously, seven, several, shall, she, should, shouldn't, since, six, so, some, somebody, somehow, someone, something, sometime, sometimes, somewhat, somewhere, soon, sorry, specified, specify, specifying, still, sub, such, sup, sure, t's, take, taken, tell, tends, th, than, thank, thanks, thanx, that, that's, thats, the, their, theirs, them, themselves, then, thence, there, there's, thereafter, thereby, therefore, therein, theres, thereupon, these, they, they'd, they'll, they're, they've, think, third, this, thorough, thoroughly, those, though, three, through, throughout, thru, thus, to, together, too, took, toward, towards, tried, tries, truly, try, trying, twice, two, un, under, unfortunately, unless, unlikely, until, unto, up, upon, us, use, used, useful, uses, using, usually, value, various, very, via, viz, vs, want, wants, was, wasn't, way, we, we'd, we'll, we're, we've, welcome, well, went, were, weren't, what, what's, whatever, when, whence, whenever, where, where's, whereafter, whereas, whereby, wherein, whereupon, wherever, whether, which, while, whither, who, who's, whoever, whole, whom, whose, why, will, willing, wish, with, within, without, won't, wonder, would, would, wouldn't, yes, yet, you, you'd, you'll, you're, you've, your, yours, yourself, yourselves, zero";
	}

	public static function mysql_stop_words_array()
	{
		return explode(', ', common::mysql_stop_words_string());
	}

	public static function strip_external_xml_decleration($xml)
	{
		$stylesheet='<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
			<xsl:output omit-xml-declaration="yes" indent="yes"/>
			<xsl:strip-space elements="*" />

				<xsl:template match="/">
					 <xsl:copy-of select="*"/>
				</xsl:template>
		</xsl:stylesheet>';

		return render::singleton()->transform_xml_with_xslt($xml, $stylesheet);
	}

	public static function spellcheck_array($words = array())
	{
		foreach($words as $key => $term)
		{
			$corrected = Common::spellcheck($term);
			if($corrected != $term)
			{
				$words[$key] = $corrected;
			}
		}
		return $words;
	}

	public static function spellcheck($word)
	{

	}
	
	public static function requires_session()
	{
		if(!common::cookie_check())
		{
			error::error('Cookies are required, please enable cookies and try again');
		}
	}
	
	public static function retrieve_values_from_array($array, $fields)
	{
		if(is_array($array) && count($array)>0)
		{
			$result = array();
			if(is_array($fields))
			{
				foreach($array as $value)
				{
					$r = array();
					foreach($fields as $field)
					{
						$r[] = $value[$field];
					}
					$result[] = $r;
				}
			}
			else
			{
				foreach($array as $value)
				{
					$result[] = $value[$fields];
				}
			}
			
			return $result;
		}
		return false;
	}
}
?>