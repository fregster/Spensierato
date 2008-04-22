<?
class ajax
{
	public static function process($vars)
	{
		if($_SERVER['REQUEST_METHOD'] == 'POST')
		{
			return $this->post($vars);
		}
		
		if($_SERVER['REQUEST_METHOD'] == 'GET')
		{
			return $this->get($vars);
		}
	}

	private function post($vars)
	{
		return 'SAVED';
	}
	
	private function get($vars)
	{
		
	}
}
?>