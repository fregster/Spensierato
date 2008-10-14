<?php
/**
 * This should contain all of the maintenance methods
 */

abstract class maintenance
{
	/**
	 * Proxy to db maintance
	 */
	static public function db_optimize()
	{
		database_maintenance::Singleton()->optimize();
	}
}
?>