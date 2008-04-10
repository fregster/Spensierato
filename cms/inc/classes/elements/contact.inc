<?
class elementContact extends elementDefault
{
	function __construct()
	{
		$this->type = __CLASS__;
		$this->name = 'Contact Details Element';
		$this->image = 'contact.png';
		$this->allowed_tags(array('name', 'email', 'userid'));
	}
}


?>