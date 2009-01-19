<?php
/**
 * Default storage engine
 * 
 * Use MyISAM for a small performance improvment over InnoDB in light situations
 * 
 * Use InnoDB anywhere where you need clustering, caching data, higher concurrency, background flushes 
 * 
 * @var db type string
 */
$default_table_engine = 'MyISAM';
//$default_table_engine = 'InnoDB';
//$archive_table_engine = 'ARCHIVE';
$archive_table_engine = 'MyISAM';
$full_text_search_table_engine = 'MyISAM';

$cache_table_engine = 'MEMORY';

$sql = array();

$sql_str = '
CREATE TABLE `cms_tbl_commands` (
  `command` varchar(32) NOT NULL,
  `type` varchar(32) NOT NULL,
  `instruction` varchar(256) NOT NULL,
  PRIMARY KEY  (`command`)
) ENGINE='.$default_table_engine.' DEFAULT CHARSET=latin1;';
array_push($sql, $sql_str);

$sql_str = '
CREATE TABLE `cms_tbl_element_data_archive` (
  `id` int(10) unsigned NOT NULL,
  `data` longtext,
  `ts` varchar(45) NOT NULL,
  `replaced_by` int(10) unsigned NOT NULL,
  `replaced_ts` varchar(45) NOT NULL,
  `element_id` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE='.$archive_table_engine.' DEFAULT CHARSET=latin1;';
array_push($sql, $sql_str);

$sql_str = "
CREATE TABLE `cms_tbl_elements` (
  `id` int(11) NOT NULL auto_increment,
  `type` varchar(32) NOT NULL,
  `title` varchar(128) NOT NULL default 'Please give the element a meaningful title',
  `created` varchar(32) NOT NULL default '0',
  `creator` int(11) NOT NULL,
  `edited` varchar(32) default NULL,
  `editor` int(11) default NULL,
  `visible` tinyint(3) unsigned NOT NULL default '1',
  `deleted` varchar(32) NOT NULL default '0',
  `approved` varchar(32) default NULL,
  `approver` int(10) unsigned default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=$default_table_engine DEFAULT CHARSET=latin1 AUTO_INCREMENT=10 ;";
array_push($sql, $sql_str);

$sql_str = '
CREATE TABLE `cms_tbl_elements_approve` (
  `id` int(11) NOT NULL auto_increment,
  `element_id` int(11) NOT NULL,
  `user_id` int(11) NOT NULL,
  `ts` varchar(64) NOT NULL,
  `data` longtext NOT NULL,
  PRIMARY KEY  (`id`),
  UNIQUE KEY `User to Element` (`element_id`,`user_id`)
) ENGINE='.$default_table_engine.' DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;';
array_push($sql, $sql_str);

$sql_str = '
CREATE TABLE `cms_tbl_elements_data` (
  `id` int(11) NOT NULL,
  `data` longtext NOT NULL,
  `search` longtext NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE='.$full_text_search_table_engine.' DEFAULT CHARSET=latin1;';
array_push($sql, $sql_str);

$sql_str = '
CREATE TABLE `cms_tbl_elements_draft` (
  `id` int(11) NOT NULL auto_increment,
  `element_id` int(11) NOT NULL,
  `user_id` int(11) NOT NULL,
  `ts` varchar(64) NOT NULL,
  `data` longtext NOT NULL,
  PRIMARY KEY  (`id`),
  UNIQUE KEY `User to Element` (`element_id`,`user_id`)
) ENGINE='.$default_table_engine.' DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;';
array_push($sql, $sql_str);

$sql_str = '
CREATE TABLE `cms_tbl_list_data` (
  `id` int(11) NOT NULL auto_increment,
  `name` varchar(128) NOT NULL,
  `valid_from` varchar(48) default NULL,
  `valid_to` varchar(48) NOT NULL,
  `created` varchar(48) NOT NULL,
  `created_by` int(11) NOT NULL,
  `edited` varchar(48) default NULL,
  `edited_by` int(11) default NULL,
  `visible` tinyint(4) NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE='.$default_table_engine.' DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;';
array_push($sql, $sql_str);

$sql_str = '
CREATE TABLE `cms_tbl_list_membership` (
  `user_id` int(11) NOT NULL,
  `list_id` int(11) NOT NULL,
  UNIQUE KEY `user_id` (`user_id`,`list_id`)
) ENGINE='.$default_table_engine.' DEFAULT CHARSET=latin1;';
array_push($sql, $sql_str);

$sql_str = "
CREATE TABLE `cms_tbl_page_elements` (
  `id` int(11) NOT NULL auto_increment,
  `page_id` int(11) NOT NULL,
  `element_id` int(11) NOT NULL,
  `sort` int(11) NOT NULL,
  `main` tinyint(4) NOT NULL default '1',
  `ts` varchar(32) NOT NULL,
  `visible` tinyint(3) unsigned NOT NULL default '1',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `elements_to_page_link` (`page_id`,`element_id`),
  UNIQUE KEY `only_one_main_element` (`element_id`,`main`)
) ENGINE=$default_table_engine  DEFAULT CHARSET=latin1 AUTO_INCREMENT=10 ;";
array_push($sql, $sql_str);

$sql_str = "
CREATE TABLE `cms_tbl_pages` (
  `id` int(11) NOT NULL auto_increment,
  `parent_id` int(11) NOT NULL,
  `title` varchar(64) NOT NULL default 'New Page',
  `visible` tinyint(4) NOT NULL default '0',
  `deleted` tinyint(4) NOT NULL default '0',
  `sort` int(11) default NULL,
  `created` varchar(45) NOT NULL,
  `creator` int(10) unsigned NOT NULL,
  `edited` varchar(45) default NULL,
  `editor` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`id`),
  KEY `valid_child` (`visible`,`deleted`,`parent_id`)
) ENGINE=$default_table_engine DEFAULT CHARSET=latin1 AUTO_INCREMENT=11 ;";
array_push($sql, $sql_str);

$sql_str = '
CREATE TABLE `cms_tbl_security` (
  `page_id` int(11) NOT NULL,
  `data` longtext NOT NULL,
  PRIMARY KEY  (`page_id`)
) ENGINE='.$default_table_engine.' DEFAULT CHARSET=latin1;';
array_push($sql, $sql_str);

$sql_str = '
CREATE TABLE `cms_tbl_settings` (
  `setting` varchar(32) NOT NULL,
  `value` varchar(128) NOT NULL,
  PRIMARY KEY  (`setting`)
) ENGINE='.$default_table_engine.' DEFAULT CHARSET=latin1;';
array_push($sql, $sql_str);

$sql_str = '
CREATE TABLE `cms_tbl_temp` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `data` text NOT NULL,
  `ts` varchar(45) NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE='.$default_table_engine.' DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;';
array_push($sql, $sql_str);

$sql_str = "
CREATE TABLE `cms_tbl_users` (
  `id` int(11) NOT NULL auto_increment,
  `username` varchar(32) NOT NULL,
  `password` varchar(64) NOT NULL,
  `hash_type` varchar(8) NOT NULL,
  `last_login` varchar(32) NOT NULL,
  `session_id` varchar(64) NOT NULL,
  `admin` tinyint(4) default NULL COMMENT 'Global site admin',
  `login_attempts` tinyint(4) unsigned NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `username` (`username`),
  KEY `login` (`username`,`password`)
) ENGINE=$default_table_engine DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;";
array_push($sql, $sql_str);

$sql_str = '
CREATE TABLE `cms_tbl_users_data` (
  `id` int(11) NOT NULL auto_increment,
  `user_id` int(11) NOT NULL,
  `data` longtext NOT NULL,
  PRIMARY KEY  (`id`),
  KEY `id` (`user_id`)
) ENGINE='.$default_table_engine.'  DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;';
array_push($sql, $sql_str);

unset($sql_str);
?>