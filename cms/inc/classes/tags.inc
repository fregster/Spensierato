<?php
class tags
{	
	protected static $protected_tags = array('cms_product'); //List of tags only the CMS can add to an object
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
		
		return common::retrieve_values_from_array(database::singleton()->sql_select(array('element_id'), 'tbl_tags_to_elements', array('tag'), array($tags)), 'page_id');
	}
	
	public static function get_tags_on_page($page, $search_elements_too = true)
	{
		$tags = array();
		$tags_page = database::singleton()->sql_select('tag', 'tbl_tags_to_pages', array('page_id'), array($page));
		if(is_array($tags_page))
		{
			foreach($tags_page as $tag)
			{
				$tags['page'][] = $tag['tag'];
			}
		}
		
		if($search_elements_too)
		{
			$elements = content::Singleton()->list_of_elements();
			if($elements)
			{
				$tags_elements_on_page = database::singleton()->sql_select('tag', 'tbl_tags_to_elements', array('element_id'), array($elements));
				
				if(is_array($tags_elements_on_page))
				{
					foreach($tags_elements_on_page as $tag)
					{
						$tags['elements'][] = $tag['tag'];
					}
				}
			}
		}
		
		return $tags;
	}
	
	public static function get_pages_with_tags($tags, $search_elements_too = true)
	{
		if(!is_array($tags))
		{
			$tags = array($tags);
		}
		
		//Force this to an AND statment
		$fields = array('tag');
		$values = array();
		foreach($tags as $tag)
		{
			$values[0][] = $tag;
		}
		
		$page_ids = common::retrieve_values_from_array(database::singleton()->sql_select_count_group_by_col('page_id', 'tbl_tags_to_pages', 'page_id', $fields, $values, false, count($values[0])), 'page_id');

		if($search_elements_too)
		{
			$pages_with_elements_tagged = common::retrieve_values_from_array(database::singleton()->sql_select_count_group_by_col('page_id', 'tbl_page_elements', 'element_id', array(tags::get_elements_with_tags($tags))), 'page_id');
			if(isset($pages_with_elements_tagged[0]))
			{
				$page_ids = array_merge($page_ids, $pages_with_elements_tagged);
			}
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