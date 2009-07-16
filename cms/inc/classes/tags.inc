<?php
class tags
{	
	public static function get_objects_with_tags($tags)
	{
		if(!is_array($tags))
		{
			$tags = array($tags);
		}
		
		return array('pages' => tags::get_pages_with_tags($tags), 'elements' => tags::get_elements_with_tags($tags));
	}
	
	public static function get_elements_with_tags($tags)
	{
		if(!is_array($tags))
		{
			$tags = array($tags);
		}
		
		return common::retrieve_values_from_array(database::singleton()->sql_select(array('page_id'), 'tbl_tags_to_elements', array('tag'), array($tags)), 'page_id');
	}
	
	public static function get_pages_with_tags($tags, $search_elements_too = true)
	{
		if(!is_array($tags))
		{
			$tags = array($tags);
		}
		
		$page_ids = common::retrieve_values_from_array(database::singleton()->sql_select(array('element_id'), 'tbl_tags_to_pages', array('tag'), array($tags)), 'element_id');
		
		if($search_elements_too)
		{
			$page_ids = array_merge($page_ids, common::retrieve_values_from_array(database::singleton()->sql_select(array('page_id'), 'tbl_page_elements', array('element_id'), array(get_elements_with_tags($tags))), 'page_id'));
		}
		return $page_ids;
	}
	
	public static function get_tags_related_to_tag($tag)
	{
		$tags_from_pages = database::singleton()->sql_select_count_group_by_col('tag', 'tbl_tags_to_pages', 'tag', array('page_id'), array(tags::get_pages_with_tags($tag)));
		$tags_from_elements = database::singleton()->sql_select_count_group_by_col('tag', 'tbl_tags_to_elements', 'tag', array('element_id'), array(tags::get_elements_with_tags($tag)));
		
		$results = array();
		
		if(isset($tags_from_pages[0]))
		{
			foreach($tags_from_pages as $result)
			{
				$results[$result['tag']] = $result['count'];
			}
		}
		
		if(isset($tags_from_elements[0]))
		{
			foreach($tags_from_elements as $result)
			{
				if(isset($results[$result['tag']]))
				{
					$results[$result['tag']] = $results[$result['tag']] + $result['count'];
				}
				else
				{
					$results[$result['tag']] = $result['count'];	
				}
			}
		}
		
		asort($results);
		return($results);
	}
}
?>