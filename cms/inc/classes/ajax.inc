<?
class ajax
{
	public static function process($vars)
	{
		switch ($vars[1]) 
		{
			case 'edit':
				ajax::edit($vars[2]);
			break;
			
			case 'register':
				if(Capthca::Code_check($_POST['security_code']))
				{
					register::username_check($_POST['username']);
				}
			break;

			case 'login':
				user::Singleton()->login();
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
		
//		if(database::Singleton()->sql_update_or_insert('tbl_elements_draft', array('element_id', 'user_id'), array($id, user::singleton()->id()), array('element_id', 'user_id', 'ts', 'data'), array($id, user::singleton()->id(), time(), $_POST['html'])))
//		{
//			echo 'SAVED';
//		}
	}
	
	private function edit_load($id)
	{
		$element = Element::type($id);
		$element->build($id);
		echo($element->ajax_edit());		
	}
	
}
?>