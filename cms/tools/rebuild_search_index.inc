<?php
if (!user::Singleton()->is_admin()) common::bounce_to_referer();
if(is_writable(Settings::singleton()->get_setting('cms_folder_logs'))) //Is the log folder writable
{
	//Can we use lucene search?
	if(is_readable(Settings::singleton()->get_setting('cms_folder_logs').'search_index/segments.gen'))
	{
		echo('<p>Deleting Old Index</p>');
		unlink(Settings::singleton()->get_setting('cms_folder_logs').'search_index/*');
//	    $lucene_search = new Zend_Search_Lucene(Settings::singleton()->get_setting('cms_folder_logs').'search_index');
	}

	
	echo('<p>Creating New Index</p>');
	new Zend_Search_Lucene(Settings::singleton()->get_setting('cms_folder_logs').'search_index', true);
	
	$pages = Database::Singleton()->sql_select(array('page_id'), 'tbl_page_elements', array('element_id'), array($id));
	foreach($pages as $page)
	{
		$this->build_page_index($page['page_id']);
	}
	
//	common::bounce_to_referer();
	die('done');
	
}
echo(Settings::singleton()->get_setting('cms_folder_logs'));
die('Unknown error');
?>