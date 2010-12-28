<?php

/**
 * Copyright (C) 2008 Bartlomiej Pohl <badek@gmx.de>.
 * 
 * Name: MimeTypeGetter
 * 
 * Version: 1.0
 * Date Released: 09/24/08 
 * 
 * @author Bartlomiej Pohl <badek@gmx.de>
 * 
 * Class to get Mime Type. It checks if the fileinfo or mime_magic extension
 * is installed and uses the proper extension. If no extension is installed
 * it uses its extension list.
 * 
 * Just use MimeTypeGetter::get('path/to/your/file.ext');
 *
 * /////////////////////////////////////////////////////////////////////////
 * ////////////////////////////////WARNING!/////////////////////////////////
 * /////////////////////////////////////////////////////////////////////////
 * / The used Extension List is not complete! I just copied the List from: /
 * / http://www.validome.org/doc/HTML_ge/diverses/mimetypen.htm            /
 * /																	   /
 * / But in most cases this list should be enough.                         /
 * / Feel Free to add your own extensions to the extensions array if you   /
 * / think that it's necessary.											   /
 * /////////////////////////////////////////////////////////////////////////
 * 
 * License: This script is distributed under the BSD License, you are free
 * to use, or modify it however you like.  If you find this script useful please
 * e-mail me.    
 */

class MimeTypeGetter {
	/**
	 * the Default Mime Type for unknown extensions
	 *
	 * @var string
	 */
	protected static $default_mime_type = 'application/octet-stream';
	
