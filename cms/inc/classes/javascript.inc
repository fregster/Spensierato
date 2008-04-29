<?php

	/************************************************************************
	 * CSS and Javascript Combinator 0.5
	 * Copyright 2006 by Niels Leenheer
	 *
	 * Permission is hereby granted, free of charge, to any person obtaining
	 * a copy of this software and associated documentation files (the
	 * "Software"), to deal in the Software without restriction, including
	 * without limitation the rights to use, copy, modify, merge, publish,
	 * distribute, sublicense, and/or sell copies of the Software, and to
	 * permit persons to whom the Software is furnished to do so, subject to
	 * the following conditions:
	 * 
	 * The above copyright notice and this permission notice shall be
	 * included in all copies or substantial portions of the Software.
	 *
	 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
	 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
	 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
	 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
	 */

abstract class javascript
{
	public static function run($files)
	{
		$cachedir = Settings::singleton()->get_setting('cms_root') . '/writeable/cache';
		$jsdir    = Settings::singleton()->get_setting('cms_root') . '/js';

		if(is_writable($cachedir))
		{
			$cache 	  = true;
		}
		else
		{
			$cachedir = '/tmp/cms';
			if(is_writable($cachedir))
			{
				$cache 	  = true;
			}
			else
			{
				$cache 	  = false;
			}
		}
		
		//Create a hash of the file names for createing and checking a single unique cache file
		$hash = sha1($files);
		$base = realpath($jsdir);
		$type = 'javascript';
		$elements = explode(',', $files);
		
		// Determine last modification date of the files
		$lastmodified = 0;
		while (list(,$element) = each($elements)) {
			$path = realpath($base . '/' . $element);
		
			if (($type == 'javascript' && substr($path, -3) != '.js') || 
				($type == 'css' && substr($path, -4) != '.css')) {
				header ("HTTP/1.0 403 Forbidden");
				exit;	
			}
		
			if (substr($path, 0, strlen($base)) != $base || !file_exists($path)) {
				header ("HTTP/1.0 404 Not Found");
				exit;
			}
			
			$lastmodified = max($lastmodified, filemtime($path));
		}
		
		//$cachefile = 'cache-' . $hash . '.' . $type . ($encoding != 'none' ? '.' . $encoding : '');	
		$cachefile = 'cms-cache-' . $hash . '.' . $type;
		$file = $cachedir . '/' . $cachefile;
		
		if (file_exists($file)) 
		{
			if($lastmodified < filemtime($file))
			{
				//JS files not moded since cached
				Cache::headers_check_304($file);
			}
		}
		
		// Determine supported compression method
		$gzip = strstr($_SERVER['HTTP_ACCEPT_ENCODING'], 'gzip');
		$deflate = strstr($_SERVER['HTTP_ACCEPT_ENCODING'], 'deflate');

		// Determine used compression method
		$encoding = $gzip ? 'gzip' : ($deflate ? 'deflate' : 'none');

		// Check for buggy versions of Internet Explorer
		if (!strstr($_SERVER['HTTP_USER_AGENT'], 'Opera') && 
			preg_match('/^Mozilla\/4\.0 \(compatible; MSIE ([0-9]\.[0-9])/i', $_SERVER['HTTP_USER_AGENT'], $matches)) {
			$version = floatval($matches[1]);
			
			if ($version < 6)
				$encoding = 'none';
				
			if ($version == 6 && !strstr($_SERVER['HTTP_USER_AGENT'], 'EV1')) 
				$encoding = 'none';
		}
		
		// First time visit or files were modified
		if ($cache) 
		{
			if (file_exists($file)) {
				if ($fp = fopen($file, 'rb')) 
				{
					//2678400 = 31 days
					Cache::headers_allow_cache($file, true, 2678400, 2678400, true);
//					if ($encoding != 'none') { header ("Content-Encoding: " . $encoding); }
					header ('Content-type: text/' . $type);
					header ('Content-Length: ' . filesize($file));

					fpassthru($fp);
					fclose($fp);
					exit;
				}
				else
				{
					die('file not opened');
				}
			}
		}

		// Get contents of the files
		$contents = '';
		reset($elements);
		while (list(,$element) = each($elements)) {
			$path = realpath($base . '/' . $element);
			$contents .= "\n\n" . file_get_contents($path);
		}
		
		$contents = str_replace('__CMS__DOCUMENT_ROOT', Settings::singleton()->get_setting('document_root'), $contents);
		$contents = str_replace('__CMS__PAGE_ID', currentPage::singleton()->id(), $contents);
		//Remove comments from the string
		switch (Settings::Singleton()->get_setting('js_minify')) 
		{
			case '1':
				//Use internal code minification
				$contents = Common::remove_js_comments(file_get_contents($contents));
			    break;
			
			case '2':
				//Use JSMini
				$contents =  JSMin::minify(file_get_contents($contents));
			    break;
			
			default:
				$contents = $contents;
		}
		
		// Store cache
		if ($cache) {
			if ($fp = fopen($cachedir . '/' . $cachefile, 'wb')) {
				$cached = fwrite($fp, $contents);
				fclose($fp);
			}
		}
		if($cached)
		{
			Cache::headers_allow_cache($cachedir . '/' . $cachefile);
		}
	
		// Send Content-Type
		header ("Content-Type: text/" . $type);
		
		if (isset($encoding) && $encoding != 'none') 
		{
			// Send compressed contents
			$contents = gzencode($contents, 9, $gzip ? FORCE_GZIP : FORCE_DEFLATE);
			header ('Content-Encoding: ' . $encoding);
			header ('Content-Length: ' . strlen($contents));
			echo $contents;
		} 
		else 
		{
			// Send regular contents
			header ('Content-Length: ' . strlen($contents));
			echo $contents;
		}
	}
}
?>