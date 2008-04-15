<?
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
	
	public function render_html($xml)
	{	
		$xslt = new xsltProcessor;
		$xslt->registerPHPFunctions();
		
		// Load the documents and process using $xslt	
		$xslt->importStyleSheet(DomDocument::loadXML(Template::Singleton()->load()));
		
		/* transform the xml document */
		$this->html = $xslt->transformToXML(DomDocument::loadXML($xml));
		
		/* hack to clean html */
		$this->clean_html();
		
		/* send the headers */
		Cache::headers_no_cache();
		
		/* output the html */
		echo(html_entity_decode($this->html));
		
		/* ouput debug stuff */
		//Debug::Singleton()->render();
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
		if(Settings::Singleton()->get_setting('tidy'))
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