	/**
	 * Array that associates file extensions with Mime Types
	 *
	 * @var array
	 */
	protected static $extensions = array(
		'dwg'     => 'application/acad',
		'asd'     => 'application/astound',
		'asn'     => 'application/astound',
		'tsp'     => 'application/dsptype',
		'dxf'     => 'application/dxf',
		'spl'     => 'application/futuresplash',
		'gz'      => 'application/gzip',
		'ptlk'    => 'application/listenup',
		'hqx'     => 'application/mac-binhex40',
		'mbd'     => 'application/mbedlet',
		'mif'     => 'application/mif',
		'xls'     => 'application/msexcel',
		'xla'     => 'application/msexcel',
		'hlp'     => 'application/mshelp',
		'chm'     => 'application/mshelp',
		'ppt'     => 'application/mspowerpoint',
		'ppz'     => 'application/mspowerpoint',
		'pps'     => 'application/mspowerpoint',
		'pot'     => 'application/mspowerpoint',
		'doc'     => 'application/msword',
		'dot'     => 'application/msword',
		'bin'     => 'application/octet-stream',
		'exe'     => 'application/octet-stream',
		'com'     => 'application/octet-stream',
		'dll'     => 'application/octet-stream',
		'class'   => 'application/octet-stream',
		'oda'     => 'application/oda',
		'pdf'     => 'application/pdf',
		'ai'      => 'application/postscript',
		'eps'     => 'application/postscript',
		'ps'      => 'application/postscript',
		'rtc'     => 'application/rtc',
		'rtf'     => 'application/rtf',
		'smp'     => 'application/studiom',
		'tbk'     => 'application/toolbook',
		'vmd'     => 'application/vocaltec-media-desc',
		'vmf'     => 'application/vocaltec-media-file',
		'htm'     => 'application/xhtml+xml',
		'html'    => 'application/xhtml+xml',
		'shtml'   => 'application/xhtml+xml',
		'xhtml'   => 'application/xhtml+xml',
		'xml'     => 'application/xml',
		'bcpio'   => 'application/x-bcpio',
		'z'       => 'application/x-compress',
		'cpio'    => 'application/x-cpio',
		'csh'     => 'application/x-csh',
		'dcr'     => 'application/x-director',
		'dir'     => 'application/x-director',
		'dxr'     => 'application/x-director',
		'dvi'     => 'application/x-dvi',
		'evy'     => 'application/x-envoy',
		'gtar'    => 'application/x-gtar',
		'hdf'     => 'application/x-hdf',
		'php'     => 'application/x-httpd-php',
		'phtml'   => 'application/x-httpd-php',
		'js'      => 'application/x-javascript',
		'latex'   => 'application/x-latex',
		'bin'     => 'application/x-macbinary',
		'mif'     => 'application/x-mif',
		'nc'      => 'application/x-netcdf',
		'cdf'     => 'application/x-netcdf',
		'nsc'     => 'application/x-nschat',
		'sh'      => 'application/x-sh',
		'shar'    => 'application/x-shar',
		'swf'     => 'application/x-shockwave-flash',
		'cab'     => 'application/x-shockwave-flash',
		'spr'     => 'application/x-sprite',
		'sprite'  => 'application/x-sprite',
		'sit'     => 'application/x-stuffit',
		'sca'     => 'application/x-supercard',
		'sv4cpio' => 'application/x-sv4cpio',
		'sv4crc'  => 'application/x-sv4crc',
		'tar'     => 'application/x-tar',
		'tcl'     => 'application/x-tcl',
		'tex'     => 'application/x-tex',
		'texinfo' => 'application/x-texinfo',
		'texi'    => 'application/x-texinfo',
		't'       => 'application/x-troff',
		'tr'      => 'application/x-troff',
		'roff'    => 'application/x-troff',
		'man'     => 'application/x-troff-man',
		'troff'   => 'application/x-troff-man',
		'me'      => 'application/x-troff-me',
		'troff'   => 'application/x-troff-me',
		'me'      => 'application/x-troff-ms',
		'troff'   => 'application/x-troff-ms',
		'ustar'   => 'application/x-ustar',
		'src'     => 'application/x-wais-source',
		'zip'     => 'application/zip',
		'au'      => 'audio/basic',
		'snd'     => 'audio/basic',
		'es'      => 'audio/echospeech',
		'tsi'     => 'audio/tsplayer',
		'vox'     => 'audio/voxware',
		'aif'     => 'audio/x-aiff',
		'aiff'    => 'audio/x-aiff',
		'aifc'    => 'audio/x-aiff',
		'dus'     => 'audio/x-dspeeh',
		'cht'     => 'audio/x-dspeeh',
		'mid'     => 'audio/x-midi',
		'midi'    => 'audio/x-midi',
		'mp2'     => 'audio/x-mpeg',
		'ram'     => 'audio/x-pn-realaudio',
		'ra'      => 'audio/x-pn-realaudio',
		'rpm'     => 'audio/x-pn-realaudio-plugin',
		'stream'  => 'audio/x-qt-stream',
		'wav'     => 'audio/x-wav',
		'dwf'     => 'drawing/x-dwf',
		'cod'     => 'image/cis-cod',
		'ras'     => 'image/cmu-raster',
		'fif'     => 'image/fif',
		'gif'     => 'image/gif',
		'ief'     => 'image/ief',
		'jpeg'    => 'image/jpeg',
		'jpg'     => 'image/jpeg',
		'jpe'     => 'image/jpeg',
		'png'     => 'image/png',
		'tiff'    => 'image/tiff',
		'tif'     => 'image/tiff',
		'mcf'     => 'image/vasa',
		'fh4'     => 'image/x-freehand',
		'fh5'     => 'image/x-freehand',
		'fhc'     => 'image/x-freehand',
		'pnm'     => 'image/x-portable-anymap',
		'pbm'     => 'image/x-portable-bitmap',
		'pgm'     => 'image/x-portable-graymap',
		'ppm'     => 'image/x-portable-pixmap',
		'rgb'     => 'image/x-rgb',
		'xwd'     => 'image/x-windowdump',
		'xbm'     => 'image/x-xbitmap',
		'xpm'     => 'image/x-xpixmap',
		'wrl'     => 'model/vrml',
		'csv'     => 'text/comma-separated-values',
		'css'     => 'text/css',
		'htm'     => 'text/html',
		'html'    => 'text/html',
		'shtml'   => 'text/html',
		'js'      => 'text/javascript',
		'txt'     => 'text/plain',
		'rtx'     => 'text/richtext',
		'rtf'     => 'text/rtf',
		'tsv'     => 'text/tab-separated-values',
		'wml'     => 'text/vnd.wap.wml',
		'wmlc'    => 'application/vnd.wap.wmlc',
		'wmls'    => 'text/vnd.wap.wmlscript',
		'wmlsc'   => 'application/vnd.wap.wmlscriptc',
		'xml'     => 'text/xml',
		'etx'     => 'text/x-setext',
		'sgm'     => 'text/x-sgml',
		'sgml'    => 'text/x-sgml',
		'talk'    => 'text/x-speech',
		'spc'     => 'text/x-speech',
		'mpeg'    => 'video/mpeg',
		'mpg'     => 'video/mpeg',
		'mpe'     => 'video/mpeg',
		'qt'      => 'video/quicktime',
		'mov'     => 'video/quicktime',
		'viv'     => 'video/vnd.vivo',
		'vivo'    => 'video/vnd.vivo',
		'avi'     => 'video/x-msvideo',
		'movie'   => 'video/x-sgi-movie',
		'vts'     => 'workbook/formulaone',
		'vtts'    => 'workbook/formulaone',
		'3dmf'    => 'x-world/x-3dmf',
		'3dm'     => 'x-world/x-3dmf',
		'qd3d'    => 'x-world/x-3dmf',
		'qd3'     => 'x-world/x-3dmf',
		'wrl'     => 'x-world/x-vrml'
	);
	
