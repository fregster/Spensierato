<?
class elementContact extends elementDefault
{
	function __construct()
	{
		$this->type = __CLASS__;
		$this->name = 'Contact Details Element';
		$this->image = 'contact.png';
		$this->allowed_tags(array('name' => 'text', 'email' => 'email', 'userid' => 'numeric'));
		$this->edit_height = 600;
	}
}


?>