<?php
class cmspage_html extends cmspage
{
	protected $tags;
	
	public function initalise()
	{
		if(CMS::Singleton()->page_exists(CMS::Singleton()->path_vars(0)) == true)
		{
			if(!isset($this->value))
			{
				$this->value = CMS::Singleton()->path_vars(0);
			}
			
			if($this->value == NULL or !is_numeric($this->value))
			{	
				if(Settings::Singleton()->get_setting('homepage'))
				{
					$this->value = Settings::Singleton()->get_setting('homepage');
				}
				else
				{
					$this->page_details = database::singleton()->sql_select_first(array('*'), 'tbl_pages', NULL, NULL, array('id'), 'ASC');
					
					if(is_numeric($this->page_details['id']))
					{
						$this->value = $this->page_details['id'];
					}
					else
					{
						//FIXME currently wont work with an empty DB
						$this->value = 0;
					}
				}
			}
			
			//Get current page details if not already found
			if(!isset($this->page_details['id']) and isset($this->value) and is_numeric($this->value))
			{
				$this->page_details = Database::Singleton()->sql_select_first(array('*'), 'tbl_pages', array('id'), array($this->value));
			}

			if(isset($this->page_details['append_tags']) && strlen($this->page_details['append_tags']) > 0)
			{
				$this->page_details['append_tags'] = explode(',',$this->page_details['append_tags']);
				if(isset($this->page_details['append_tags'][0])) //Clean up the tags
				{
					foreach($this->page_details['append_tags'] as $key => $tag)
					{
						$this->page_details['append_tags'][$key] = trim($tag);
						if(strlen($this->page_details['append_tags'][$key]) < 1)
						{
							unset($this->page_details['append_tags'][$key]);							
						}
					}
				}
			}
			
			
			if(isset($this->page_details['css']) && strlen($this->page_details['css']) > 0)
			{
				$this->page_details['css'] = explode(',',$this->page_details['append_tags']);
				if(isset($this->page_details['css'][0])) //Clean up the tags
				{
					foreach($this->page_details['css'] as $key => $tag)
					{
						$this->page_details['css'][$key] = trim($tag);
						if(strlen($this->page_details['css'][$key]) < 1)
						{
							unset($this->page_details['css'][$key]);							
						}
					}
						
					$inherirted_css = Settings::Singleton()->get_setting('template_inherits_css');
					if($inherirted_css)
					{
						if(is_array($inherirted_css))
						{
							$inherirted_css = array_merge($include, $this->page_details['css']);
						}
						else
						{
							$inherirted_css[] = $this->page_details['css'];
						}
					}
					Settings::Singleton()->overwrite_setting('template_inherits_css', $inherirted_css, false, true);
				}
			}
			
			return true;
		}
		return false;
	}
	
	public function run()
	{
		//Database file, call content singleton
		$this->security_page_view();
		
		Options::singleton()->admin_check();
		Render::Singleton()->render_html(Content::Singleton()->build(), true);
	}
	
	public function get_tags()
	{
		return $this->tags;
	}
	
	public function get_menu_children()
	{
		if(isset($this->page_details['append_tags'][0]))
		{
			return tags::get_pages_with_tags($this->page_details['append_tags'], false);
		}
		return array();
	}
}