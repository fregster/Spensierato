<?
class elementAccount extends elementDefault
{
	function __construct()
	{
		$this->type = __CLASS__;
		$this->name = 'Account Information Element';
		$this->image = 'account.png';
	}
	
	protected function get_data()
	{
		/*
		$html = 'Account information';
		$this->data = '<data>'. htmlentities($html, ENT_QUOTES, Settings::Singleton()->get_setting('char_set')) .'</data>';
		$this->search = NULL;
		*/
		
		$this->data = '
		<data>
			<lists>
				<list>1</list>
			</lists>
		</data>';
	}
}


?>