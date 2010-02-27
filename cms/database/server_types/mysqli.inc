<?php
require_once('mysql.inc');
class Database_mysqli extends Database_mysql
{
	protected $escape_method = 'mysqli_real_escape_string';
}