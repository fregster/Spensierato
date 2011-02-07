<?php
class Database_mssql extends Database
{
	protected $dsn_host_type = 'mssql';
	protected $full_text_search = true;
	
	public function sql_select($fields = NULL, $table, $where_fields = NULL, $where_values = NULL, $order_fields = NULL, $order_directions = NULL, $FORCE_NO_CACHE = FALSE, $limit = NULL)
	{
		$sql = $this->sql_select_build($fields, $table, $where_fields, $where_values, $order_fields, $order_directions);
		
		if( ($limit != NULL) and (is_numeric($limit)) )
		{			
			$sql = str_replace('SELECT ', 'SELECT TOP '.$limit.' ', $sql);
		}
		
		return $this->sql_select_exe($sql, NULL, $FORCE_NO_CACHE, $table);
	}
	
	public function sql_search($table, $col, $query, $select = NULL, $boolean = false, $where_fields = array(), $where_values = array(), $from = 0 , $to = 100, $order = 'DESC', $score = 10)
	{
		$boolean = '';
		$order = 'DESC';
		$where = $this->sql_build_where_string_part($where_fields, $where_values);
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
		$sql = 'SELECT '.$this->sql_build_select_string($select).', (MATCH(`'.$this->sql_clean_input($col)."`) AGAINST('".$this->sql_clean_input($query)."'".$boolean.") * 10) AS score FROM `".$this->prefix. $this->sql_clean_input($table).'` WHERE ( MATCH(`'.$this->sql_clean_input($col)."`) AGAINST('".$this->sql_clean_input($query)."'".$boolean.') '.$where.' ) ORDER BY score '.$order.' LIMIT '.$from.', '.$to;
		
		//MS SQL search
		/*
		 * SELECT * FROM Table WHERE FREETEXTTABLE (colname, 'value')
		 * $sql = 'SELECT '.$this->sql_build_select_string($select).' FROM `".$this->prefix. $this->sql_clean_input($table).'` WHERE FREETEXTTABLE (`'.$this->sql_clean_input($col)."` '.$this->sql_clean_input($query).') '.$where
		 */
		$result = $this->sql_exe($sql, NULL);
		return $result;
	}

	public function get_schema()
	{
//		USE YourDBName GO SELECT * FROM sys.Tables GO
		 return $this->sql_select_exe('SELECT * FROM sys.Tables', NULL);
	}
}