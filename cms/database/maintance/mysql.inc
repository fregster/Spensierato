<?php
class database_maintenance_mysql implements database_maintenance
{
	public function optimize()
	{
		$alltables = database::singleton()->direct_sql('SHOW TABLES');
		
		while ($table = mysql_fetch_assoc($alltables))
		{
		   foreach ($table as $db => $tablename)
		   {
		   		database::singleton()->direct_sql("OPTIMIZE TABLE '".$tablename."'");
		   }
		}
	}
}