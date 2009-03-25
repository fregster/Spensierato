<?php
class Render
{
	private static $instance;
	private $render_mode = 'VIEW';
	private $template_folder;
	private $html;

	public static function Singleton()
	{
	   if (!isset(self::$instance)) {
		   $c = __CLASS__;
		   self::$instance = new $c;
	   }

	   return self::$instance;
	}

	function __construct()
	{
		if(isset($_SESSION['render_admin']))
		{
			$this->render_mode = 'ADMIN';
		}
	}

	public function __clone()
	{
	   trigger_error('Clone is not allowed.', E_USER_ERROR);
	}

	public function renderMode()
	{
		return $this->render_mode;
	}

	public function toggle_render_mode()
	{
		if(isset($_SESSION['render_admin']))
		{
			session_unregister("render_admin");
		}
		else
		{
			$_SESSION['render_admin'] = true;
		}
	}

	public function render_html($xml, $allow_cache = false)
	{
		/* send the headers and do 304 check before processing the page */
		if(options::Singleton()->get_option('admin_mode') or !$allow_cache)
		{
			//When in admin mode do not cache and do not allow cacheing
			Cache::headers_no_cache();
		}
		else if (user::singleton()->is_logged_in())
		{
			Cache::headers_allow_cache_dynamic(currentPage::Singleton()->id(), content::Singleton()->last_updated(), content::Singleton()->e_string(), false);
		}

		$this->render(Template::Singleton()->load(), $xml);
	}

	public function render_edit($xml)
	{
		Cache::headers_no_cache();
		$this->render(Template::Singleton()->edit(), $xml);
	}

	public function render_file($filepath)
	{
		Cache::headers_allow_cache($filepath);
		$this->final_output(file_get_contents($filepath), false);
	}

	public function render_this($input, $htmldecode = false)
	{
		$this->final_output($input, $htmldecode);
		exit;
	}

	private function render($template, $xml)
	{
		// Load the site XML pass directly into Render
		if(get::variable('xml','int') or Settings::singleton()->get_setting('send_as_xml') or class_exists('xsltProcessor') === false)
		{
			//Just send the XML
			header('content-type: text/xml');

			$search = '<?xml version="1.0"?>';
			$replace = '<?xml version="1.0" encoding="'. Settings::singleton()->get_setting('char_set').'" ?><?xml-stylesheet type="text/xsl" href="'. Settings::singleton()->get_setting('document_root').'/xslt" ?>';
			if(get::variable('xml','int') == 1) { $replace = '<?xml version="1.0" encoding="'. Settings::singleton()->get_setting('char_set').'" ?>'; }

			$this->render_this(str_replace($search, $replace, $xml));
			exit;
		}

		$xslt = new xsltProcessor;
		$xslt->registerPHPFunctions();

		// Load the documents and process using $xslt
		$xslt->importStyleSheet(DomDocument::loadXML($template));

		/* transform the xml document */
		$this->html = $xslt->transformToXML(DomDocument::loadXML($xml));

		/* hack to clean html */
		$this->clean_html();

		/* output the html */
		$this->send_html();
		exit;
	}

	private function final_output($content, $htmldecode = true)
	{
		if($htmldecode) { $content = html_entity_decode($content); }


		/* If compression is requested and the level is numeric */
		if(is_numeric(Settings::Singleton()->get_setting('output_compression_level')) and $this->acceptsGZIP() and Settings::Singleton()->get_setting('output_compression_level') > 0)
		{
//			ob_start("ob_gzhandler");
//			ob_start("compress");
			if(ob_get_length() != false) //If content has been pushed into the Output buffer add it to the contents to be gziped and clean the buffer
			{
				$content = ob_get_clean(). $content;
				
			}
			$contents = gzencode($content, Settings::Singleton()->get_setting('output_compression_level'));
			header ('Content-Encoding: gzip');
			header ('Content-Length: ' . strlen($contents));
			echo($contents);
		}
		else
		{
			/* output the html */
			echo($content);
		}
	}

	public function send_css()
	{
		Cache::headers_allow_cache(Template::Singleton()->css_path(), true);
		$this->final_output(Template::Singleton()->send_css());
		exit;
	}

	public function send_xslt()
	{
		$contents = Template::Singleton()->load();

		$ETag = md5($contents);
		$expires = gmdate('D, d M Y H:i:s', ($this->time + 36000)) . ' GMT';
		$lastmoded = gmdate('D, d M Y H:i:s', Template::Singleton()->lastmodified()) . ' GMT';

		cache::headers_check_custom_etag($ETag, $lastmoded);

		header('Content-Type: text/xml; charset='.Settings::Singleton()->get_setting('char_set'));
		cache::send_headers($ETag, $lastmoded, $expires);
		$this->final_output($contents, false);
	}

	private function send_html()
	{
		$this->final_output($this->html);
	}

	private function acceptsGZip()
	{
		$accept = str_replace(" ","",strtolower($_SERVER['HTTP_ACCEPT_ENCODING']) );
		$accept = explode(",",$accept);
		return in_array("gzip",$accept);
	}

	private function clean_html()
	{
		$remove = array('xmlns:php="http://www.php.net/xsl"',
						'xmlns:exslt="http://exslt.org/common"',
						'<?xml version="1.0" encoding="'.Settings::Singleton()->get_setting('char_set').'"?>');

		/*$remove = array('<?xml version="1.0" encoding="UTF-8"?>');*/

		$this->html = template::singleton()->dtd().str_replace($remove, '', $this->html);

		$this->tidy();
	}

	private function tidy()
	{
		if( (Settings::Singleton()->get_setting('tidy')) and (function_exists('tidy_parse_string')))
		{
			/**
			 * Set configuration options for HTML tidy.
			 */
			$config = array('indent' => TRUE,
							'output-xhtml' => TRUE,
							'wrap' => 200,
							'doctype' => 'strict');

			/**
			 * Run HTML tidy on the html code
			 */
			$tidy = tidy_parse_string($this->html, $config, 'UTF8');
			$tidy->cleanRepair();

			$this->html = $tidy;
		}
	}
}
?>