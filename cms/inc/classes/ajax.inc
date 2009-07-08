<?php
abstract class ajax
{
	public static function process($vars)
	{
		switch ($vars[2])
		{
			case 'edit':
				self::edit($vars[2]);
				break;

			case 'revision':
				self::get_revision($vars[2], $vars[3]);
				break;

			case 'approvals':
				self::view_approvals($vars[2]);
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
				
			case 'search':
				header('content-type: text/xml');
				die(search::singleton()->search(get::variable('searchtext'), null, 5, 0, false, false));

			default:
				print_r($vars);
				die();
				break;
		}
	}

	private static function view_approvals($element_id){
		$element_id = str_replace('e_', '', $element_id);
		$revision_ids = post::variable('approvals');

		$revisions = database::singleton()->sql_select(array('*'), 'tbl_elements_revisions', array('id', 'element_id'), array($revision_ids, $element_id));

		$data = revisions::diff_data($element_id, $revisions);

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
		$element_revs = database::singleton()->sql_select_first(array('data'), 'tbl_elements_revisions', array('id', 'element_id'), array($rev_id, $element_id));

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