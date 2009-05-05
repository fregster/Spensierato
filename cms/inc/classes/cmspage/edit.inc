<?php
class cmspage_edit extends cmspage
{
	public function initalise()
	{
		$this->page_details = Database::Singleton()->sql_select_first(array('*'), 'tbl_pages', array('id'), array($this->value[1]));

		$this->value = CMS::Singleton()->path_vars;

		foreach ($this->value as $val){
			if (is_numeric($val)) {
				$this->id = $val;
				break;
			}
		}

		return true;
	}

	public function run()
	{
		if($_SERVER['REQUEST_METHOD'] == 'POST')
		{
			if ($this->value[1] == 'update') {
				$this->update();
			}
			else if (post::variable('save_approvals', 'isset')) {
				$this->save_approvals();
			}
			else $this->save();
		}
		else
		{
			if($this->value[1] == 'toggle')
			{
				$this->toggle();
			}
			else $this->edit();
		}
	}

	private function edit(){


		//Edit render element
		$this->security_element_edit();

		$element = Element::type($this->value[1]);
		$element->build($this->value[1]);
		Render::Singleton()->render_edit(Content::Singleton()->build());
	}

	//FIXME: most of the code in this method is duplicated from ajax::approvals(),
	//and some from the element saving process.
	private function save_approvals(){
		$this->security_element_approve();

		$element_data = database::singleton()->sql_select_first(array('data', 'title'), 'tbl_elements_data', array('id'), array($this->value[1]));

		//archive old data
		database::singleton()->sql_insert('tbl_elements_data_archive', array('data', 'title', 'ts', 'replaced_by', 'replaced_ts', 'element_id'), array($element_data['data'], $element_data['title'], time(), user::singleton()->id(), time(), $this->value[1]));

		$data = $element_data['data'];
		$approvals_list = post::variable('approvals');
		if (!empty($approvals_list)) {
			$approvals = array();
			foreach ($approvals_list as $approval_id){
				$result = database::Singleton()->sql_select_first(array('data'), 'tbl_elements_approve', array('id'), array($approval_id));
				$approvals[] = $result['data'];
			}

			$approvals = array_reverse($approvals);

			$data = diffCompare::multi_diff($data, $approvals, TRUE);

			$data = '<html>'.utf8_encode(common::sql_clean(strip_tags(common::html($data)))).'</html>';

			database::singleton()->sql_update_by_id($this->value[1], 'tbl_elements_data', array('data'), array($data));
			database::singleton()->sql_update_by_id($this->value[1], 'tbl_elements', array('approved', 'approver'), array(time(), user::singleton()->id()));

			foreach ($approvals_list as $approval_id){
				database::Singleton()->sql_update_by_id($approval_id, 'tbl_elements_approve', array('approved'), array('1'));
			}

		}

		//reload the current element to continue editing
		common::redirect('/'.implode('/', $this->value));
	}

	private function save(){
		//Is this an update or createing a new element
		if($this->value[1] == 'account')
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
				$this->create_new_page($this->value[1]);
			}
			else
			{
				$element = Element::type(post::variable('new_type', 'string'));
				$element->create($this->value[1]);
			}
		}
		else
		{
			$this->security_element_edit();

			$element = Element::type($this->value[1]);
			$element->build($this->value[1]);
			if($element->save(false))
			{
				$this->saved($element->main());
			}
			else
			{
				//Save failed show error page
			}
		}
		common::redirect($this->value[1]);
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

	private function toggle(){
		if ($this->value[2] == 'page') {
			$this->toggle_page();
		}
		else if ($this->value[2] == 'element') {
			$this->toggle_element();
		}
		else error::do403();
	}

	private function toggle_page(){
		if(Security::singleton()->can_approve($this->value[3]))
		{
			if($this->value[4] == 'visibility')
			{
				database::singleton()->sql_toggle_field_by_id($this->value[3], 'tbl_pages', 'visible');
			}

			if($this->value[4] == 'delete')
			{
				database::singleton()->sql_toggle_field_by_id($this->value[3], 'tbl_pages', 'deleted');
			}
			common::redirect($this->value[3]);
		}
		else
		{
			error::do403();
		}
	}

	private function toggle_element(){
		if(Security::singleton()->can_approve_element($this->value[3]))
		{
			$element = Element::type($this->value[3]);
			$element->build($this->value[3]);

			$element->toggle($this->value[4]);
			common::redirect($element->main());
		}
		else
		{
			error::do403();
		}
	}

	private function update(){
		if(Security::singleton()->can_approve($this->value[2]))
		{
			if(post::variable('title', 'is_set'))
			{
				database::singleton()->sql_update_by_id($this->value[2], 'tbl_pages', array('title'), array(post::variable('title', 'sql')));
			}
			common::redirect($this->value[2]);
		}
		else
		{
			error::do403();
		}
	}

	private function saved($main)
	{
		die('
			<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
			<html xmlns="http://www.w3.org/1999/xhtml">
			<head>
			<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
			<title>Saved</title>
			<link rel="stylesheet" type="text/css" href="style.css" />
			<script type="text/javascript">
		window.top.hidePopWin(true);
			</script>
			</head>
			<body>
			<p>
			Your updates have been saved.<br />
			You should be redirected back the page the element you changed is on in a few seconds<br/>
			If not click <a href="'.common::document_root().'/'.$main.'">here</a>
			</p>

			</body>
			</html>');
	}
}
?>