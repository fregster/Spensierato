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
				ajax::revision($vars[2], $vars[3]);
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

	//code is duplicated in edit::save_approvals()
	private static function approvals($element_id){
		$element_id = str_replace('e_', '', $element_id);
		$element_data = database::Singleton()->sql_select_first(array('data'), 'tbl_elements_data', array('id'), array($element_id));
		$data = $element_data['data'];
		$approvals_list = post::variable('approvals');
		if (!empty($approvals_list)) {
			$approvals = array();
			foreach ($approvals_list as $approval_id){
				$result = database::Singleton()->sql_select_first(array('data'), 'tbl_elements_approve', array('id'), array($approval_id));
				$approvals[] = $result['data'];
			}

			$approvals = array_reverse($approvals);
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

	private static function edit_load($id, $data = NULL)
	{
		$element = Element::type($id);
		$element->build($id, $data);
		return $element->ajax_edit();
	}

	private static function revision($rev_id, $element_id){
		$element_id = str_replace('e_', '', $element_id);
		$element_revs = database::singleton()->sql_select_first(array('data'), 'tbl_elements_data_archive', array('element_id'), array($element_id));
		$revision_data = $element_revs[($rev_id - 1)]['data'];
		$element = Element::type($element_id);
		$element->build($element_id, $revision_data);
		$element->save();
		echo $revision_data; //change to $element->ajax_edit() after elementDefault cleanup
	}
}
?>