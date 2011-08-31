<?php

class Database_mysql extends Database
{
	protected $escape_method = 'mysql_real_escape_string';
	
	public function sql_select($fields = NULL, $table, $where_fields = NULL, $where_values = NULL, $order_fields = NULL, $order_directions = NULL, $FORCE_NO_CACHE = FALSE, $limit = NULL)
	{
		$id = $this->sql_id();
		$sql = $this->sql_select_build($fields, $table, $where_fields, $where_values, $order_fields, $order_directions, $id);		
		if( ($limit != NULL) and (is_numeric($limit)) )
		{
			$sql = $sql	. ' LIMIT ' . $limit;
		}

		return $this->sql_select_exe($sql, NULL, $FORCE_NO_CACHE, $table, $id);
	}
	
	public function sql_search($table, $col, $query, $select = NULL, $boolean = false, $where_fields = array(), $where_values = array(), $from = 0 , $to = 100, $order = 'DESC', $score = 10)
	{
		$id = $this->sql_id();
		if(!$this->fts())
		{
			return false;
		}
		$boolean = '';
		$order = 'DESC';
		$where = $this->sql_build_where_string_part($where_fields, $where_values, $id);
		if(strlen($where) > 0)
		{
			$where = ' AND '.$where;
		}

		if($boolean != false)
		{
			$boolean = ' IN BOOLEAN MODE';
		}

		if($order != 'DESC')
		{
			$order = 'ASC';
		}

		if(!is_numeric($score))
		{
			$score = 10;
		}
		
		//Mysql search
		$sql = 'SELECT '.$this->sql_build_select_string($select).', (MATCH('.$this->sql_clean_input($col, $this->col_quote).') AGAINST('.$this->sql_clean_input($query, '\'').$boolean.') * 10) AS score FROM '.$this->sql_clean_input($this->prefix.$table, $this->table_quote).' WHERE ( MATCH('.$this->sql_clean_input($col, $this->col_quote).') AGAINST('.$this->sql_clean_input($query, '\'').$boolean.') '.$where.' ) ORDER BY score '.$order.' LIMIT '.$from.', '.$to;
		return database_execution::singleton()->sql_select_exe_run($this->sql_exe($sql, NULL, NULL, NULL, $id));
	}
	
	public function get_schema()
	{
//		USE YourDBName GO SELECT * FROM sys.Tables GO
		 return $this->sql_select_exe('SELECT * FROM sys.Tables', NULL);
	}
	
	/*
	protected function sql_clean_input($var)
	{
		//If the input is just a number just return it
		if(is_numeric($var)) { return $var; }
		
		return call_user_func('addslashes',$var); //FIXME mysql uses a reference in the function needs fixing.
	}
	*/
}