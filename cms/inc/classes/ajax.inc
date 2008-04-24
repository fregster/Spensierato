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
		if($_SERVER['REQUEST_METHOD'] == 'POST')
		{
			ajax::edit_post($vars);
		}
		
		if($_SERVER['REQUEST_METHOD'] == 'GET')
		{
			ajax::edit_load($vars);
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
		echo($element->render());		
	}
}
?>