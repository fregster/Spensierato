<?php
class cmspage_move extends cmspage
{
	private $page;
	private $way;
	
	public function initalise()
	{
		$this->value = CMS::Singleton()->path_vars(1);
		$this->page = CMS::Singleton()->path_vars(2);
		$this->way = CMS::Singleton()->path_vars(3);
		if($this->way == 'u' || $this->way == 'd')
		{
			return true;
		}
		return false;
	}
	
	public function run()
	{
		$data = database::singleton()->sql_select_first(array('element_id', 'page_id', 'sort'), 'tbl_page_elements', array('element_id', 'page_id'), array($this->value, $this->page));
		
		if($this->way === 'u' && $data['sort'] > 1)
		{
			$new_sort = $data['sort'] - 1;
		}
		else if($this->way === 'd')
		{
			$count = database::Singleton()->sql_count_where('tbl_page_elements', array('page_id'), array($this->page));
			if($data['sort'] < $count)
			{
				$new_sort = $data['sort'] + 1;
			}
		}
		
		if($new_sort)
		{
			$swap_id = database::singleton()->sql_select_first(array('element_id'), 'tbl_page_elements', array('page_id', 'sort'), array($this->page, $new_sort));
			if($swap_id)
			{
				database::singleton()->sql_update_by_col('tbl_page_elements', array('element_id', 'page_id'), array($swap_id['element_id'], $this->page), array('sort'), array($data['sort']));
				database::singleton()->sql_update_by_col('tbl_page_elements', array('element_id', 'page_id'), array($this->value, $this->page), array('sort'), array($new_sort));
				Common::bounce_to_referer();
			}
		}
		die('There was an error trying to move your content');
	}
}