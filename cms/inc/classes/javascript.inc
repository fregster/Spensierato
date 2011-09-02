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
		$cachedir = Settings::singleton()->get_setting('cms_folder_cache');
		$jsdir    = Settings::singleton()->get_setting('cms_root') . DS . 'js';

		//Create a hash of the file names for createing and checking a single unique cache file
		$hash = sha1($files);
		$base = realpath($jsdir);
		$type = 'javascript';
		$elements = explode(',', $files);
		
		
		$cache = false;
		if(Settings::singleton()->get_setting('js_cache'))
		{
			$cache = persistantcache::singleton()->get($hash, 'javascript');
		}		

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

		$gmdate_mod = gmdate('D, d M Y H:i:s', $lastmodified) . ' GMT';

		if(!isset($_SERVER['HTTP_ACCEPT_ENCODING'])){
			$_SERVER['HTTP_ACCEPT_ENCODING'] = false;
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

		if(isset($cache['mtime']) && isset($cache['md5']) && isset($cache['contents']) && isset($cache['moddate']))
		{
			if($lastmodified == $cache['mtime'])
			{
				/*---------- JS files not moded since cached --------------*/
				header('X-Cache-JS: Quick');
				$contents = javascript::getcontentscompressed($cache['contents'], $encoding);
				Cache::headers_allow_cache_dynamic('js', $cache['mtime'], $cache['md5'], 36000);
				javascript::sendcontent($type, $encoding, $contents);
				exit;
			}
		}

		/*---------- First time visit or files were modified --------------*/

		// Get contents of the files
		$contents = '';
		reset($elements);
		while (list(,$element) = each($elements)) {
			$path = realpath($base . '/' . $element);
			$contents .= "\n\n" . file_get_contents($path);
		}
		
		$md5 = md5($contents);
		// Store cache
		if(Settings::singleton()->get_setting('js_cache'))
		{
			persistantcache::singleton()->save($hash, array('contents' => $contents, 'mtime' => $lastmodified, 'encoding' => $encoding, 'md5' => $md5, 'moddate' => $gmdate_mod), 0, 'javascript', null, true);
		}
	
		//If the file was sucsesfully cached then send the file through the cache class
		header('X-Cache-JS: True');
		Cache::headers_allow_cache_dynamic('js', $lastmodified, $md5);
		header('X-Cache-JS: False');
		javascript::sendcontent($type, $encoding, javascript::getcontentscompressed(javascript::clean($contents), $encoding));
	}

	private static function clean ($contents)
	{
		$contents = str_replace('__CMS__DOCUMENT_ROOT', Settings::singleton()->get_setting('document_root'), $contents);
		$contents = str_replace('__CMS__PAGE_ID', currentPage::singleton()->id(), $contents);

		//Remove comments from the string
		if(!get::variable('full', 'isset'))
		{
			$notice = '/* add ?full=1 to download the full JS including licence information as parts of this JS file may be licence restricted */ ';
			return($contents);
			switch (Settings::Singleton()->get_setting('js_minify'))
			{
				case '1':
					//Use internal code minification
					$contents = $notice.Common::remove_js_comments($contents);
				    break;
	
				case '2':
					//Use external minification
					$contents = $notice.JSMin::minify($contents);
				    break;
	
				default:
					break;
			}
		}

		return($contents);
	}

	private static function getcontentscompressed($contents, $encodeing)
	{
		// Determine supported compression method
		$gzip = strstr($_SERVER['HTTP_ACCEPT_ENCODING'], 'gzip');
		$deflate = strstr($_SERVER['HTTP_ACCEPT_ENCODING'], 'deflate');

		// Determine used compression method
		$encoding = $gzip ? 'gzip' : ($deflate ? 'deflate' : 'none');

		if (isset($encoding) && $encoding != 'none')
		{
			// Send compressed contents
			header('Vary: Accept-Encoding');
			$contents=(gzencode($contents, 9, $gzip ? FORCE_GZIP : FORCE_DEFLATE));
		}

		// Send regular contents
		return $contents;
	}

	private static function sendcontent($type, $encoding, $contents)
	{
		header ('Content-Type: text/' . $type);
		if ($encoding != 'none') { header ("Content-Encoding: " . $encoding); }
		header ('Content-Length: ' . strlen($contents));
		die($contents);
	}
}