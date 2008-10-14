<?
class Template
{
	protected static $instance;
	
	private $template_path;
	private $preload = array();
	
	public static function Singleton()
	{
	   if (!isset(self::$instance)) {
		   $c = __CLASS__;
		   self::$instance = new $c;
	   }
	
	   return self::$instance;
	}
	
	public function __clone()
	{
	   trigger_error('Clone is not allowed.', E_USER_ERROR);
	}
	
	function __construct()
	{
		$this->template_path = Settings::Singleton()->get_setting('cms_root').'/skins/'.Settings::Singleton()->get_setting('skin');
		$this->defailt_path = Settings::Singleton()->get_setting('cms_root').'/skins/'.'/default';
		
		$templateinc = $this->template_path.'/template.inc';
		
		if(file_exists($templateinc))
		{
			include($templateinc);
			$this->load_template_options();
		}
	}
	
	public function get_file_content($file)
	{
		if(is_file($this->template_path.'/'.$file))
		{
			return Common::get_file_contents_as_string($this->template_path.'/'.$file);
		}
		else
		{
			if(is_file($this->defailt_path.'/'.$file))
			{
				return Common::get_file_contents_as_string($this->defailt_path.'/'.$file);
			}
		}
		return '';
	}
	
	public function load()
	{
		return $this->build_xslt_template($this->load_xslt_template());
	}
	
	public function edit()
	{
		$path = $this->template_path = Settings::Singleton()->get_setting('cms_root').'/skins/edit/edit.xml';
		return $this->build_xslt_template(file_get_contents($path));
	}
	
	public function send_css()
	{
		// initialize ob_gzhandler function to send and compress data
		ob_start ("ob_gzhandler");
		ob_start ("compress");
		//ob_start();
		session_start();
		
		switch (CMS::Singleton()->page_details['value']) 
		{
			case 'mobile':
				$path = $this->defailt_path .'/mobile.css';
			break;
			
			case 'submodal':
				$path = $this->defailt_path .'/subModal.css';
			break;
			
			default:
				if(Settings::Singleton()->get_setting('skin_alternative_css'))
				{
					$css_file = Settings::Singleton()->get_setting('skin_alternative_css');
				}
				else
				{
					$css_file = 'css.css';
				}
				$path = $this->template_path .'/'.$css_file;
		}
		
		if(is_file($path))
		{
			// send the requisite header information and character set
			Cache::headers_allow_cache($file, true, 2678400, 2678400);
			header ('Content-type: text/css; charset: '.Settings::Singleton()->get_setting('char_set'));

			if(Settings::singleton()->get_setting('css_minify'))
			{
		    	echo(Common::remove_extra_lines(Common::remove_css_comments(file_get_contents($path))));
			}
			else
			{
				echo(file_get_contents($path));
			}
		}
		else
		{
			//Send 404
			//TODO write 404
			die($path);
		}
	}
	
	private function build_xslt_template($xslt)
	{
		return '<?xml version="1.0" encoding="'.Settings::Singleton()->get_setting('char_set').'"?>
<xsl:stylesheet version="1.0" 
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:php="http://www.php.net/xsl"
	xmlns:exslt="http://exslt.org/common"
	lang="en" xml:lang="en"
	xsl:extension-element-prefixes="php exslt">
<xsl:output method="xml" version="1.0"
encoding="'.Settings::Singleton()->get_setting('char_set').'" indent="yes"/>
	'.$xslt.'
</xsl:stylesheet>

';
	}
	
	private function load_xslt_template()
	{
		$path = $this->defailt_path .'/templates/*.xml';
		$xslt = '';
		$files = array();
		
		foreach (glob($path) as $filepath) 
		{
			$filename = basename($filepath);
			if(file_exists($this->template_path .'/templates/'.$filename))
			{
				$filepath = $this->template_path .'/templates/'.$filename;
				
			}
			$files[$filepath] = true;
			$xslt .= file_get_contents($filepath);

		}
		
		if($this->default_path != $this->template_path)
		{
			foreach (glob($this->template_path.'/templates/*.xml') as $filepath) 
			{
				if(!isset($files[$filepath]))
				{
					$xslt .= file_get_contents($filepath);
				}
			}
		}
		return $xslt;
	}
	
	private function load_template_options()
	{		
		//Does this template have a header graphic option?
		if(Settings::Singleton()->get_setting('template_header_graphic') != NULL)
		{
//			echo('<br />');
//			print_r(Settings::Singleton()->get_setting('template_header_graphic'));
//			echo('<br />');
//			print_r(Common::get_random_from_array(Settings::Singleton()->get_setting('template_header_graphic')));
//			echo('<br />');
			
			Settings::Singleton()->overwrite_setting('template_header_graphic', Common::get_random_from_array(Settings::Singleton()->get_setting('template_header_graphic')));
			
//			echo('<br />');
//			print_r(Settings::Singleton()->get_setting('template_header_graphic'));
//			
//			die();
			
			
		}
		
		if(isset($this->preload[0]))
		{
			Settings::Singleton()->new_setting('pre_load_images', $this->preload);
		}
	}
}
?>