<?php
class cmspage_editsave extends cmspage
{
	public function initalise()
	{
		$this->value = CMS::Singleton()->path_vars(1);
		return true;
	}

	public function run()
	{
		//Is this an update or createing a new element
		if($this->value == 'account')
		{
			User::Singleton()->update_from_post();
			Common::redirect('account');
		}

		if(post::variable('new_type', 'is_set'))
		{
			$this->security_page_edit();

			if(post::variable('new_type') === 'newPage')
			{
				$this->security_page_approve();
				$this->create_new_page($this->value);
			}
			else
			{
				$element = Element::type(post::variable('new_type', 'string'));
				$element->create($this->value);
			}
		}
		else
		{
			$this->security_element_edit();

			$element = Element::type($this->value);
			$element->build($this->value);
			if($element->save(false))
			{
				//Save worked bounce to main page for element
				$this->saved($element->main());
			}
			else
			{
				//Save failed show error page
			}
		}
		common::redirect($this->value);
	}

	private function create_new_page($parent_id)
	{
		if($parent_id === '-1')
		{
			if(Security::Singleton()->is_admin())
			{
				$id = database::Singleton()->sql_insert('tbl_pages', array('created', 'creator', 'title'), array(time(), user::Singleton()->id(), 'Homepage'));
			}
		}
		else
		{
			$sort = database::Singleton()->sql_max_field('tbl_pages', 'sort', array('parent_id'), array($parent_id));
			$id = database::Singleton()->sql_insert('tbl_pages', array('parent_id', 'created', 'creator'), array($parent_id, time(), user::Singleton()->id()));
		}
		common::redirect($id);
	}
}
?>