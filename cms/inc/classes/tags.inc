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
	
	public static function auto_tag_page($page_id)
	{
		//FIXME:: CMSPage object seems to be a bit wrong in it's logic, needs re-writing so that I can call it directly
		//DOTO:: Remove the SQL from below
		$params['content'] = database::singleton()->sql_select_first('search', 'tbl_search', 'id', $page_id); //page content
		$params['content'] = $params['content']['search'];
		$params['min_word_length'] = 5;  //minimum length of single words
		$params['min_word_occur'] = 2;  //minimum occur of single words
		$params['min_2words_length'] = 3;  //minimum length of words for 2 word phrases
		$params['min_2words_phrase_length'] = 10; //minimum length of 2 word phrases
		$params['min_2words_phrase_occur'] = 2; //minimum occur of 2 words phrase
		$params['min_3words_length'] = 3;  //minimum length of words for 3 word phrases
		$params['min_3words_phrase_length'] = 10; //minimum length of 3 word phrases
		$params['min_3words_phrase_occur'] = 2; //minimum occur of 3 words phrase
		
		$keyword = new autokeyword($params, 'UTF-8');
		return $keyword->keywords;
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
	
	public static function get_pages_with_tags($tags, $search_elements_too = true, $products = false, $limit = 10) //TODO: Make this only pick / esclude products
	{
		if(!is_array($tags))
		{
			$tags = array($tags);
		}
		
		if(!is_int($limit)){
			$limit = 10;
		}
		
		//Force this to an AND statment
		$fields = array('tag');
		$values = array();
		
		foreach($tags as $tag) //Make this an OR statment
		{
			$values[0][] = $tag;
		}

		$page_ids = common::retrieve_values_from_array(database::singleton()->sql_select_count_group_by_col('page_id', 'tbl_tags_to_pages', 'page_id', $fields, $values), 'page_id');

		if($search_elements_too)
		{
			$pages_with_elements_tagged = common::retrieve_values_from_array(database::singleton()->sql_select_count_group_by_col('page_id', 'tbl_page_elements', 'element_id', array(tags::get_elements_with_tags($tags))), 'page_id');
			if(isset($pages_with_elements_tagged[0]))
			{
				$page_ids = array_merge($page_ids, $pages_with_elements_tagged);
			}
		}

		return array_slice(array_unique($page_ids), 0, $limit);
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