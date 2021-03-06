<?php
/**
* Spensierato
* Content Managment System
*
* Requirements: PHP5.2, SimpleXML, Tidy, MDB2 / PDO with MySQLi /SQLite (Depending on install), XML/Serializer, PHP xsltProcessor
*
* Copyright (c) 2008 Paul Fryer (www.fryer.org.uk)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; version 3 or any latter version of the license.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
*
* @category CMS
* @package Spensierato
* @author Paul Fryer <paul@fryer.org.uk>
* @license http://www.opensource.org/licenses/gpl-3.0.html GPL
* @link http://www.spensierato.net/
*/
//$force_install = true;
error_reporting(E_ERROR | E_WARNING | E_PARSE | E_NOTICE);
ini_set('display_errors', 1);
$include_path = dirname(__FILE__);
require_once($include_path.'/../spensierato_cms.inc');