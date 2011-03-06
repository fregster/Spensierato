<?php
cookie::singleton()->debug();

var_dump($_COOKIE);
var_dump($_SESSION);

echo(Settings::Singleton()->get_setting('document_root'));