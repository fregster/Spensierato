<?php
class Template
{
	private static $instance;

	private $template_path;
	private $preload = array();
	private $lastmodified = 0;
	private $xslt;

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

	protected function __construct()
	{
		if(Settings::Singleton()->get_setting('use_external_skins') === true)
		{
			$this->template_path = file_root().'/skins/'.Settings::Singleton()->get_setting('skin');
		}
		else
		{
			$this->template_path = Settings::Singleton()->get_setting('cms_root').'/skins/'.Settings::Singleton()->get_setting('skin');
		}
		
		$this->default_path = Settings::Singleton()->get_setting('cms_root').'/skins/default';

		$templateinc = $this->template_path.'/template.inc';

		if(file_exists($templateinc))
		{
			include($templateinc);
			$this->load_template_options();
		}
		$this->xslt = $this->build_xslt_template($this->load_xslt_template());
	}
	
	public function path()
	{
		return $this->template_path;
	}

	public function get_file_content($file)
	{
		if(is_file($this->template_path.'/'.$file))
		{
			return Common::get_file_contents_as_string($this->template_path.'/'.$file);
		}
		else
		{
			if(is_file($this->default_path.'/'.$file))
			{
				return Common::get_file_contents_as_string($this->default_path.'/'.$file);
			}
		}
		return '';
	}

	public function load()
	{
		return $this->xslt;
	}

	public function lastmodified()
	{
		return $this->lastmodified;
	}

	public function dtd()
	{
		$dtd = array(
		'xhtml' => '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
	  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">',
		'transitional' => '<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">',

		);

		if(isset($dtd[Settings::singleton()->get_setting('template_dtd')]))
		{
			return $dtd[Settings::singleton()->get_setting('template_dtd')];
		}

		return $dtd['xhtml'];
	}

	public function edit()
	{
		$path = $this->template_path = Settings::Singleton()->get_setting('cms_root').'/skins/edit/edit.xml';
		return $this->build_xslt_template(file_get_contents($path));
	}

	public function css_path()
	{
		$path_vars = CMS::Singleton()->path_vars(1);
		switch ($path_vars)
		{
			case 'mobile':
				$path = $this->default_path .'/mobile.css';
			break;

			case 'submodal':
				$path = $this->default_path .'/subModal.css';
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
		return $path;
	}

	public function send_css()
	{
		if(is_file($this->css_path()))
		{
			// send the requisite header information and character set
			Cache::headers_allow_cache($this->css_path(), true);
			header ('Content-type: text/css; charset: '.Settings::Singleton()->get_setting('char_set'));

			$css = persistantcache::Singleton()->get('template_css');
			if(!isset($css['mtime']) || $css['mtime'] != filemtime($this->css_path()))
			{
				$css = array('css' => file_get_contents($this->css_path()), 'mtime' => filemtime($this->css_path()));
				persistantcache::Singleton()->save('template_css', $css);
			}

			if(Settings::singleton()->get_setting('css_minify'))
			{
		    	return(Common::remove_extra_lines(Common::remove_css_comments($css['css'])));
			}
			else
			{
				return($css['css']);
			}
		}
		error::do404();
	}

	private function build_xslt_template($xslt)
	{
		$xslt = '<?xml version="1.0" encoding="'.Settings::Singleton()->get_setting('char_set').'"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" >
<xsl:output method="html" version="1.0" encoding="'.Settings::Singleton()->get_setting('char_set').'" indent="yes"/>
	'.$xslt.'
</xsl:stylesheet>';

		if(currentpage::singleton()->xslt() === true)
		{
			$footer = '<div id="page_section_footer">Page rendered client side</div>';
		}
		else
		{
			/* call end CMS CMS::Singleton()->end_cms() to work out page generation times and replace the footer text */
			$footer = CMS::Singleton()->end_cms();
		}

		return str_replace('***FOOTER_TEXT***', $footer, $xslt);

	}

	private function load_xslt_template()
	{
		$path = $this->default_path .'/templates/*.xml';
		$files = array();
		$list_of_files = glob($path);
		foreach ($list_of_files as $filepath)
		{
			$filename = basename($filepath);
			if(file_exists($this->template_path .'/templates/'.$filename))
			{
				$filepath = $this->template_path .'/templates/'.$filename;

			}
			$files[$filepath] = true;
			$this->lastmodified = max($this->lastmodified, filemtime($filepath));
		}

		if($this->default_path != $this->template_path && (Settings::Singleton()->get_setting('template_dont_extend_default') == false))
		{
			$list_of_files = glob($this->template_path.'/templates/*.xml');
			foreach ($list_of_files as $filepath)
			{
				if(!isset($files[$filepath]))
				{
					$files[$filepath] = true;
					$this->lastmodified = max($this->lastmodified, filemtime($filepath));
				}
			}
		}
		unset($list_of_files);

		$xslt = persistantcache::Singleton()->get('template_xslt');

		if(!isset($xslt['mtime']) || $xslt['mtime'] != $this->lastmodified)
		{
			$xslt = ''; //drop the array
			foreach($files as $key => $value)
			{
				$xslt .= file_get_contents($key);
			}
		}
		else
		{
			$xslt = $xslt['xslt']; //Overwrite the array
		}

		persistantcache::Singleton()->save('template_xslt', array('xslt' => $xslt, 'mtime' => $this->lastmodified));

		return $xslt;
	}

	private function load_template_options()
	{
		//Does this template have a header graphic option?
		if(Settings::Singleton()->get_setting('template_header_graphic') != NULL)
		{
			Settings::Singleton()->overwrite_setting('template_header_graphic', Common::get_random_from_array(Settings::Singleton()->get_setting('template_header_graphic')));
		}

		if(isset($this->preload[0]))
		{
			Settings::Singleton()->new_setting('pre_load_images', $this->preload);
		}
	}
}
?>