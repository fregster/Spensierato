<?php
require_once('mysqli.inc');
class Database_mysqlnd extends Database_mysqli
{
	protected $full_text_search = true;
	protected $escape_method = 'mysqli_real_escape_string';
}