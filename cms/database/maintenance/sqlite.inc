<?php

require_once 'mysqli.inc';

class database_maintenance_mysqli extends database_maintenance_mysql
{
	public $default_table_engine = false;
	public $archive_table_engine = false;
	public $full_text_search_table_engine = false;
	public $cache_table_engine = false;
	public $default_charset = false;
	public $full_text_enabled = false;
	public $allow_unique_keys = false;
	
	public $long_text = 'TEXT';
	public $full_text = 'TEXT';
	public $tiny_int = 'INTEGER';
	public $int = 'INTEGER';
	public $increment = 'AUTOINCREMENT';
	public $varchar = 'TEXT';
	protected $primary_key_in_define = true;
	protected $strip_unsigned_ints = true;
	
	protected $create_db_users = false;
	
}