<?php
// return_hash($var)
// Returns the has of an input based on the site global var


//These functions need to be moved into the classes

function build_menu_items($level, $parentid)
{
	$pages = Database::Singleton()->sql_select(array('id', 'title', 'page'), 'tbl_pages', array('parentid', 'visible', 'deleted'), array($parentid, '1', '0'), array('sort'));
	return $pages;
}