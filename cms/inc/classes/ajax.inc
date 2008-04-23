<?
class ajax
{
	public static function process($vars)
	{
		switch ($vars[0]) 
		{
			case 'edit':
				$this->edit($vars[1]);
			break;
			
			default:
				print_r($vars);
				die();
			break;
		}
	}

	private function post($vars)
	{
		return 'SAVED';
	}
	
	private function get($vars)
	{
		$element = Element::type($vars);
		$element->build($vars);
		echo('<ajax>'.$element->render().'</ajax>');		
	}
}
?>