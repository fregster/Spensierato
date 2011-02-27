<?php
Settings::Singleton()->overwrite_setting('template_extends', settings::singleton()->get_setting('skin'));
Settings::Singleton()->overwrite_setting('skin', 'iframe');