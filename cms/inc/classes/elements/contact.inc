<?
element::register('Page Contact', 'elementContact');
class elementContact extends elementDefault
{
	function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Page Contact';
		$this->image = 'contact.png';
		$this->image_edit = '';
		$this->allowed_tags(array('name' => 'text', 'email' => 'email', 'userid' => 'numeric'));
	}

}
?>