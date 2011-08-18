<?php

require_once 'mysqli.inc';

class database_maintenance_mysqli extends database_maintenance_mysql
{
	public $int = 'BIGINT';
	public $increment = 'IDENTITY';
}