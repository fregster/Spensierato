<?php
if (!user::Singleton()->is_logged_in() || !user::Singleton()->is_admin()) common::bounce_to_referer();
persistantcache::singleton()->garbage_collection(true);
die('Done');