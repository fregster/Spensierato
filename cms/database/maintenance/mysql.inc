<?php

class database_maintenance_mysql extends database_maintenance
{
	/**
	 * Default storage engine
	 *
	 * Use MyISAM for a small performance improvment over InnoDB in light situations
	 * Use InnoDB anywhere where you need clustering, caching data, higher concurrency, background flushes
	 */

	//public $default_table_engine = 'MYISAM';
	public $default_table_engine = 'INNODB';
	public $archive_table_engine = 'ARCHIVE';
	//public $archive_table_engine = 'MYISAM';
	public $full_text_search_table_engine = 'MYISAM';
	public $cache_table_engine = 'MEMORY';
	public $default_charset = 'latin1';
	public $long_text = 'LONGTEXT';
	public $tiny_int = 'TINYINT';
	public $full_text = 'FULLTEXT';
	public $full_text_enabled = true;
	public $allow_unique_keys = true;
	
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