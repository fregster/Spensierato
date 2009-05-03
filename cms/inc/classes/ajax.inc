<?php
abstract class ajax
{
	public static function process($vars)
	{
		switch ($vars[1])
		{
			case 'edit':
				ajax::edit($vars[2]);
				break;

			case 'revision':
				ajax::get_revision($vars[2], $vars[3]);
				break;

			case 'approvals':
				ajax::approvals($vars[2]);
				break;

			case 'register':
				if(Capthca::Code_check(post::variable('security_code', 'is_set')))
				{
					register::username_check(post::variable('username', 'string'));
				}
				break;

			case 'login':
				enhancedLogin::Singleton()->login();
				break;

			default:
				print_r($vars);
				die();
				break;
		}
	}

	//FIXME: code is duplicated in edit::save_approvals()
	private static function approvals($element_id){
		$element_id = str_replace('e_', '', $element_id);

		$element_data = database::Singleton()->sql_select_first(array('data'), 'tbl_elements_data', array('id'), array($element_id));
		$data = json_decode($element_data['data'], TRUE);

		if (empty($data)) {
			$data = '';
		}
		else{
			$data = $data['html'];
		}

		$approvals_list = post::variable('approvals');
		if (!empty($approvals_list)) {
			$approvals = array();

			foreach ($approvals_list as $approval_id){
				$result = database::Singleton()->sql_select_first(array('data'), 'tbl_elements_approve', array('id'), array($approval_id), array('id'), array('ASC'));
				$result = json_decode($result['data'], TRUE);

				if (empty($result)) {
					$result = '';
				}
				else{
					$result = $result['html'];
				}

				$approvals[] = $result;
			}

			$data = diffCompare::multi_diff($data, $approvals);
		}
		die($data);
	}

	private static function edit($id)
	{
		//remove the e_ from form inputs
		$id = str_replace('e_', '', $id);
		if($_SERVER['REQUEST_METHOD'] == 'POST')
		{
			echo ajax::edit_post($id);
		}

		if($_SERVER['REQUEST_METHOD'] == 'GET')
		{
			echo ajax::edit_load($id);
		}
	}

	private static function edit_post($id)
	{
		$element = Element::type($id);
		$element->build($id);
		return $element->save(true);
	}

	private static function edit_load($id)
	{
		$element = Element::type($id);
		$element->build($id);
		$data = $element->reload_element_data();

		if ($data != NULL) {
			$data = json_decode(html_entity_decode($data['data']), TRUE);
			$data = $data['html'];
		}
		return $data;
	}

	//FIXME: make get_revision display the title as well as the content
	private static function get_revision($rev_id, $element_id){
		$element_id = str_replace('e_', '', $element_id);
		$element_revs = database::singleton()->sql_select_first(array('data'), 'tbl_elements_data_archive', array('id', 'element_id'), array($rev_id, $element_id));

		if ($element_revs == NULL) {
			die(ajax::edit_load($element_id));
		}
		else{
			$rev_data = json_decode($element_revs['data'], TRUE);
			die(html_entity_decode($rev_data['html']));
		}
	}
}
?>