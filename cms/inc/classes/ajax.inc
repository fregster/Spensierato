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
		echo 'SAVED';
	}
	
	private function edit_load($id)
	{
		$element = Element::type($id);
		$element->build($id);
		echo($element->ajax_edit());		
	}
}
?>