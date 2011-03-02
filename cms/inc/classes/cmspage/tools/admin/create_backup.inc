<?php
/**
 * This tool should provide a single download zip
 * the zip should contant standard files
 * database in the format required to restore through the application with the same DB exe mode (PDO, MDB2)
 * spensierato_options.inc
 * spensierato_setup.inc
 * encryption key files
 * custom dir tar
 * skin dir tar
 * 
 * optional: all writeable files / downloads
 * 
 * require: passwords to encrypt the contents of the files with in the zip
 */
Database::singleton()->backupDatabase();