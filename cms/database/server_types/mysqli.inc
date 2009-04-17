<?php
require_once('mysql.inc');
class Database_mysqli extends Database
{
	public function sql_select($fields = NULL, $table, $where_fields = NULL, $where_values = NULL, $order_fields = NULL, $order_directions = NULL, $FORCE_NO_CACHE = FALSE, $limit = NULL)
	{
		$sql = parent::sql_select($fields, $table, $where_fields, $where_values, $order_fields, $order_directions, $FORCE_NO_CACHE, $limit);
		
		if( ($limit != NULL) and (is_numeric($limit)) )
		{
			$sql = $sql	. ' LIMIT ' . $limit;
		}

		return $this->sql_select_exe($sql, NULL, $FORCE_NO_CACHE);
	}
}
?>