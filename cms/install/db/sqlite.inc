<?php

$default_table_engine = false;
$archive_table_engine = false;
$full_text_search_table_engine = false;
$cache_table_engine = false;
$default_charset = false;
$full_text_enabled = false;
$allow_unique_keys = false;

$long_text = 'TEXT';
$full_text = 'TEXT';
$tiny_int = 'INTEGER';
$int = 'INTEGER';
$increment = 'AUTOINCREMENT';
$varchar = 'TEXT';
$primary_key_in_define = true;
$strip_unsigned_ints = true;
$disable_keys = true;

if(!isset($col_quote))
{
	$col_quote = '"';
}

require_once 'mysqli.inc';

$create_db_users = false;