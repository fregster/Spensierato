<?php
class ajax
{
	public static function process($vars)
	{
		switch ($vars[1])
		{
			case 'edit':
				ajax::edit($vars[2]);
			break;

			case 'rev':
				ajax::revision($vars[2], $vars[3]);
				break;

			case 'register':
				if(Capthca::Code_check(post::variable('is_set','security_code')))
				{
					register::username_check(post::variable('string','username'));
				}
			break;

			case 'login':
				enhanced_login::Singleton()->login();
			break;

			default:
				print_r($vars);
				die();
			break;
		}
	}

	private function edit($id)
	{
		//remove the e_ from form inputs
		$id = str_replace('e_', '', $id);
		if($_SERVER['REQUEST_METHOD'] == 'POST')
		{
			ajax::edit_post($id);
		}

		if($_SERVER['REQUEST_METHOD'] == 'GET')
		{
			ajax::edit_load($id);
		}
	}

	private function edit_post($id)
	{
		$element = Element::type($id);
		$element->build($id);
		echo($element->save(true));
	}

	private function edit_load($id)
	{
		$element = Element::type($id);
		$element->build($id);
		echo($element->ajax_edit());
	}

	private function revision($rev_id, $element_id){
		$element_revs = database::singleton()->sql_select_first(array('data'), 'tbl_elements_archive_data', array('element_id'), array($element_id));
		$revision_data = $element_revs[($rev_id - 1)]['data'];
		$element = Element::type($element_id);
		$element->build($element_id, $revision_data);
		$element->save();
		echo $revision_data; //change to $element->ajax_edit() after elementDefault cleanup
	}
}
?>