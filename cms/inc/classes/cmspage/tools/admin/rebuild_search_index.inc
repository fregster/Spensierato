<?php
if (!user::Singleton()->is_admin()) common::bounce_to_referer();
if(is_writable(Settings::singleton()->get_setting('cms_folder_search'))) //Is the log folder writable
{
	//Can we use lucene search?
	if(is_readable(Settings::singleton()->get_setting('cms_folder_search').DS.'search_index'.DS.'segments.gen'))
	{
		echo('<p>Deleting Old Index</p>');
		foreach (glob(Settings::singleton()->get_setting('cms_folder_search').DS.'search_index'.DS.'*', GLOB_NOSORT) as $filename) 
		{
			unlink(Settings::singleton()->get_setting('cms_folder_search').DS.'search_index'.DS.basename($filename));
		}
		
//	    $lucene_search = new Zend_Search_Lucene(Settings::singleton()->get_setting('cms_folder_search').'search_index');
	}

	
	echo('<p>Creating New Index</p>');
	new Zend_Search_Lucene(Settings::singleton()->get_setting('cms_folder_search').DS.'search_index', true);
	
	$pages = Database::Singleton()->sql_select(array('id'), 'tbl_pages');
	foreach($pages as $page)
	{
		echo('Building '.$page['id'].'<br/>');
		search::Singleton()->build_page_index($page['id']);
	}
	
//	common::bounce_to_referer();
	die('done');
	
}
echo(Settings::singleton()->get_setting('cms_folder_search'));
die('Unknown error');
?>