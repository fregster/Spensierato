<?php
echo('Settings file created');
if(!masterpassword::Singleton()->set_password(true) || !masterpassword::Singleton()->set_password_secondary(true))
{
	echo('<strong>Random Encryption key file output failed or they already exsisted. Check for there exsistance or fix folder permissions and re run the tool</strong>');
	exit;
}

echo 'Random Encryption key files created';