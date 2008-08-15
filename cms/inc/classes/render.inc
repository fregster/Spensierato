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
		else
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
	
	private function render($template, $xml)
	{
		// Load the site XML pass directly into Render
		if($_GET['xml'] or Settings::singleton()->get_setting('send_as_xml'))
		{
			//Just send the XML
			header('content-type: text/xml');
			$search = '<?xml version="1.0"?>';
			$replace = '<?xml version="1.0" encoding="'. Settings::singleton()->get_setting('char_set').'"?>';
			if($_GET['xml'] == 2) { $replace .= '<?xml-stylesheet href="'. Settings::singleton()->get_setting('document_root').'/xslt" type="text/xsl" ?>'; }
/*			$replace = '<?xml version="1.0" encoding="'. Settings::singleton()->get_setting('char_set').'"?>';*/
			
			echo(str_replace($search, $replace, $xml));
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
		
		/* ouput debug stuff */
		//Debug::Singleton()->render();
		die();
	}
	
	private function send_html()
	{
		/* If compression is requested and the level is numeric */
		if(is_numeric(Settings::Singleton()->get_setting('output_compression_level')) and $this->acceptsGZIP() and Settings::Singleton()->get_setting('output_compression_level') > 0)
		{
			$contents = gzencode(html_entity_decode($this->html), Settings::Singleton()->get_setting('output_compression_level'));
			header ('Content-Encoding: gzip');
			header ('Content-Length: ' . strlen($contents));
			echo $contents;
		}
		else
		{
			/* output the html */
			echo(html_entity_decode($this->html));
		}
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
						
		$this->html = '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" 
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">'.str_replace($remove, '', $this->html);
	
		$this->tidy();
		
		/* call end CMS CMS::Singleton()->end_cms() to work out page generation times and replace the footer text */
		$this->html = str_replace('***FOOTER_TEXT***', CMS::Singleton()->end_cms(), $this->html);
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