<?php
/**
 * Main database bootstrapper
 * 
 * 
 * Requirements: PHP5
 * Copyright (c) 2011 Paul Fryer (www.fryer.org.uk)
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the The GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3 or any latter version of the license.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * The GNU Lesser General Public License (LGPLv3) for more details.
 * 
 * 
 * @package PDA (PHP Database Abstraction)
 * @author Paul Fryer <paul@fryer.org.uk>
 * @license http://www.opensource.org/licenses/lgpl-3.0.html LGPL
 *
 */

//Supported database connection methods
$db_backends = array('mdb2', 'pdo', 'php'); //PHP is mysql, sqlite only

//Supported database type drivers
$db_drivers = array('mysqli', 'mysql', 'mssql', 'sqlite');

//Set the required defines
require_once 'static.inc';

//Load the required files for the database layer
require_once(DATABASE_LAYER_PATH.DS.'backends'.DS.'database_execution.inc');

//Load the main database API
require_once(DATABASE_LAYER_PATH.DS.'layer.inc');

//Load the maintenance class
require_once(DATABASE_LAYER_PATH.DS.'maintenance'.DS.'database_maintenance.inc');

//Clean up
unset($db_backends);
unset($db_drivers);