	/**
	 * Gets the Mime Type using the Fileinfo Extension.
	 * If the Extension returns nothing the extension list
	 * is used.
	 *
	 * @param string $file the path to the File
	 * @return string the Mime Type
	 */
	protected static function getTypeFinfoExt($file)
	{
		$finfo = @finfo_open(FILEINFO_MIME);
		$type  = @finfo_file($finfo, $file);
		
		if($type != '') {
			return $type;
		}
		else {
			return self::getTypeFileExtList($file);
		}
	}
	
	/**
	 * Gets the Mime Type using the mime_magic extension.
	 * If the function returns nothing the extension list
	 * is used.
	 *
	 * @param string $file the path to the file
	 * @return string the Mime Type
	 */
	protected static function getTypeMimeExt($file)
	{
		$type = @mime_content_type($file);
		
		if($type != '') {
			return $type;
		}
		else {
			return self::getTypeFileExtList($file);
		}
	}
	
	/**
	 * extracts the File extension and checks the extension array
	 * for the extension. If it's found it returns the Mime Type.
	 * If not it returns the Default Mime Type.
	 *
	 * @param string $file the path to the file
	 * @return string
	 */
	protected static function getTypeFileExtList($file)
	{
		$info = pathinfo($file);
		
		if(@isset($info['extension']) && @isset(self::$extensions[$info['extension']])) {
			return self::$extensions[$info['extension']];
		}
		else {
			return self::$default_mime_type;
		}
	}
	
	/**
	 * Gets the Mime type for a given File. It first checks which
	 * extension is installed and uses it. If no extension is loaded
	 * it gets the Type with the extension list.
	 * 
	 * You can force the function to use a specific extension by using
	 * the optional $force_extension param.
	 * 
	 * Accepted Values:
	 * - fileinfo
	 * - mime_magic
	 *
	 * @param string $file the path to the File
	 * @param string $force_extension (optional) Forces specific extension.
	 * @return string the Mime Type
	 */
	public static function get($file, $force_extension=false)
	{
		if($force_extension == false) {
			if (extension_loaded('fileinfo')) {
				return self::getTypeFinfoExt($file);
			}
			else if (extension_loaded('mime_magic')) {
				return self::getTypeMimeExt($file);
			}
			else {
				return self::getTypeFileExtList($file);
			}
		}
		else {
			switch($force_extension) {
				case 'fileinfo'  : return self::getTypeFinfoExt($file);
				case 'mime_magic': return self::getTypeMimeExt($file);
				default          : return self::getTypeFileExtList($file);
			}
		}
	}
}