<?php
//if (!user::Singleton()->is_admin()) common::bounce_to_referer();

if(is_writable(Settings::singleton()->get_setting('cms_folder_logs'))) //Is the log folder writable
{
	//Can we use lucene search?
	if(is_readable(Settings::singleton()->get_setting('cms_folder_logs').'search_index/segments.gen'))
	{
		echo('Updating ');
	    $lucene_search = new Zend_Search_Lucene(Settings::singleton()->get_setting('cms_folder_logs').'search_index');
	}
	else
	{
		echo('Creating ');
		$lucene_search = new Zend_Search_Lucene(Settings::singleton()->get_setting('cms_folder_logs').'search_index', true);
	}
	
	if($_GET['purge'])
	{
		for ($count = 0; $count < $lucene_search->maxDoc(); $count++)
		{
			$lucene_search->delete($count);
		    if($lucene_search->isDeleted($count)) 
		    {
		        echo "Document #$count is deleted.\n";
		    }
		}
	}
	
	//Get all the search text from the db
	$result = database::singleton()->sql_select(array('id', 'title', 'search'), 'tbl_elements_data');
	foreach($result as $item)
	{
		$doc = new Zend_Search_Lucene_Document();
		$doc->addField(Zend_Search_Lucene_Field::Keyword('element_id', $item['id']));
		$doc->addField(Zend_Search_Lucene_Field::Text('title', $item['title']));
		$doc->addField(Zend_Search_Lucene_Field::Text('search', $item['search']));
		
		$l_id = $lucene_search->find('element_id:' . $item['id']);
		if(is_numeric($l_id->id)) { $lucene_search->delete($l_id->id); }//Ensure we delete the old record
		
		$lucene_search->addDocument($doc);
	}
	
	$lucene_search->optimize();
	
	die('done');
	
}
echo(Settings::singleton()->get_setting('cms_folder_logs'));
die('Unknown error');
?>