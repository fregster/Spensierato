<?php
class Template
{
	private static $instance;

	private $template_path;
	private $working_template_path;
	private $child_template;
	private $inherited_template_paths = array();
	private $preload = array();
	private $lastmodified = 0;
	private $xslt;
	private $list_of_files = array();
	private $filenames = array();

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
		$this->default_path = Settings::Singleton()->get_setting('cms_root').'skins'.DS.'default';
		
		if(Settings::Singleton()->get_setting('use_external_skins') === true && Settings::Singleton()->get_setting('skin'))
		{
			$this->template_path = DIR_INDEX.'skins'.DS.Settings::Singleton()->get_setting('skin');
		}
		else if (Settings::Singleton()->get_setting('skin'))
		{
			$this->template_path = Settings::Singleton()->get_setting('cms_root').'skins'.DS.Settings::Singleton()->get_setting('skin');
		}
		else
		{
			$this->template_path = $this->default_path;
		}
		
		

		$templateinc = $this->template_path.DS.'template.inc';

		if(file_exists($templateinc))
		{
			include($templateinc);
			$this->load_template_options();
		}
		$this->working_template_path = $this->template_path;
		$this->inherited_template_paths[] = $this->template_path;
		$this->build_template();		
	}
	
	public function path()
	{
		return $this->template_path;
	}
	
	public function inhearited_paths()
	{
		return $this->inherited_template_paths;
	}

	public function get_file_content($file)
	{
		$path = $this->get_file_path($file);
		if($path)
		{
			return Common::get_file_contents_as_string($path);
		}
		return '';
	}
	
	public function get_file_path($file)
	{
		foreach($this->inherited_template_paths as $pos_path)
		{
			$path = $pos_path .DS.$file;
			if(file_exists($path))
			{
				return $path;
			}
		}
		return false;
	}

	public function load()
	{
		if(!isset($this->xslt))
		{
			$this->xslt = $this->build_xslt_template($this->load_xslt_template());
		}
		
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
		'transitional-rec' => '<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" "http://www.w3.org/TR/REC-html40/loose.dtd">'
		,

		);

		if(isset($dtd[Settings::singleton()->get_setting('template_dtd')]))
		{
			return $dtd[Settings::singleton()->get_setting('template_dtd')];
		}

		return $dtd['xhtml'];
	}

	public function edit()
	{
		$path = $this->template_path = Settings::Singleton()->get_setting('cms_root').'skins'.DS.'edit'.DS.'edit.xml';
		return $this->build_xslt_template(file_get_contents($path));
	}

	public function css_path()
	{
		$path_vars = CMS::Singleton()->path_vars(1);
		switch ($path_vars)
		{
			case 'mobile':
				$path = $this->default_path.DS.'mobile.css';
			break;

			case 'submodal':
				$path = $this->default_path.DS.'subModal.css';
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
				foreach($this->inherited_template_paths as $pos_css_path)
				{
					$path = $pos_css_path.DS.$css_file;
					if(file_exists($path))
					{
						return $path;
					}
				}
				error::do404();
		}
		return $path;
	}

	public function send_css()
	{
		if(is_file($this->css_path()))
		{			
			$css_m_time = 0;
			$css_files = Settings::Singleton()->get_setting('template_inherits_css');
			if(!isset($css_files[0])){
				$css_files = array();
			}
			$css_files[] = $this->css_path();
			$css_paths = array();
			$estring = '';
			
			$css = persistantcache::Singleton()->get('template_css', 'templates');
			
			//Quick cache return to save stating the FS every hit
			if(!isset($css['created']) || ($css['created'] + 5 < CMS::Singleton()->time()))
			{
				//Cycle the cache files to check for updates
				foreach($css_files as $file)
				{
					$path = $this->get_file_path(basename($file));
					$css_paths[] = $path;
					$estring .= $path;
					$css_m_time = max($css_m_time, filemtime($path));
				}
				$estring = md5($estring);			
				
				
				//If any file has been updated
				if(!isset($css['mtime']) || $css['mtime'] != $css_m_time || $css['paths'] != $css_paths)
				{
					$css = '@charset "UTF-8";';
					foreach($css_paths as $path)
					{
						$css .= str_replace('@CHARSET "UTF-8";', '', file_get_contents($path));
					}
					$css = array('css' => $css, 'mtime' => $css_m_time, 'paths' => $css_paths, 'created' => CMS::Singleton()->time());
					
					persistantcache::Singleton()->save('template_css', $css, 3600, 'templates');
				}
			}
			
			// send the requisite header information and character set
			header ('Content-type: text/css; charset: '.Settings::Singleton()->get_setting('char_set'));
			Cache::headers_allow_cache_dynamic('css', $css_m_time, $estring, true, 31536000, 31536000);

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
		else if(Settings::singleton()->get_setting('template_footer_text'))
		{
			$footer = Settings::singleton()->get_setting('template_footer_text');
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
		$xslt = persistantcache::Singleton()->get('template_xslt', 'templates');

		if(!isset($xslt['mtime']) || $xslt['mtime'] != $this->lastmodified)
		{
			$xslt = ''; //drop the array
			foreach($this->filenames as $filepath)
			{
				$xslt .= file_get_contents($filepath);
			}
		}
		else
		{
			$xslt = $xslt['xslt']; //Overwrite the array
		}

		persistantcache::Singleton()->save('template_xslt', array('xslt' => $xslt, 'mtime' => $this->lastmodified), null, 'templates');

		return $xslt;
	}
	
	private function build_template()
	{
		$path = $this->template_path.DS.'templates'.DS.'*.xml';
		$files = glob($path);
		
		foreach($files as $filepath)
		{
			$this->filenames[basename($filepath)] = $filepath;
			$this->lastmodified = max($this->lastmodified, filemtime($filepath));
		}
		
		$this->extend_template();
	}
	
	private function template_mtime()
	{
		
	}
	
	private function extend_template()
	{
		$old_template = $this->working_template_path;
		if(file_exists($this->working_template_path.DS.'template.inc'))
		{
			require_once($this->working_template_path.DS.'template.inc');
		
			if(Settings::Singleton()->get_setting('template_extends') != false)
			{
				if(file_exists(DIR_INDEX.'skins'.DS.Settings::Singleton()->get_setting('template_extends')))
				{
					$this->working_template_path = file_exists(INDEX_PATH.'skins'.DS.Settings::Singleton()->get_setting('template_extends'));
				}
				else
				{
					$this->working_template_path = Settings::Singleton()->get_setting('cms_root').'skins'.DS.Settings::Singleton()->get_setting('template_extends');
				}
				Settings::Singleton()->overwrite_setting('template_extends', false, false, true);
				
				if($this->working_template_path != $old_template)
				{
					if(file_exists($this->working_template_path.DS.'template.inc'))
					{
						require_once($this->working_template_path.DS.'template.inc');
					}
					
					$files = glob($this->working_template_path.DS.'templates'.DS.'*.xml');
					foreach ($files as $filepath)
					{
						$filename = basename($filepath);
						if(!isset($this->filenames[$filename]))
						{
							$this->filenames[$filename] = $filepath;
							$this->lastmodified = max($this->lastmodified, filemtime($filepath));
						}
					}
					$this->inherited_template_paths[] = $this->working_template_path;
					$this->extend_template();
				}
			}
		}
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