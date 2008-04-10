<?
if($_SESSION['site_login'])
{
	$db_user = $GLOBALS['database_user_readwrite'];
}
else
{
	$db_user = $GLOBALS['database_user_read'];
}


$dsn = array(
    'phptype'  => $GLOBALS['database_driver'],
    'username' => $db_user,
    'password' => $GLOBALS['database_password'],
    'hostspec' => $GLOBALS['database_host'],
    'database' => $GLOBALS['database_name'],
);


$mdb2 =& MDB2::singleton($dsn, $options);
if (PEAR::isError($mdb2)) 
{
    die($mdb2->getMessage());
}
?>