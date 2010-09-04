<?php
require_once('mysql.inc');
class Database_mysqli extends Database_mysql
{
	protected $full_text_search = true;
	protected $escape_method = 'mysqli_real_escape_string';
}