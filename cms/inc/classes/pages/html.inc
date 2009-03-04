<?php
class cmspage_html extends cmspage
{
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
					$value = database::singleton()->sql_select_first(array('id'), 'tbl_pages', NULL, NULL, array('id'), 'ASC');
					if(is_numeric($value['id']))
					{
						$this->value = $value['id'];
					}
					else
					{
						//FIXME currently wont work with an empty DB
						$this->value = 0;
					}
				}
			}
			
			//Get current page details
			$this->page_details = Database::Singleton()->sql_select_first(array('*'), 'tbl_pages', array('id'), array($this->id));
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
}
?>