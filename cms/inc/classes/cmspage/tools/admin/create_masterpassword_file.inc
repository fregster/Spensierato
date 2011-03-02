<?php
if (!user::Singleton()->is_logged_in() || !user::Singleton()->is_admin()) common::bounce_to_referer();
if(!masterpassword::Singleton()->set_password(true) || !masterpassword::Singleton()->set_password_secondary(true))
{
	echo('<strong>Random Encryption key file output failed. Fix folder permissions and re run the installer</strong>');
	exit;
}

echo'Random Encryption key files created';