<?
class ajax
{
	public static function process($vars)
	{
		switch ($vars[1]) 
		{
			case 'edit':
				$this->edit($vars[2]);
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
			$this->edit_post($vars);
		}
		
		if($_SERVER['REQUEST_METHOD'] == 'GET')
		{
			$this->edit_load($vars);
		}
	}
	
	private function edit_post($id)
	{
		echo 'SAVED';
	}
	
	private function edit_load($id)
	{
		$element = Element::type(id);
		$element->build(id);
		echo('<ajax>'.$element->render().'</ajax>');		
	}
}
?>